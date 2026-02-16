/// fuzz_constexpr_map.cpp — Phase 36: Fuzz harness for ConstexprMap lookup

#include "core/ConstexprMap.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0 || size > 1024)
    {
        return 0;
    }

    std::string_view key(reinterpret_cast<const char*>(data), size);

    // Exercise MIME type lookup
    auto mime = markamp::core::kMimeTypes.find(key);
    static_cast<void>(mime);

    // Exercise keyword lookup
    auto kw = markamp::core::kCppKeywords.find(key);
    static_cast<void>(kw);

    return 0;
}
