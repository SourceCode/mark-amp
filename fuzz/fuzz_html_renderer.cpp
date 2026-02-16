/// fuzz_html_renderer.cpp — Phase 36: Fuzz harness for HtmlRenderer

#include "core/HtmlRenderer.h"
#include "core/MarkdownParser.h"

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
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(std::string(input));

    markamp::core::HtmlRenderer renderer;
    auto html = renderer.render(doc);
    return 0;
}
