#include "user_service.h"
#include <iostream>
#include <sstream>

UserService::UserService(DatabaseManager* db) : db_manager(db) {
}

UserService::~UserService() {
    // Don't delete db_manager as it's managed externally
}

// VULNERABLE: Login bypass through SQL injection
std::vector<std::string> UserService::loginUser(const std::string& username, const std::string& password) {
    // Classic SQL injection vulnerability - can be bypassed with ' OR '1'='1' --
    std::string query = "SELECT id, username, role FROM users WHERE username = '" + username + 
                       "' AND password = '" + password + "'";
    return db_manager->executeRawQueryVulnerable(query);
}

// VULNERABLE: LIKE injection with wildcards
std::vector<std::string> UserService::findUsersByName(const std::string& name_pattern) {
    // Allows injection through LIKE patterns and can expose all users
    std::string query = "SELECT username, email FROM users WHERE username LIKE '" + name_pattern + "'";
    return db_manager->executeRawQueryVulnerable(query);
}

// VULNERABLE: Password change with multiple injection points
bool UserService::changeUserPassword(const std::string& username, const std::string& old_password, const std::string& new_password) {
    // Multiple injection points in WHERE and SET clauses
    std::string query = "UPDATE users SET password = '" + new_password + 
                       "' WHERE username = '" + username + "' AND password = '" + old_password + "'";
    std::vector<std::string> result = db_manager->executeRawQueryVulnerable(query);
    return !result.empty();
}

// VULNERABLE: Dynamic column/table access
std::vector<std::string> UserService::getUserProfile(const std::string& user_identifier) {
    // This could allow access to any column or even other tables
    std::string query;
    
    // Check if it's numeric (assuming it's an ID) or string (assuming it's username)
    if (user_identifier.find_first_not_of("0123456789") == std::string::npos) {
        // Numeric - treat as ID
        query = "SELECT * FROM users WHERE id = " + user_identifier;
    } else {
        // String - treat as username, but vulnerable to injection
        query = "SELECT * FROM users WHERE username = '" + user_identifier + "'";
    }
    
    return db_manager->executeRawQueryVulnerable(query);
}

// VULNERABLE: Admin function with ORDER BY injection
std::vector<std::string> UserService::getAdminReport(const std::string& filter_criteria) {
    // Allows injection in ORDER BY and WHERE clauses
    std::string base_query = "SELECT u.username, u.email, u.role, COUNT(o.id) as order_count "
                            "FROM users u LEFT JOIN orders o ON u.id = o.user_id ";
    
    std::string query = base_query + "WHERE " + filter_criteria + " GROUP BY u.id";
    return db_manager->executeRawQueryVulnerable(query);
}

// VULNERABLE: Direct query execution for admin
bool UserService::executeAdminQuery(const std::string& admin_query) {
    // Extremely dangerous - allows any SQL execution
    std::vector<std::string> result = db_manager->executeRawQueryVulnerable(admin_query);
    return !result.empty();
}

// VULNERABLE: Processing web request data
std::vector<std::string> UserService::processWebRequest(const std::string& request_data) {
    // Simulating processing of web request data that could contain malicious SQL
    std::string decoded_data = request_data; // In real app, this would be URL decoded
    
    // Extract user ID from request data (vulnerable parsing)
    size_t pos = decoded_data.find("user_id=");
    if (pos != std::string::npos) {
        std::string user_id = decoded_data.substr(pos + 8); // Skip "user_id="
        
        // Find end of parameter (& or end of string)
        size_t end_pos = user_id.find('&');
        if (end_pos != std::string::npos) {
            user_id = user_id.substr(0, end_pos);
        }
        
        // Vulnerable query construction
        std::string query = "SELECT * FROM users WHERE id = " + user_id;
        return db_manager->executeRawQueryVulnerable(query);
    }
    
    return std::vector<std::string>();
}

// VULNERABLE: API parameter injection
std::vector<std::string> UserService::handleApiCall(const std::string& api_params) {
    // Simulating API parameter processing with JSON-like syntax
    std::string query = "SELECT * FROM users WHERE 1=1";
    
    // Parse parameters and add to query (vulnerable)
    if (api_params.find("status") != std::string::npos) {
        size_t start = api_params.find("status\":\"") + 9;
        size_t end = api_params.find("\"", start);
        if (end != std::string::npos) {
            std::string status = api_params.substr(start, end - start);
            query += " AND role = '" + status + "'";
        }
    }
    
    if (api_params.find("filter") != std::string::npos) {
        size_t start = api_params.find("filter\":\"") + 9;
        size_t end = api_params.find("\"", start);
        if (end != std::string::npos) {
            std::string filter = api_params.substr(start, end - start);
            query += " AND " + filter; // Direct injection point
        }
    }
    
    return db_manager->executeRawQueryVulnerable(query);
}
