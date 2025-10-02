#!/bin/bash

# Build script for SQL Injection Demo with SonarQube analysis

set -e

echo "=== Building SQL Injection C++ Demo ==="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if required tools are installed
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: cmake is not installed${NC}"
        exit 1
    fi
    
    if ! command -v pkg-config &> /dev/null; then
        echo -e "${RED}Error: pkg-config is not installed${NC}"
        exit 1
    fi
    
    if ! pkg-config --exists sqlite3; then
        echo -e "${RED}Error: sqlite3 development libraries are not installed${NC}"
        echo "On Ubuntu/Debian: sudo apt-get install libsqlite3-dev"
        echo "On macOS: brew install sqlite3"
        exit 1
    fi
    
    echo -e "${GREEN}All dependencies found${NC}"
}

# Clean previous build
clean_build() {
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf build
    rm -rf build-wrapper-output
    rm -f test.db
}

# Build the project
build_project() {
    echo -e "${YELLOW}Building project...${NC}"
    
    mkdir -p build
    cd build
    
    # Generate compile commands for SonarQube
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    
    # Build the project
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    cd ..
    echo -e "${GREEN}Build completed successfully${NC}"
}

# Run the demo (optional)
run_demo() {
    if [ "$1" = "--run" ]; then
        echo -e "${YELLOW}Running SQL injection demo...${NC}"
        ./build/sql_injection_demo
    fi
}

# Main execution
main() {
    check_dependencies
    clean_build
    build_project
    run_demo "$1"
    
    echo -e "${GREEN}Build process completed!${NC}"
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. Run SonarQube analysis with: ./analyze.sh"
    echo "2. Or run the demo with: ./build/sql_injection_demo"
}

# Execute main function with all arguments
main "$@"
