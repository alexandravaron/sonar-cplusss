#include <iostream>
#include <string>
#include <vector>
#include "database_manager.h"
#include "user_service.h"
#include "vulnerable_queries.h"

void demonstrateVulnerabilities() {
    std::cout << "=== SQL Injection Vulnerability Demonstration ===" << std::endl;
    
    // Initialize database
    DatabaseManager db("test.db");
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return;
    }
    
    UserService userService(&db);
    
    std::cout << "\n1. Testing vulnerable user lookup:" << std::endl;
    // This would be vulnerable to: 1 OR 1=1--
    std::vector<std::string> users = db.getUserByIdVulnerable("1 OR 1=1");
    for (const auto& user : users) {
        std::cout << "User: " << user << std::endl;
    }
    
    std::cout << "\n2. Testing vulnerable search:" << std::endl;
    // This would be vulnerable to: %' OR '1'='1
    std::vector<std::string> search_results = db.searchUsersVulnerable("admin' OR '1'='1");
    for (const auto& result : search_results) {
        std::cout << "Search result: " << result << std::endl;
    }
    
    std::cout << "\n3. Testing vulnerable login:" << std::endl;
    // This would be vulnerable to: admin'--
    std::vector<std::string> login_result = userService.loginUser("admin'--", "wrong_password");
    for (const auto& result : login_result) {
        std::cout << "Login result: " << result << std::endl;
    }
    
    std::cout << "\n4. Testing vulnerable query building:" << std::endl;
    std::string malicious_query = VulnerableQueries::buildLoginQuery("admin' OR '1'='1'--", "password");
    std::cout << "Generated query: " << malicious_query << std::endl;
    
    std::cout << "\n5. Testing vulnerable web request processing:" << std::endl;
    std::string malicious_request = "user_id=1 UNION SELECT username,password,email,role,id FROM users--";
    std::vector<std::string> web_results = userService.processWebRequest(malicious_request);
    for (const auto& result : web_results) {
        std::cout << "Web result: " << result << std::endl;
    }
    
    std::cout << "\n6. Testing vulnerable API call:" << std::endl;
    std::string malicious_api = "{\"status\":\"admin\",\"filter\":\"1=1 OR role='admin'\"}";
    std::vector<std::string> api_results = userService.handleApiCall(malicious_api);
    for (const auto& result : api_results) {
        std::cout << "API result: " << result << std::endl;
    }
    
    std::cout << "\n7. Testing vulnerable admin report:" << std::endl;
    // This demonstrates ORDER BY injection and other advanced techniques
    std::string malicious_filter = "u.role='user' ORDER BY (SELECT CASE WHEN (SELECT COUNT(*) FROM users WHERE role='admin')>0 THEN username ELSE email END)";
    std::vector<std::string> report_results = userService.getAdminReport(malicious_filter);
    for (const auto& result : report_results) {
        std::cout << "Report result: " << result << std::endl;
    }
    
    std::cout << "\n8. Testing second-order injection:" << std::endl;
    std::string stored_malicious = "user123' UNION SELECT password FROM users WHERE username='admin'--";
    std::string second_order_query = VulnerableQueries::buildSecondOrderQuery(stored_malicious, "web");
    std::cout << "Second-order query: " << second_order_query << std::endl;
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    std::cout << "Note: These vulnerabilities should be detected by SonarQube's taint analysis." << std::endl;
}

void demonstrateSafeMethods() {
    std::cout << "\n=== Safe Method Demonstration ===" << std::endl;
    
    DatabaseManager db("test.db");
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return;
    }
    
    std::cout << "\nTesting safe user lookup:" << std::endl;
    std::vector<std::string> safe_users = db.getUserByIdSafe(1);
    for (const auto& user : safe_users) {
        std::cout << "Safe user: " << user << std::endl;
    }
    
    std::cout << "\nTesting safe email update:" << std::endl;
    bool update_result = db.updateUserEmailSafe(1, "newemail@example.com");
    std::cout << "Update successful: " << (update_result ? "Yes" : "No") << std::endl;
}

int main() {
    std::cout << "SQL Injection Vulnerability Demo for SonarQube Analysis" << std::endl;
    std::cout << "======================================================" << std::endl;
    
    try {
        demonstrateVulnerabilities();
        demonstrateSafeMethods();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
