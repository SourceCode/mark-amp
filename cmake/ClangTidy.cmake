# ClangTidy.cmake -- Integrate clang-tidy into the CMake build

option(MARKAMP_ENABLE_CLANG_TIDY "Enable clang-tidy static analysis during compilation" OFF)

if(MARKAMP_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy clang-tidy-18 clang-tidy-17 clang-tidy-16)

    if(CLANG_TIDY_EXECUTABLE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXECUTABLE}")

        # Use project .clang-tidy config file and optionally treat warnings as errors
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY_EXECUTABLE}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            --header-filter=src/.*
        )

        # Optionally make clang-tidy warnings fail the build
        option(MARKAMP_CLANG_TIDY_ERRORS "Treat clang-tidy warnings as errors" OFF)
        if(MARKAMP_CLANG_TIDY_ERRORS)
            list(APPEND CMAKE_CXX_CLANG_TIDY --warnings-as-errors=*)
        endif()
    else()
        message(WARNING
            "MARKAMP_ENABLE_CLANG_TIDY is ON but clang-tidy was not found. "
            "Install clang-tidy or disable this option."
        )
    endif()
endif()
