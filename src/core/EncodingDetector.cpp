#include "EncodingDetector.h"

namespace markamp::core
{

namespace
{

auto is_valid_utf8(std::string_view data) -> bool
{
    std::size_t i = 0;
    while (i < data.size())
    {
        auto byte = static_cast<unsigned char>(data[i]);

        int continuation_bytes = 0;
        if (byte <= 0x7F)
        {
            // ASCII byte
            ++i;
            continue;
        }
        else if ((byte & 0xE0) == 0xC0)
        {
            continuation_bytes = 1;
        }
        else if ((byte & 0xF0) == 0xE0)
        {
            continuation_bytes = 2;
        }
        else if ((byte & 0xF8) == 0xF0)
        {
            continuation_bytes = 3;
        }
        else
        {
            return false; // Invalid lead byte
        }

        if (i + static_cast<std::size_t>(continuation_bytes) >= data.size())
        {
            return false; // Truncated sequence
        }

        for (int j = 1; j <= continuation_bytes; ++j)
        {
            auto cont = static_cast<unsigned char>(data[i + static_cast<std::size_t>(j)]);
            if ((cont & 0xC0) != 0x80)
            {
                return false; // Invalid continuation byte
            }
        }

        i += 1 + static_cast<std::size_t>(continuation_bytes);
    }
    return true;
}

auto is_ascii_only(std::string_view data) -> bool
{
    for (char ch : data)
    {
        auto byte = static_cast<unsigned char>(ch);
        if (byte > 0x7F)
        {
            return false;
        }
    }
    return true;
}

} // anonymous namespace

auto detect_encoding(std::string_view raw_bytes) -> DetectedEncoding
{
    if (raw_bytes.empty())
    {
        return {Encoding::Utf8, "UTF-8"};
    }

    // Check for BOMs
    if (raw_bytes.size() >= 3 && static_cast<unsigned char>(raw_bytes[0]) == 0xEF &&
        static_cast<unsigned char>(raw_bytes[1]) == 0xBB &&
        static_cast<unsigned char>(raw_bytes[2]) == 0xBF)
    {
        return {Encoding::Utf8Bom, "UTF-8 BOM"};
    }

    if (raw_bytes.size() >= 2)
    {
        auto b0 = static_cast<unsigned char>(raw_bytes[0]);
        auto b1 = static_cast<unsigned char>(raw_bytes[1]);

        if (b0 == 0xFF && b1 == 0xFE)
        {
            return {Encoding::Utf16LE, "UTF-16 LE"};
        }
        if (b0 == 0xFE && b1 == 0xFF)
        {
            return {Encoding::Utf16BE, "UTF-16 BE"};
        }
    }

    // No BOM — check content
    if (is_ascii_only(raw_bytes))
    {
        return {Encoding::Ascii, "ASCII"};
    }

    if (is_valid_utf8(raw_bytes))
    {
        return {Encoding::Utf8, "UTF-8"};
    }

    return {Encoding::Unknown, "Unknown"};
}

auto strip_bom(const std::string& content, Encoding enc) -> std::string
{
    if (enc == Encoding::Utf8Bom && content.size() >= 3)
    {
        return content.substr(3);
    }
    if ((enc == Encoding::Utf16LE || enc == Encoding::Utf16BE) && content.size() >= 2)
    {
        return content.substr(2);
    }
    return content;
}

auto encoding_display_name(Encoding enc) -> std::string
{
    switch (enc)
    {
        case Encoding::Utf8:
            return "UTF-8";
        case Encoding::Utf8Bom:
            return "UTF-8 BOM";
        case Encoding::Utf16LE:
            return "UTF-16 LE";
        case Encoding::Utf16BE:
            return "UTF-16 BE";
        case Encoding::Ascii:
            return "ASCII";
        case Encoding::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

} // namespace markamp::core
