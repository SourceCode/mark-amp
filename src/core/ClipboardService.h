/// @file ClipboardService.h
/// @brief V4 Phase 17 – Copy/Paste with Formatting service.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

/// Type of pasted content detected.
enum class PasteContentType : uint8_t
{
    kPlainText,
    kHtml,
    kImage,
    kUrl,
    kFilePaths,
    kCsvTable,
    kUnknown
};

/// Describes content from the clipboard.
struct PasteContent
{
    PasteContentType type{PasteContentType::kPlainText};
    std::string text;
    std::string html;
    std::vector<uint8_t> image_data;
    std::string image_format; ///< e.g. "png", "jpg"
    std::vector<std::string> file_paths;
    std::string url;
};

/// Result of a paste conversion.
struct PasteResult
{
    std::string markdown;
    std::string saved_file_path; ///< When an image is saved
    bool success{false};
    std::string error;
};

/// Clipboard conversion service for rich paste.
class ClipboardService
{
public:
    ClipboardService(EventBus& event_bus, VaultService& vault_service);

    /// Convert HTML string to Markdown.
    [[nodiscard]] auto html_to_markdown(const std::string& html) const -> std::string;

    /// Convert CSV text to a Markdown table.
    [[nodiscard]] auto csv_to_markdown_table(const std::string& csv) const -> std::string;

    /// Convert a URL to a Markdown link (or image embed if image URL).
    [[nodiscard]] auto url_to_markdown_link(const std::string& url) const -> std::string;

    /// Strip all HTML tags from a string.
    [[nodiscard]] static auto strip_html_tags(const std::string& html) -> std::string;

    /// Detect the type of paste content.
    [[nodiscard]] auto detect_content_type(const PasteContent& content) const -> PasteContentType;

    /// Full conversion pipeline: detect type, convert appropriately.
    [[nodiscard]] auto convert_to_markdown(const PasteContent& content) const -> PasteResult;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    /// Convert a single HTML tag to its Markdown equivalent.
    [[nodiscard]] static auto convert_tag(const std::string& tag,
                                          const std::string& inner,
                                          const std::string& attrs) -> std::string;

    /// Extract attribute value from an HTML tag.
    [[nodiscard]] static auto extract_attr(const std::string& attrs, const std::string& name)
        -> std::string;
};

} // namespace markamp::core
