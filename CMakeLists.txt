cmake_minimum_required(VERSION 3.10)
project(cpp-rest-api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(src/config)
include_directories(src/third_party)
include_directories(src/third_party/jwt-cpp)

# Source files
set(SOURCES
    src/main.cpp
    src/db/connection.cpp
    src/repositories/user_repository.cpp
    src/repositories/refresh_token_repository.cpp
    src/routes/user_routes.cpp
    src/routes/auth_routes.cpp
    src/utils/hash.cpp
    src/utils/session_manager.cpp
)

# Executable
add_executable(api ${SOURCES})

# Link libraries
target_link_libraries(api PostgreSQL::PostgreSQL OpenSSL::SSL OpenSSL::Crypto ws2_32)