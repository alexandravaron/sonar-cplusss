#include "vulnerable_queries.h"
#include <sstream>

// VULNERABLE: Basic login query with string concatenation
std::string VulnerableQueries::buildLoginQuery(const std::string& username, const std::string& password) {
    // Classic injection point - can be bypassed with admin'--
    return "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'";
}

// VULNERABLE: Search query with LIKE operator
std::string VulnerableQueries::buildSearchQuery(const std::string& search_term, const std::string& table_name) {
    // Both parameters are injection points
    return "SELECT * FROM " + table_name + " WHERE name LIKE '%" + search_term + "%'";
}

// VULNERABLE: Dynamic UPDATE query construction
std::string VulnerableQueries::buildUpdateQuery(const std::string& table, const std::string& column, 
                                               const std::string& value, const std::string& condition) {
    // Multiple injection points in table, column, value, and condition
    return "UPDATE " + table + " SET " + column + " = '" + value + "' WHERE " + condition;
}

// VULNERABLE: DELETE query with user-controlled WHERE clause
std::string VulnerableQueries::buildDeleteQuery(const std::string& table, const std::string& where_clause) {
    // Can delete from any table with any condition
    return "DELETE FROM " + table + " WHERE " + where_clause;
}

// VULNERABLE: UNION-based injection pattern
std::string VulnerableQueries::buildUnionQuery(const std::string& base_query, const std::string& union_part) {
    // Allows data extraction through UNION SELECT
    return base_query + " UNION " + union_part;
}

// VULNERABLE: Time-based blind injection
std::string VulnerableQueries::buildTimeBasedQuery(const std::string& condition, const std::string& delay_value) {
    // Enables time-based blind SQL injection attacks
    std::string query = "SELECT * FROM users WHERE id = 1";
    
    if (!condition.empty()) {
        // This allows injection of time-based payloads like: 1; WAITFOR DELAY '00:00:05'--
        query += " AND " + condition;
    }
    
    if (!delay_value.empty()) {
        // SQLite doesn't have WAITFOR, but this simulates the pattern
        query += " AND (SELECT COUNT(*) FROM users WHERE username LIKE '%" + delay_value + "%') > 0";
    }
    
    return query;
}

// VULNERABLE: Boolean-based blind injection
std::string VulnerableQueries::buildBooleanQuery(const std::string& base_condition, const std::string& injection_condition) {
    // Enables boolean-based blind injection attacks
    std::string query = "SELECT COUNT(*) FROM users WHERE " + base_condition;
    
    if (!injection_condition.empty()) {
        // This allows conditions like: 1=1 AND (SELECT SUBSTRING(password,1,1) FROM users WHERE id=1)='a'
        query += " AND " + injection_condition;
    }
    
    return query;
}

// VULNERABLE: Error-based injection pattern
std::string VulnerableQueries::buildErrorBasedQuery(const std::string& table, const std::string& malicious_input) {
    // Constructs queries that can trigger database errors to extract information
    std::string query = "SELECT * FROM " + table + " WHERE id = ";
    
    // This could contain payloads like: 1 AND (SELECT COUNT(*) FROM information_schema.tables)
    query += malicious_input;
    
    return query;
}

// VULNERABLE: Second-order injection
std::string VulnerableQueries::buildSecondOrderQuery(const std::string& stored_value, const std::string& context) {
    // Uses previously stored (potentially malicious) data in a new query
    std::string query = "SELECT * FROM audit_log WHERE action = 'login' AND details LIKE '%";
    
    // The stored_value might contain malicious SQL from a previous operation
    query += stored_value + "%'";
    
    if (!context.empty()) {
        // Additional context that could also be exploited
        query += " AND context = '" + context + "'";
    }
    
    return query;
}

// VULNERABLE: JSON-style injection in SQL context
std::string VulnerableQueries::buildJsonQuery(const std::string& json_input, const std::string& field) {
    // Simulates extracting values from JSON input and using them in SQL
    std::string query = "SELECT * FROM users WHERE ";
    
    // Simulate JSON parsing (vulnerable)
    size_t field_pos = json_input.find("\"" + field + "\":");
    if (field_pos != std::string::npos) {
        size_t value_start = json_input.find("\"", field_pos + field.length() + 3) + 1;
        size_t value_end = json_input.find("\"", value_start);
        
        if (value_start != std::string::npos && value_end != std::string::npos) {
            std::string value = json_input.substr(value_start, value_end - value_start);
            // Direct injection of extracted JSON value
            query += field + " = '" + value + "'";
        }
    } else {
        // Fallback that's also vulnerable
        query += "username = '" + json_input + "'";
    }
    
    return query;
}
