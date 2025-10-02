#pragma once

#include <string>
#include <vector>
#include "database_manager.h"

class UserService {
private:
    DatabaseManager* db_manager;

public:
    UserService(DatabaseManager* db);
    ~UserService();
    
    // Vulnerable service methods that accept user input
    std::vector<std::string> loginUser(const std::string& username, const std::string& password);
    std::vector<std::string> findUsersByName(const std::string& name_pattern);
    bool changeUserPassword(const std::string& username, const std::string& old_password, const std::string& new_password);
    std::vector<std::string> getUserProfile(const std::string& user_identifier);
    
    // Admin functions with vulnerabilities
    std::vector<std::string> getAdminReport(const std::string& filter_criteria);
    bool executeAdminQuery(const std::string& admin_query);
    
    // Methods that demonstrate different taint sources
    std::vector<std::string> processWebRequest(const std::string& request_data);
    std::vector<std::string> handleApiCall(const std::string& api_params);
};
