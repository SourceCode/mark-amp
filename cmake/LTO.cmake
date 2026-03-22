# LTO.cmake -- Link-Time Optimization support
#
# Provides the markamp_configure_lto() function to enable LTO per-target.
# Auto-detects ThinLTO (Clang), regular LTO (GCC), or WPO (MSVC).

option(MARKAMP_ENABLE_LTO "Enable Link-Time Optimization" OFF)

function(markamp_configure_lto target_name)
    if(NOT MARKAMP_ENABLE_LTO)
        return()
    endif()

    # Use CMake's built-in IPO support check
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(NOT ipo_supported)
        message(WARNING "LTO is not supported on this platform/compiler: ${ipo_error}")
        return()
    endif()

    # Enable IPO for the target
    set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

    if(NOT MSVC)
        # Prefer ThinLTO for Clang (faster link, nearly identical optimization)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target_name} PRIVATE -flto=thin)
            target_link_options(${target_name} PRIVATE -flto=thin)
            message(STATUS "LTO enabled for ${target_name}: ThinLTO (Clang)")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target_name} PRIVATE -flto)
            target_link_options(${target_name} PRIVATE -flto)
            message(STATUS "LTO enabled for ${target_name}: Full LTO (GCC)")
        endif()

        # Dead-code stripping (GCC/Clang)
        target_compile_options(${target_name} PRIVATE -fdata-sections -ffunction-sections)
        if(APPLE)
            target_link_options(${target_name} PRIVATE -Wl,-dead_strip)
        else()
            target_link_options(${target_name} PRIVATE -Wl,--gc-sections)
        endif()

        # Prefer lld linker for faster ThinLTO linking on non-Apple platforms
        if(NOT APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            find_program(LLD_LINKER "ld.lld")
            if(LLD_LINKER)
                target_link_options(${target_name} PRIVATE -fuse-ld=lld)
                message(STATUS "  Using lld linker for faster ThinLTO")
            endif()
        endif()
    else()
        # MSVC: /GL for compile, /LTCG for link
        target_compile_options(${target_name} PRIVATE /GL)
        target_link_options(${target_name} PRIVATE /LTCG)
        message(STATUS "LTO enabled for ${target_name}: WPO (MSVC /GL + /LTCG)")
    endif()
endfunction()
