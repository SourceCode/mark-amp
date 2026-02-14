#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Options for a text decoration type (mirrors VS Code's DecorationRenderOptions).
struct DecorationOptions
{
    std::string background_color;
    std::string border_color;
    std::string border_style; // "solid", "dashed", "dotted"
    std::string border_width; // "1px"
    std::string outline_color;
    std::string outline_style;
    std::string gutter_icon_path;
    std::string after_content_text;  // Text to show after decorated range
    std::string before_content_text; // Text to show before decorated range
    bool is_whole_line{false};
};

/// A decoration range to apply to text.
struct DecorationRange
{
    int start_line{0};
    int start_character{0};
    int end_line{0};
    int end_character{0};
    std::string hover_message; // Optional hover text for this range
};

/// Handle to a decoration type.
using DecorationTypeHandle = std::uint32_t;

/// Service for managing text editor decorations (mirrors VS Code's TextEditorDecorationType).
class DecorationService
{
public:
    DecorationService() = default;

    /// Create a new decoration type and return its handle.
    auto create_decoration_type(DecorationOptions options) -> DecorationTypeHandle;

    /// Set decorations for a file + decoration type.
    void set_decorations(const std::string& file_uri,
                         DecorationTypeHandle type_handle,
                         std::vector<DecorationRange> ranges);

    /// Get decorations for a file + decoration type.
    [[nodiscard]] auto get_decorations(const std::string& file_uri,
                                       DecorationTypeHandle type_handle) const
        -> const std::vector<DecorationRange>&;

    /// Get decoration options for a type handle.
    [[nodiscard]] auto get_options(DecorationTypeHandle type_handle) const
        -> const DecorationOptions*;

    /// Remove a decoration type and all its decorations.
    void dispose_decoration_type(DecorationTypeHandle type_handle);

    /// Clear all decorations for a file.
    void clear_file_decorations(const std::string& file_uri);

    /// Listener for decoration changes.
    using ChangeListener = std::function<void(const std::string& file_uri)>;
    auto on_change(ChangeListener listener) -> std::size_t;
    void remove_listener(std::size_t listener_id);

private:
    DecorationTypeHandle next_handle_{1};
    std::unordered_map<DecorationTypeHandle, DecorationOptions> types_;

    /// file_uri → (type_handle → ranges)
    std::unordered_map<std::string,
                       std::unordered_map<DecorationTypeHandle, std::vector<DecorationRange>>>
        decorations_;

    static const std::vector<DecorationRange> kEmptyRanges;

    std::vector<std::pair<std::size_t, ChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void fire_change(const std::string& file_uri);
};

} // namespace markamp::core
