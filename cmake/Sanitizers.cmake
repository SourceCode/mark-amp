# Sanitizers.cmake -- Configure AddressSanitizer, UBSan, ThreadSanitizer, and LeakSanitizer

function(markamp_configure_sanitizers target_name)
    # Validate mutual exclusivity of ASAN/UBSAN and TSAN
    if(MARKAMP_ENABLE_TSAN AND (MARKAMP_ENABLE_ASAN OR MARKAMP_ENABLE_UBSAN))
        message(FATAL_ERROR
            "ThreadSanitizer (TSAN) cannot be used with AddressSanitizer (ASAN) or "
            "UndefinedBehaviorSanitizer (UBSAN). Disable one group to proceed."
        )
    endif()

    # LSan standalone is incompatible with TSan
    if(MARKAMP_ENABLE_LSAN AND MARKAMP_ENABLE_TSAN)
        message(FATAL_ERROR
            "LeakSanitizer (LSAN) standalone cannot be used with ThreadSanitizer (TSAN). "
            "Disable one to proceed."
        )
    endif()

    # LSan is already included in ASan -- warn if both are explicitly enabled
    if(MARKAMP_ENABLE_LSAN AND MARKAMP_ENABLE_ASAN)
        message(WARNING
            "LeakSanitizer is already included in AddressSanitizer. "
            "The standalone LSAN flag is redundant when ASAN is enabled."
        )
    endif()

    set(SANITIZER_FLAGS "")

    if(MSVC)
        if(MARKAMP_ENABLE_ASAN)
            list(APPEND SANITIZER_FLAGS /fsanitize=address)
        endif()
        if(MARKAMP_ENABLE_UBSAN)
            message(WARNING "UBSan is not natively supported on MSVC. Skipping.")
        endif()
        if(MARKAMP_ENABLE_TSAN)
            message(WARNING "TSan is not natively supported on MSVC. Skipping.")
        endif()
        if(MARKAMP_ENABLE_LSAN)
            message(WARNING "LSan is not natively supported on MSVC. Skipping.")
        endif()
    else()
        if(MARKAMP_ENABLE_ASAN)
            list(APPEND SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
        endif()
        if(MARKAMP_ENABLE_UBSAN)
            list(APPEND SANITIZER_FLAGS -fsanitize=undefined)
        endif()
        if(MARKAMP_ENABLE_TSAN)
            list(APPEND SANITIZER_FLAGS -fsanitize=thread)
        endif()
        if(MARKAMP_ENABLE_LSAN)
            list(APPEND SANITIZER_FLAGS -fsanitize=leak)
        endif()
    endif()

    if(SANITIZER_FLAGS)
        target_compile_options(${target_name} PRIVATE ${SANITIZER_FLAGS})
        target_link_options(${target_name} PRIVATE ${SANITIZER_FLAGS})
        message(STATUS "Sanitizers enabled for ${target_name}: ${SANITIZER_FLAGS}")
    endif()
endfunction()
