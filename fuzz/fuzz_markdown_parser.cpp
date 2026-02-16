/// fuzz_markdown_parser.cpp — Phase 36: Fuzz harness for MarkdownParser
///
/// libFuzzer harness that fuzzes the markdown parser with arbitrary input.

#include "core/MarkdownParser.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0 || size > 1024 * 1024)
    {
        return 0;
    }

    std::string_view input(reinterpret_cast<const char*>(data), size);
    markamp::core::MarkdownParser parser;
    auto result = parser.parse(std::string(input));
    return 0;
}
