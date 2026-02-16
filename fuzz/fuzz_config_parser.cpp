/// fuzz_config_parser.cpp — Phase 36: Fuzz harness for Config parser

#include "core/Config.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0 || size > 256 * 1024)
    {
        return 0;
    }

    std::string_view input(reinterpret_cast<const char*>(data), size);
    try
    {
        markamp::core::Config config;
        config.loadFromString(std::string(input));
    }
    catch (...)
    {
        // Expected — configs can throw on invalid input
    }
    return 0;
}
