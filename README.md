# SQL Injection Vulnerability Demo for SonarQube C++ Analysis

This project demonstrates various SQL injection vulnerabilities in C++ code that can be detected by SonarQube's taint analysis engine. It serves as a test case for security analysis tools and educational purposes.

## ⚠️ Security Warning

**This code contains intentional security vulnerabilities and should NEVER be used in production environments.** It is designed solely for testing static analysis tools and educational purposes.

## Project Structure

```
sql_injection_cpp_project/
├── src/
│   ├── main.cpp                 # Main demo application
│   ├── database_manager.cpp     # Database operations with SQL injection vulnerabilities
│   ├── user_service.cpp         # User service with various injection patterns
│   └── vulnerable_queries.cpp   # Query building functions with vulnerabilities
├── include/
│   ├── database_manager.h       # Database manager header
│   ├── user_service.h          # User service header
│   └── vulnerable_queries.h    # Vulnerable queries header
├── CMakeLists.txt              # CMake build configuration
├── sonar-project.properties    # SonarQube configuration
├── .sonarcloud.properties      # SonarCloud configuration
├── build.sh                    # Build script
├── analyze.sh                  # SonarQube analysis script
└── README.md                   # This file
```

## Vulnerability Types Demonstrated

### 1. **Basic SQL Injection**
- Direct string concatenation in SQL queries
- User input directly embedded in WHERE clauses
- Vulnerable login bypass scenarios

### 2. **Advanced Injection Patterns**
- UNION-based injection
- Boolean-based blind injection
- Time-based blind injection
- Error-based injection
- Second-order injection

### 3. **Context-Specific Vulnerabilities**
- ORDER BY clause injection
- LIKE operator injection
- JSON parameter injection
- Web request parameter injection
- API parameter injection

### 4. **Taint Analysis Scenarios**
- User input flowing through multiple functions
- Data transformation that preserves taint
- Cross-function vulnerability propagation

## Prerequisites

### System Requirements
- **C++ Compiler**: GCC 7+ or Clang 6+
- **CMake**: Version 3.10 or higher
- **SQLite3**: Development libraries
- **pkg-config**: For dependency management

### SonarQube Requirements
- **SonarQube Server**: 8.9+ (Community Edition or higher)
- **SonarQube Scanner**: Latest version
- **Build Wrapper**: For C++ analysis (optional but recommended)

### Installation Commands

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libsqlite3-dev pkg-config
```

#### macOS
```bash
brew install cmake sqlite3 pkg-config
```

#### SonarQube Scanner
```bash
# macOS
brew install sonar-scanner

# Ubuntu/Debian
sudo apt-get install sonar-scanner

# Or download from: https://docs.sonarqube.org/latest/analysis/scan/sonarscanner/
```

## Building the Project

### Quick Build
```bash
./build.sh
```

### Manual Build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j$(nproc)
```

### Running the Demo
```bash
# Build and run
./build.sh --run

# Or run manually
./build/sql_injection_demo
```

## SonarQube Analysis

### Local SonarQube Server
```bash
# Start SonarQube server (if running locally)
# Then run analysis
./analyze.sh
```

### SonarCloud Analysis
```bash
# Set environment variables
export SONAR_HOST_URL=https://sonarcloud.io
export SONAR_TOKEN=your_sonarcloud_token

# Update .sonarcloud.properties with your organization
# Then run analysis
./analyze.sh
```

### Manual Analysis
```bash
# With build wrapper (recommended)
build-wrapper-linux-x86-64 --out-dir build-wrapper-output make -C build clean all
sonar-scanner

# Without build wrapper (using compile_commands.json)
sonar-scanner -Dsonar.cfamily.compile-commands=build/compile_commands.json
```

## Expected SonarQube Findings

When analyzed with SonarQube, this project should generate the following types of security findings:

### Critical Security Issues
- **SQL Injection vulnerabilities** in `database_manager.cpp`
- **Taint analysis violations** in `user_service.cpp`
- **Security hotspots** for string concatenation in SQL queries

### Specific Vulnerable Functions
1. `getUserByIdVulnerable()` - Direct ID injection
2. `searchUsersVulnerable()` - LIKE clause injection
3. `loginUser()` - Authentication bypass
4. `executeRawQueryVulnerable()` - Direct query execution
5. `buildLoginQuery()` - Query construction vulnerabilities
6. `processWebRequest()` - Web parameter injection
7. `handleApiCall()` - API parameter injection

### Security Rules Triggered
- **cpp:S2077**: SQL queries should not be vulnerable to injection attacks
- **cpp:S3649**: Database queries should not be vulnerable to injection attacks
- **cpp:S5131**: Endpoints should not be vulnerable to injection attacks

## Configuration Files

### sonar-project.properties
Key configurations for C++ analysis:
- Enables taint analysis: `sonar.cfamily.taint.enable=true`
- Security hotspot detection: `sonar.security.hotspots.enable=true`
- Build wrapper integration: `sonar.cfamily.build-wrapper-output`

### CMakeLists.txt
- Generates `compile_commands.json` for SonarQube
- Links SQLite3 libraries
- Sets debug flags for better analysis

## Comparing Safe vs Vulnerable Code

The project includes both vulnerable and safe implementations:

### Vulnerable Pattern
```cpp
// VULNERABLE: Direct string concatenation
std::string query = "SELECT * FROM users WHERE id = " + user_id;
return executeQuery(query);
```

### Safe Pattern
```cpp
// SAFE: Parameterized query
sqlite3_stmt* stmt;
const char* query = "SELECT * FROM users WHERE id = ?";
sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
sqlite3_bind_int(stmt, 1, user_id);
```

## Educational Use Cases

This project is useful for:

1. **Security Training**: Understanding SQL injection patterns
2. **Tool Testing**: Validating static analysis tools
3. **CI/CD Integration**: Testing security gates in pipelines
4. **Compliance**: Demonstrating security scanning capabilities

## Troubleshooting

### Build Issues
```bash
# Check dependencies
pkg-config --exists sqlite3 && echo "SQLite3 found" || echo "SQLite3 missing"

# Clean build
rm -rf build build-wrapper-output
./build.sh
```

### SonarQube Issues
```bash
# Check scanner installation
sonar-scanner --version

# Verify project configuration
cat sonar-project.properties

# Check build wrapper output
ls -la build-wrapper-output/
```

### Common Problems

1. **Missing SQLite3**: Install development libraries
2. **Build wrapper not found**: Download from SonarQube server
3. **No vulnerabilities detected**: Check taint analysis is enabled
4. **Authentication errors**: Verify SONAR_TOKEN

## Contributing

This is a demonstration project. If you find additional vulnerability patterns that should be included:

1. Add the vulnerable code following existing patterns
2. Document the vulnerability type
3. Ensure SonarQube can detect it
4. Update this README

## License

This project is provided for educational and testing purposes only. Use at your own risk and never in production environments.

## Disclaimer

The vulnerabilities in this code are intentional and designed for testing purposes. The authors are not responsible for any misuse of this code or any security incidents that may result from its use.

---

**Remember**: This code demonstrates security vulnerabilities. Always use parameterized queries and proper input validation in real applications!
