# Allocator.cmake — Custom allocator integration
#
# Finds and links mimalloc when MARKAMP_ENABLE_MIMALLOC is ON.
# Automatically disables mimalloc when sanitizers are active (known conflict).
#
# Usage in target CMakeLists:
#   markamp_configure_allocator(target_name)

include_guard(GLOBAL)

option(MARKAMP_ENABLE_MIMALLOC "Use mimalloc as the global allocator" OFF)

function(markamp_configure_allocator target_name)
    if(NOT MARKAMP_ENABLE_MIMALLOC)
        message(STATUS "mimalloc: disabled (MARKAMP_ENABLE_MIMALLOC=OFF)")
        return()
    endif()

    # Sanitizers and mimalloc are incompatible — auto-disable
    if(MARKAMP_ENABLE_ASAN OR MARKAMP_ENABLE_TSAN OR MARKAMP_ENABLE_LSAN OR MARKAMP_ENABLE_UBSAN)
        message(STATUS "mimalloc: disabled (sanitizer active — known conflict)")
        return()
    endif()

    find_package(mimalloc CONFIG REQUIRED)

    target_link_libraries(${target_name} PRIVATE mimalloc)
    target_compile_definitions(${target_name} PRIVATE MARKAMP_MIMALLOC_ENABLED=1)

    message(STATUS "mimalloc: enabled for target '${target_name}'")

    # On macOS, mimalloc auto-overrides malloc via interposition
    # On Linux, we need LD_PRELOAD or static linking
    if(UNIX AND NOT APPLE)
        message(STATUS "mimalloc: on Linux, use LD_PRELOAD=libmimalloc.so or link statically")
    endif()
endfunction()
