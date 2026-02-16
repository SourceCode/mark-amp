/// LargeFileMode.h — Phase 35: Large File Mode
///
/// Auto-activates when file size exceeds a threshold. Disables
/// expensive features (syntax highlighting, minimap, bracket matching)
/// to maintain responsiveness.
///
/// Pattern implemented: #31 Large file mode

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace markamp::core
{

/// Features that can be toggled off in large file mode.
struct LargeFileFeatures
{
    bool syntax_highlighting{true};
    bool minimap{true};
    bool bracket_matching{true};
    bool line_numbers{true};
    bool folding{true};
    bool word_wrap{true};
    bool auto_indent{true};
    bool link_detection{true};
    bool spell_check{true};
};

/// Large file mode controller.
///
/// Usage:
///   LargeFileMode lfm;
///   lfm.set_threshold(5 * 1024 * 1024); // 5 MB
///   lfm.evaluate(file_size, line_count);
///   if (lfm.is_active()) { /* disable features */ }
///   auto features = lfm.allowed_features();
class LargeFileMode
{
public:
    static constexpr std::size_t kDefaultSizeThreshold = 2 * 1024 * 1024; // 2 MB
    static constexpr std::size_t kDefaultLineThreshold = 50'000;

    /// Set the file size threshold for activation.
    void set_size_threshold(std::size_t bytes)
    {
        size_threshold_ = bytes;
    }

    /// Set the line count threshold for activation.
    void set_line_threshold(std::size_t lines)
    {
        line_threshold_ = lines;
    }

    /// Evaluate whether large file mode should be active.
    void evaluate(std::size_t file_size_bytes, std::size_t line_count)
    {
        file_size_ = file_size_bytes;
        line_count_ = line_count;

        if (force_active_)
        {
            active_ = true;
        }
        else if (force_inactive_)
        {
            active_ = false;
        }
        else
        {
            active_ = file_size_bytes >= size_threshold_ || line_count >= line_threshold_;
        }

        if (active_)
        {
            compute_features();
        }
        else
        {
            features_ = LargeFileFeatures{}; // all enabled
        }
    }

    /// Force large file mode on/off (user override).
    void force_active(bool active)
    {
        force_active_ = active;
        force_inactive_ = !active;
    }

    /// Clear any forced state.
    void clear_force()
    {
        force_active_ = false;
        force_inactive_ = false;
    }

    /// Whether large file mode is currently active.
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return active_;
    }

    /// Get which features are allowed.
    [[nodiscard]] auto allowed_features() const noexcept -> const LargeFileFeatures&
    {
        return features_;
    }

    /// Current file size being tracked.
    [[nodiscard]] auto file_size() const noexcept -> std::size_t
    {
        return file_size_;
    }

    /// Current line count being tracked.
    [[nodiscard]] auto line_count() const noexcept -> std::size_t
    {
        return line_count_;
    }

    /// Size threshold.
    [[nodiscard]] auto size_threshold() const noexcept -> std::size_t
    {
        return size_threshold_;
    }

    /// Line threshold.
    [[nodiscard]] auto line_threshold() const noexcept -> std::size_t
    {
        return line_threshold_;
    }

    /// Human-readable status string.
    [[nodiscard]] auto status_text() const -> std::string
    {
        if (!active_)
        {
            return "Normal mode";
        }
        return "Large file mode (file: " + std::to_string(file_size_ / 1024) + " KB, " +
               std::to_string(line_count_) + " lines)";
    }

private:
    std::size_t size_threshold_{kDefaultSizeThreshold};
    std::size_t line_threshold_{kDefaultLineThreshold};
    std::size_t file_size_{0};
    std::size_t line_count_{0};
    bool active_{false};
    bool force_active_{false};
    bool force_inactive_{false};
    LargeFileFeatures features_;

    void compute_features()
    {
        // Progressively disable features based on file size
        features_.syntax_highlighting = (file_size_ < size_threshold_ * 5);
        features_.minimap = false; // always off in large file mode
        features_.bracket_matching = (file_size_ < size_threshold_ * 3);
        features_.folding = (file_size_ < size_threshold_ * 2);
        features_.word_wrap = (file_size_ < size_threshold_ * 4);
        features_.link_detection = false;
        features_.spell_check = false;
        features_.line_numbers = true; // always keep line numbers
        features_.auto_indent = true;  // always keep auto-indent
    }
};

} // namespace markamp::core
