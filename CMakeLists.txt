cmake_minimum_required(VERSION 3.30)

# vcpkg toolchain detection (must be before project())
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "vcpkg toolchain file")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "vcpkg toolchain file")
endif()

project(MarkAmp VERSION 1.4.7 LANGUAGES CXX)

# C++23 with C++26-forward patterns
# Note: C++26 mode breaks spdlog/fmt's FMT_STRING consteval macro on Clang 21 and GCC 14.
# All code uses C++23 features (std::expected, concepts, constexpr) ready for C++26 upgrade
# when spdlog/fmt are updated for full C++26 compliance.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling (clang-tidy, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug if build type is not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(CppCheck)
include(IWYU)
include(Dependencies)

# Platform detection
if(APPLE)
    set(MARKAMP_PLATFORM_MACOS ON)
elseif(WIN32)
    set(MARKAMP_PLATFORM_WINDOWS ON)
elseif(UNIX AND NOT APPLE)
    set(MARKAMP_PLATFORM_LINUX ON)
endif()

# Version definitions
add_compile_definitions(
    MARKAMP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    MARKAMP_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    MARKAMP_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Sanitizer options (OFF by default)
option(MARKAMP_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(MARKAMP_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(MARKAMP_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# Testing option
option(MARKAMP_BUILD_TESTS "Build tests" ON)

# Subdirectories
add_subdirectory(src)

if(MARKAMP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
