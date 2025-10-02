#pragma once

#include <string>
#include <vector>

class VulnerableQueries {
public:
    // Static methods demonstrating various SQL injection patterns
    static std::string buildLoginQuery(const std::string& username, const std::string& password);
    static std::string buildSearchQuery(const std::string& search_term, const std::string& table_name);
    static std::string buildUpdateQuery(const std::string& table, const std::string& column, 
                                      const std::string& value, const std::string& condition);
    static std::string buildDeleteQuery(const std::string& table, const std::string& where_clause);
    
    // Union-based injection patterns
    static std::string buildUnionQuery(const std::string& base_query, const std::string& union_part);
    
    // Time-based blind injection patterns
    static std::string buildTimeBasedQuery(const std::string& condition, const std::string& delay_value);
    
    // Boolean-based blind injection patterns
    static std::string buildBooleanQuery(const std::string& base_condition, const std::string& injection_condition);
    
    // Error-based injection patterns
    static std::string buildErrorBasedQuery(const std::string& table, const std::string& malicious_input);
    
    // Second-order injection patterns
    static std::string buildSecondOrderQuery(const std::string& stored_value, const std::string& context);
    
    // NoSQL-style injection in SQL context
    static std::string buildJsonQuery(const std::string& json_input, const std::string& field);
};
