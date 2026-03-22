# CppCheck.cmake -- Integrate cppcheck into the CMake build

find_program(CPPCHECK_EXECUTABLE cppcheck)

if(CPPCHECK_EXECUTABLE)
    message(STATUS "cppcheck found: ${CPPCHECK_EXECUTABLE}")

    set(CPPCHECK_SUPPRESSIONS_FILE
        "${CMAKE_SOURCE_DIR}/cppcheck-suppressions.txt"
        CACHE FILEPATH "Path to cppcheck suppressions file"
    )

    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK_EXECUTABLE}
        --enable=all
        --std=c++23  # Updated to match project standard
        --suppressions-list=${CPPCHECK_SUPPRESSIONS_FILE}
        --inline-suppr
        --quiet
    )
else()
    message(STATUS "cppcheck not found -- skipping static analysis via cppcheck")
endif()
