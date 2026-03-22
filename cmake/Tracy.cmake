# Tracy.cmake -- Tracy profiler integration
#
# When MARKAMP_ENABLE_TRACY is ON, finds and links the Tracy client library
# and defines MARKAMP_TRACY_ENABLED. When OFF, TracyIntegration.h macros no-op.

option(MARKAMP_ENABLE_TRACY "Enable Tracy profiler integration" OFF)

function(markamp_configure_tracy target_name)
    if(NOT MARKAMP_ENABLE_TRACY)
        return()
    endif()

    find_package(Tracy CONFIG QUIET)
    if(NOT Tracy_FOUND)
        message(WARNING "Tracy enabled but not found via vcpkg. Tracy instrumentation will be disabled.")
        return()
    endif()

    target_link_libraries(${target_name} PRIVATE Tracy::TracyClient)
    target_compile_definitions(${target_name} PRIVATE MARKAMP_TRACY_ENABLED TRACY_ENABLE)
    message(STATUS "Tracy profiler enabled for ${target_name}")
endfunction()
