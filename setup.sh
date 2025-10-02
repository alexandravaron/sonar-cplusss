#!/bin/bash

# Setup script for SQL Injection Demo project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== SQL Injection C++ Demo Setup ===${NC}"

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# Install dependencies based on OS
install_dependencies() {
    local os=$(detect_os)
    
    echo -e "${YELLOW}Installing dependencies for $os...${NC}"
    
    case $os in
        "linux")
            if command -v apt-get &> /dev/null; then
                sudo apt-get update
                sudo apt-get install -y build-essential cmake libsqlite3-dev pkg-config
            elif command -v yum &> /dev/null; then
                sudo yum groupinstall -y "Development Tools"
                sudo yum install -y cmake sqlite-devel pkgconfig
            elif command -v pacman &> /dev/null; then
                sudo pacman -S --noconfirm base-devel cmake sqlite pkg-config
            else
                echo -e "${RED}Unsupported Linux distribution${NC}"
                exit 1
            fi
            ;;
        "macos")
            if ! command -v brew &> /dev/null; then
                echo -e "${YELLOW}Installing Homebrew...${NC}"
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi
            brew install cmake sqlite3 pkg-config
            ;;
        "windows")
            echo -e "${YELLOW}For Windows, please install:${NC}"
            echo "1. Visual Studio with C++ support"
            echo "2. CMake from https://cmake.org/"
            echo "3. SQLite3 development libraries"
            echo "4. vcpkg for package management"
            ;;
        *)
            echo -e "${RED}Unsupported operating system${NC}"
            exit 1
            ;;
    esac
}

# Install SonarQube Scanner
install_sonar_scanner() {
    local os=$(detect_os)
    
    echo -e "${YELLOW}Installing SonarQube Scanner...${NC}"
    
    case $os in
        "linux")
            if command -v apt-get &> /dev/null; then
                # Try to install from package manager first
                if apt-cache search sonar-scanner | grep -q sonar-scanner; then
                    sudo apt-get install -y sonar-scanner
                else
                    install_sonar_manual
                fi
            else
                install_sonar_manual
            fi
            ;;
        "macos")
            brew install sonar-scanner
            ;;
        *)
            install_sonar_manual
            ;;
    esac
}

# Manual SonarQube Scanner installation
install_sonar_manual() {
    echo -e "${YELLOW}Installing SonarQube Scanner manually...${NC}"
    
    local version="4.8.0.2856"
    local url="https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-${version}.zip"
    local install_dir="/opt/sonar-scanner"
    
    # Download and install
    wget -O sonar-scanner.zip "$url"
    sudo unzip sonar-scanner.zip -d /opt/
    sudo mv "/opt/sonar-scanner-${version}" "$install_dir"
    sudo chmod +x "$install_dir/bin/sonar-scanner"
    
    # Add to PATH
    echo "export PATH=\"$install_dir/bin:\$PATH\"" >> ~/.bashrc
    echo -e "${GREEN}SonarQube Scanner installed to $install_dir${NC}"
    echo -e "${YELLOW}Please restart your shell or run: source ~/.bashrc${NC}"
    
    rm sonar-scanner.zip
}

# Verify installation
verify_installation() {
    echo -e "${YELLOW}Verifying installation...${NC}"
    
    local errors=0
    
    # Check C++ compiler
    if command -v g++ &> /dev/null || command -v clang++ &> /dev/null; then
        echo -e "${GREEN}✓ C++ compiler found${NC}"
    else
        echo -e "${RED}✗ C++ compiler not found${NC}"
        errors=$((errors + 1))
    fi
    
    # Check CMake
    if command -v cmake &> /dev/null; then
        echo -e "${GREEN}✓ CMake found${NC}"
    else
        echo -e "${YELLOW}⚠ CMake not found (Makefile will be used instead)${NC}"
    fi
    
    # Check SQLite3
    if pkg-config --exists sqlite3 2>/dev/null; then
        echo -e "${GREEN}✓ SQLite3 development libraries found${NC}"
    else
        echo -e "${RED}✗ SQLite3 development libraries not found${NC}"
        errors=$((errors + 1))
    fi
    
    # Check SonarQube Scanner
    if command -v sonar-scanner &> /dev/null; then
        echo -e "${GREEN}✓ SonarQube Scanner found${NC}"
    else
        echo -e "${YELLOW}⚠ SonarQube Scanner not found (optional for analysis)${NC}"
    fi
    
    if [ $errors -eq 0 ]; then
        echo -e "${GREEN}All required dependencies are installed!${NC}"
        return 0
    else
        echo -e "${RED}Some dependencies are missing. Please install them manually.${NC}"
        return 1
    fi
}

# Build the project
build_project() {
    echo -e "${YELLOW}Building the project...${NC}"
    
    if command -v cmake &> /dev/null; then
        ./build.sh
    else
        make clean
        make
    fi
    
    echo -e "${GREEN}Build completed!${NC}"
}

# Show next steps
show_next_steps() {
    echo -e "${BLUE}=== Setup Complete ===${NC}"
    echo -e "${GREEN}Your SQL Injection Demo project is ready!${NC}"
    echo ""
    echo "Next steps:"
    echo "1. Run the demo: ./sql_injection_demo"
    echo "2. Analyze with SonarQube: ./analyze.sh"
    echo "3. Read the README.md for detailed instructions"
    echo ""
    echo "Available commands:"
    echo "• make help          - Show Makefile targets"
    echo "• ./build.sh --help  - Show build script options"
    echo "• ./analyze.sh --help - Show analysis script options"
}

# Main execution
main() {
    local install_deps=false
    local install_sonar=false
    local build=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --deps)
                install_deps=true
                shift
                ;;
            --sonar)
                install_sonar=true
                shift
                ;;
            --build)
                build=true
                shift
                ;;
            --all)
                install_deps=true
                install_sonar=true
                build=true
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [options]"
                echo ""
                echo "Options:"
                echo "  --deps    Install system dependencies"
                echo "  --sonar   Install SonarQube Scanner"
                echo "  --build   Build the project"
                echo "  --all     Do everything above"
                echo "  --help    Show this help"
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # If no options provided, ask user
    if [ "$install_deps" = false ] && [ "$install_sonar" = false ] && [ "$build" = false ]; then
        echo "What would you like to do?"
        echo "1) Install dependencies only"
        echo "2) Install SonarQube Scanner only"
        echo "3) Build project only"
        echo "4) Install everything and build"
        echo "5) Just verify current setup"
        read -p "Choose an option (1-5): " choice
        
        case $choice in
            1) install_deps=true ;;
            2) install_sonar=true ;;
            3) build=true ;;
            4) install_deps=true; install_sonar=true; build=true ;;
            5) ;;
            *) echo "Invalid choice"; exit 1 ;;
        esac
    fi
    
    # Execute selected actions
    if [ "$install_deps" = true ]; then
        install_dependencies
    fi
    
    if [ "$install_sonar" = true ]; then
        install_sonar_scanner
    fi
    
    verify_installation
    
    if [ "$build" = true ]; then
        build_project
    fi
    
    show_next_steps
}

# Execute main function with all arguments
main "$@"
