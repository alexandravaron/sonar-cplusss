#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

class DatabaseManager {
private:
    sqlite3* db;
    std::string db_path;

public:
    DatabaseManager(const std::string& database_path);
    ~DatabaseManager();
    
    bool initialize();
    void close();
    
    // Vulnerable methods - direct string concatenation
    std::vector<std::string> getUserByIdVulnerable(const std::string& user_id);
    std::vector<std::string> searchUsersVulnerable(const std::string& search_term);
    bool updateUserEmailVulnerable(const std::string& user_id, const std::string& email);
    bool deleteUserVulnerable(const std::string& user_id);
    
    // More vulnerable patterns
    std::vector<std::string> executeRawQueryVulnerable(const std::string& query);
    std::vector<std::string> getOrdersVulnerable(const std::string& status, const std::string& user_input);
    
    // Safe methods (for comparison)
    std::vector<std::string> getUserByIdSafe(int user_id);
    bool updateUserEmailSafe(int user_id, const std::string& email);
    
private:
    std::vector<std::string> executeQuery(const std::string& query);
    void createTables();
};
