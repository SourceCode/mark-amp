// ============================================================================
// File: src/ui/PeekEditorHost.h
// Phase 47: Peek View System — Editor host model
// ============================================================================
#pragma once

#include <string>

namespace markamp::ui
{

/// Model for the peek preview editor (read-only).
class PeekEditorHostModel
{
public:
    PeekEditorHostModel() = default;

    /// Load content for display.
    void load_content(const std::string& file_path, const std::string& content, int target_line);

    /// Get the file path.
    [[nodiscard]] auto file_path() const -> const std::string&
    {
        return file_path_;
    }

    /// Get the content.
    [[nodiscard]] auto content() const -> const std::string&
    {
        return content_;
    }

    /// Get the target highlight line.
    [[nodiscard]] auto target_line() const -> int
    {
        return target_line_;
    }

    /// Get the scroll-to line (centers target in viewport).
    [[nodiscard]] auto scroll_to_line() const -> int
    {
        return scroll_to_line_;
    }

    /// Set visible line range (for virtual scrolling).
    void set_visible_range(int first_line, int last_line);

    /// Get total line count.
    [[nodiscard]] auto total_lines() const -> int
    {
        return total_lines_;
    }

    /// Whether content is loaded.
    [[nodiscard]] auto is_loaded() const -> bool
    {
        return is_loaded_;
    }

private:
    std::string file_path_;
    std::string content_;
    int target_line_{0};
    int scroll_to_line_{0};
    int total_lines_{0};
    int visible_first_{0};
    int visible_last_{0};
    bool is_loaded_{false};
};

} // namespace markamp::ui
