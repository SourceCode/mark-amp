/// fuzz_syntax_highlighter.cpp — Phase 36: Fuzz harness for SyntaxHighlighter

#include "core/SyntaxHighlighter.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0 || size > 512 * 1024)
    {
        return 0;
    }

    std::string_view input(reinterpret_cast<const char*>(data), size);
    markamp::core::SyntaxHighlighter highlighter;
    auto tokens = highlighter.tokenize(std::string(input), "cpp");
    return 0;
}
