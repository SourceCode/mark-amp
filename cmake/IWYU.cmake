# IWYU.cmake -- Integrate include-what-you-use into the CMake build

option(MARKAMP_ENABLE_IWYU "Enable include-what-you-use analysis" OFF)

if(MARKAMP_ENABLE_IWYU)
    find_program(IWYU_EXECUTABLE NAMES include-what-you-use iwyu)

    if(IWYU_EXECUTABLE)
        message(STATUS "include-what-you-use found: ${IWYU_EXECUTABLE}")
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXECUTABLE})
    else()
        message(STATUS "include-what-you-use not found -- skipping IWYU analysis")
    endif()
endif()
