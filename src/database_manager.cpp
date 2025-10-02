#include "database_manager.h"
#include <iostream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string& database_path) 
    : db(nullptr), db_path(database_path) {
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::initialize() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    createTables();
    return true;
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void DatabaseManager::createTables() {
    const char* create_users_table = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            email TEXT NOT NULL,
            password TEXT NOT NULL,
            role TEXT DEFAULT 'user'
        );
    )";
    
    const char* create_orders_table = R"(
        CREATE TABLE IF NOT EXISTS orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            product_name TEXT,
            status TEXT,
            amount REAL,
            FOREIGN KEY(user_id) REFERENCES users(id)
        );
    )";
    
    sqlite3_exec(db, create_users_table, nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_orders_table, nullptr, nullptr, nullptr);
    
    // Insert some test data
    const char* insert_data = R"(
        INSERT OR IGNORE INTO users (id, username, email, password, role) VALUES 
        (1, 'admin', 'admin@example.com', 'admin123', 'admin'),
        (2, 'user1', 'user1@example.com', 'password1', 'user'),
        (3, 'user2', 'user2@example.com', 'password2', 'user');
        
        INSERT OR IGNORE INTO orders (user_id, product_name, status, amount) VALUES
        (1, 'Laptop', 'completed', 999.99),
        (2, 'Mouse', 'pending', 29.99),
        (3, 'Keyboard', 'shipped', 79.99);
    )";
    
    sqlite3_exec(db, insert_data, nullptr, nullptr, nullptr);
}

// VULNERABLE: Direct string concatenation - SonarQube should detect this
std::vector<std::string> DatabaseManager::getUserByIdVulnerable(const std::string& user_id) {
    // This is vulnerable to SQL injection
    std::string query = "SELECT * FROM users WHERE id = " + user_id;
    return executeQuery(query);
}

// VULNERABLE: String concatenation with LIKE operator
std::vector<std::string> DatabaseManager::searchUsersVulnerable(const std::string& search_term) {
    // This allows injection through the search term
    std::string query = "SELECT username, email FROM users WHERE username LIKE '%" + search_term + "%'";
    return executeQuery(query);
}

// VULNERABLE: Direct concatenation in UPDATE statement
bool DatabaseManager::updateUserEmailVulnerable(const std::string& user_id, const std::string& email) {
    // Both parameters are vulnerable to injection
    std::string query = "UPDATE users SET email = '" + email + "' WHERE id = " + user_id;
    std::vector<std::string> result = executeQuery(query);
    return !result.empty();
}

// VULNERABLE: DELETE with string concatenation
bool DatabaseManager::deleteUserVulnerable(const std::string& user_id) {
    // Direct concatenation allows injection
    std::string query = "DELETE FROM users WHERE id = " + user_id;
    std::vector<std::string> result = executeQuery(query);
    return !result.empty();
}

// VULNERABLE: Executing raw user-provided queries
std::vector<std::string> DatabaseManager::executeRawQueryVulnerable(const std::string& query) {
    // This is extremely dangerous - executing user input directly
    return executeQuery(query);
}

// VULNERABLE: Multiple concatenations
std::vector<std::string> DatabaseManager::getOrdersVulnerable(const std::string& status, const std::string& user_input) {
    // Multiple injection points
    std::string query = "SELECT o.*, u.username FROM orders o JOIN users u ON o.user_id = u.id WHERE o.status = '" + status + "' AND " + user_input;
    return executeQuery(query);
}

// SAFE: Using parameterized queries (for comparison)
std::vector<std::string> DatabaseManager::getUserByIdSafe(int user_id) {
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM users WHERE id = ?";
    
    std::vector<std::string> results;
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string row;
            for (int i = 0; i < sqlite3_column_count(stmt); i++) {
                const char* value = (const char*)sqlite3_column_text(stmt, i);
                row += (value ? value : "NULL");
                if (i < sqlite3_column_count(stmt) - 1) row += "|";
            }
            results.push_back(row);
        }
        
        sqlite3_finalize(stmt);
    }
    
    return results;
}

// SAFE: Parameterized update
bool DatabaseManager::updateUserEmailSafe(int user_id, const std::string& email) {
    sqlite3_stmt* stmt;
    const char* query = "UPDATE users SET email = ? WHERE id = ?";
    
    bool success = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, user_id);
        
        success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
    }
    
    return success;
}

std::vector<std::string> DatabaseManager::executeQuery(const std::string& query) {
    std::vector<std::string> results;
    char* error_msg = nullptr;
    
    auto callback = [](void* data, int argc, char** argv, char** col_names) -> int {
        auto* results = static_cast<std::vector<std::string>*>(data);
        std::string row;
        
        for (int i = 0; i < argc; i++) {
            row += (argv[i] ? argv[i] : "NULL");
            if (i < argc - 1) row += "|";
        }
        
        results->push_back(row);
        return 0;
    };
    
    int rc = sqlite3_exec(db, query.c_str(), callback, &results, &error_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << error_msg << std::endl;
        sqlite3_free(error_msg);
    }
    
    return results;
}
