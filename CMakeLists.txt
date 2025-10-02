cmake_minimum_required(VERSION 3.10)
project(SQLInjectionDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find SQLite3
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Include directories
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(include)

# Add executable
add_executable(sql_injection_demo
    src/main.cpp
    src/database_manager.cpp
    src/user_service.cpp
    src/vulnerable_queries.cpp
)

# Link libraries
target_link_libraries(sql_injection_demo ${SQLITE3_LIBRARIES})
target_compile_options(sql_injection_demo PRIVATE ${SQLITE3_CFLAGS_OTHER})

# Set compiler flags for better analysis
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Create directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/reports)
