# Dependencies.cmake -- Find and aggregate all third-party dependencies

# --- wxWidgets ---
find_package(wxWidgets REQUIRED COMPONENTS core base html xml aui stc)
include(${wxWidgets_USE_FILE})

# --- nlohmann/json ---
find_package(nlohmann_json CONFIG REQUIRED)

# --- md4c ---
find_package(md4c CONFIG REQUIRED)

# --- fmt ---
find_package(fmt CONFIG REQUIRED)

# --- spdlog ---
find_package(spdlog CONFIG REQUIRED)

# --- yaml-cpp ---
find_package(yaml-cpp CONFIG REQUIRED)

# --- libzip ---
find_package(libzip CONFIG REQUIRED)

# --- cpp-httplib (requires OpenSSL) ---
find_package(OpenSSL REQUIRED)
find_package(httplib CONFIG REQUIRED)

# --- Catch2 (test-only) ---
if(MARKAMP_BUILD_TESTS)
    find_package(Catch2 3 CONFIG REQUIRED)
endif()

# --- SQLite3 ---
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Aggregate interface library for convenience linking
add_library(markamp_dependencies INTERFACE)
target_link_libraries(markamp_dependencies INTERFACE
    ${wxWidgets_LIBRARIES}
    nlohmann_json::nlohmann_json
    md4c::md4c
    fmt::fmt
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    libzip::zip
    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    unofficial::sqlite3::sqlite3
)

message(STATUS "All dependencies found and aggregated into markamp_dependencies")
