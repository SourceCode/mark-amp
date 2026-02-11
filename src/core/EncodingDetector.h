#pragma once

#include <string>
#include <string_view>

namespace markamp::core
{

enum class Encoding
{
    Utf8,
    Utf8Bom,
    Utf16LE,
    Utf16BE,
    Ascii,
    Unknown
};

struct DetectedEncoding
{
    Encoding encoding{Encoding::Unknown};
    std::string display_name;
};

/// Detect the encoding of raw file bytes.
[[nodiscard]] auto detect_encoding(std::string_view raw_bytes) -> DetectedEncoding;

/// Strip BOM prefix from content if present.
[[nodiscard]] auto strip_bom(const std::string& content, Encoding enc) -> std::string;

/// Get display name for an encoding.
[[nodiscard]] auto encoding_display_name(Encoding enc) -> std::string;

} // namespace markamp::core
