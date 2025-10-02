#!/bin/bash

# SonarQube analysis script for SQL Injection Demo

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SONAR_HOST_URL="${SONAR_HOST_URL:-http://localhost:9000}"
SONAR_TOKEN="${SONAR_TOKEN:-}"
BUILD_WRAPPER_OUT_DIR="build-wrapper-output"

echo -e "${BLUE}=== SonarQube Analysis for SQL Injection Demo ===${NC}"

# Check if SonarQube scanner is installed
check_sonar_scanner() {
    echo -e "${YELLOW}Checking SonarQube scanner...${NC}"
    
    if ! command -v sonar-scanner &> /dev/null; then
        echo -e "${RED}Error: sonar-scanner is not installed${NC}"
        echo "Please install SonarQube scanner:"
        echo "1. Download from: https://docs.sonarqube.org/latest/analysis/scan/sonarscanner/"
        echo "2. Or install via package manager:"
        echo "   - macOS: brew install sonar-scanner"
        echo "   - Ubuntu: sudo apt-get install sonar-scanner"
        exit 1
    fi
    
    echo -e "${GREEN}SonarQube scanner found${NC}"
}

# Check if build wrapper is available (for C++ analysis)
check_build_wrapper() {
    echo -e "${YELLOW}Checking build wrapper...${NC}"
    
    if ! command -v build-wrapper-linux-x86-64 &> /dev/null && ! command -v build-wrapper-macosx-x86 &> /dev/null; then
        echo -e "${YELLOW}Warning: build-wrapper not found${NC}"
        echo "For better C++ analysis, download build-wrapper from your SonarQube server:"
        echo "http://your-sonarqube-server/static/cpp/build-wrapper-linux-x86-64.zip"
        echo "http://your-sonarqube-server/static/cpp/build-wrapper-macosx-x86.zip"
        echo ""
        echo -e "${YELLOW}Continuing without build-wrapper (using compile_commands.json)${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Build wrapper found${NC}"
    return 0
}

# Build with wrapper if available
build_with_wrapper() {
    if check_build_wrapper; then
        echo -e "${YELLOW}Building with SonarQube build wrapper...${NC}"
        
        # Clean previous build
        rm -rf build $BUILD_WRAPPER_OUT_DIR
        mkdir -p build
        
        # Determine the correct build wrapper command
        if command -v build-wrapper-linux-x86-64 &> /dev/null; then
            BUILD_WRAPPER_CMD="build-wrapper-linux-x86-64"
        elif command -v build-wrapper-macosx-x86 &> /dev/null; then
            BUILD_WRAPPER_CMD="build-wrapper-macosx-x86"
        fi
        
        # Build with wrapper
        cd build
        cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
        cd ..
        
        $BUILD_WRAPPER_CMD --out-dir $BUILD_WRAPPER_OUT_DIR make -C build -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
        
        echo -e "${GREEN}Build with wrapper completed${NC}"
        return 0
    else
        echo -e "${YELLOW}Building without wrapper...${NC}"
        ./build.sh
        return 1
    fi
}

# Run SonarQube analysis
run_analysis() {
    echo -e "${YELLOW}Running SonarQube analysis...${NC}"
    
    # Prepare analysis parameters
    ANALYSIS_PARAMS=""
    
    if [ -n "$SONAR_TOKEN" ]; then
        ANALYSIS_PARAMS="$ANALYSIS_PARAMS -Dsonar.login=$SONAR_TOKEN"
    fi
    
    if [ "$SONAR_HOST_URL" != "http://localhost:9000" ]; then
        ANALYSIS_PARAMS="$ANALYSIS_PARAMS -Dsonar.host.url=$SONAR_HOST_URL"
    fi
    
    # Choose configuration file based on analysis type
    if [ "$SONAR_HOST_URL" = "https://sonarcloud.io" ]; then
        echo -e "${YELLOW}Using SonarCloud automatic analysis configuration${NC}"
        # SonarCloud will use automatic analysis - no build wrapper needed
        if [ -f ".sonarcloud.properties" ]; then
            ANALYSIS_PARAMS="$ANALYSIS_PARAMS -Dproject.settings=.sonarcloud.properties"
        fi
    else
        echo -e "${YELLOW}Using manual analysis configuration${NC}"
        # Use manual configuration for local SonarQube
        if [ -f "sonar-project-manual.properties" ]; then
            ANALYSIS_PARAMS="$ANALYSIS_PARAMS -Dproject.settings=sonar-project-manual.properties"
        fi
        
        # Add build wrapper output if available
        if [ -d "$BUILD_WRAPPER_OUT_DIR" ]; then
            ANALYSIS_PARAMS="$ANALYSIS_PARAMS -Dsonar.cfamily.build-wrapper-output=$BUILD_WRAPPER_OUT_DIR"
        fi
    fi
    
    # Run the analysis
    sonar-scanner $ANALYSIS_PARAMS
    
    echo -e "${GREEN}SonarQube analysis completed${NC}"
}

# Display results information
show_results() {
    echo -e "${BLUE}=== Analysis Complete ===${NC}"
    echo -e "${GREEN}Your project has been analyzed for SQL injection vulnerabilities!${NC}"
    echo ""
    echo "Expected findings:"
    echo "• SQL injection vulnerabilities in database_manager.cpp"
    echo "• Taint analysis issues in user_service.cpp"
    echo "• Security hotspots in vulnerable_queries.cpp"
    echo "• String concatenation security issues"
    echo ""
    echo "View results at: $SONAR_HOST_URL"
    echo "Project key: sql-injection-cpp-demo"
}

# Main execution
main() {
    check_sonar_scanner
    
    # Build the project (with or without wrapper)
    if ! build_with_wrapper; then
        echo -e "${YELLOW}Proceeding with standard build...${NC}"
    fi
    
    # Run analysis
    run_analysis
    
    # Show results
    show_results
}

# Help function
show_help() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Environment variables:"
    echo "  SONAR_HOST_URL    SonarQube server URL (default: http://localhost:9000)"
    echo "  SONAR_TOKEN       Authentication token for SonarQube"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Analyze with local SonarQube"
    echo "  SONAR_TOKEN=xxx $0                   # Analyze with authentication"
    echo "  SONAR_HOST_URL=https://sonarcloud.io SONAR_TOKEN=xxx $0  # Analyze with SonarCloud"
}

# Check for help flag
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    show_help
    exit 0
fi

# Execute main function
main "$@"
