/// fuzz_incremental_tokenizer.cpp — Phase 36: Fuzz harness for IncrementalTokenizer

#include "core/IncrementalTokenizer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < 2 || size > 256 * 1024)
    {
        return 0;
    }

    // Split input into lines at '\n'
    std::string_view input(reinterpret_cast<const char*>(data), size);
    std::vector<std::string> lines;
    std::size_t pos = 0;
    while (pos < input.size())
    {
        auto next = input.find('\n', pos);
        if (next == std::string_view::npos)
        {
            lines.emplace_back(input.substr(pos));
            break;
        }
        lines.emplace_back(input.substr(pos, next - pos));
        pos = next + 1;
    }

    markamp::core::SyntaxHighlighter highlighter;
    markamp::core::IncrementalTokenizer tokenizer(highlighter, "cpp");
    tokenizer.set_content(lines);
    tokenizer.retokenize();

    // Invalidate a line and retokenize
    if (!lines.empty())
    {
        tokenizer.invalidate_line(0);
        tokenizer.retokenize();
    }

    return 0;
}
