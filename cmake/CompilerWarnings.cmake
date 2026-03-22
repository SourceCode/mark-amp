# CompilerWarnings.cmake -- Set compiler warning flags per target

function(markamp_set_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wconversion
            -Wsign-conversion
            -Wcast-align
            -Wunused
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Wshadow
            -Wold-style-cast
            -Woverloaded-virtual
            -Wmisleading-indentation
        )
    endif()
endfunction()
