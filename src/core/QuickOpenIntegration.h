/// @file QuickOpenIntegration.h
/// @brief P05-T03: Unifies quick open, command palette, and navigation indexes.
///
/// Shares navigation source providers and ensures palette open/reveal behavior
/// matches explorer navigation. Records actions into navigation history.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// Palette navigation mode.
enum class QuickOpenMode
{
    kFile,    ///< Open file by name
    kSymbol,  ///< Go to symbol
    kCommand, ///< Execute command
    kRecent,  ///< Recent files (MRU)
};

/// A palette navigation item.
struct QuickOpenItem
{
    std::string label;
    std::string detail;
    std::string file_path;
    int line_number{0};
    QuickOpenMode mode{QuickOpenMode::kFile};
};

/// Integrates quick open modes with consistent navigation behavior.
class QuickOpenIntegration
{
public:
    explicit QuickOpenIntegration(EventBus& bus);

    /// Activate a quick-open item (opens file, executes command, etc.)
    void activate(const QuickOpenItem& item);

    /// Record an item into recent/MRU list.
    void record_mru(const std::string& file_path);

    /// Get recent items.
    [[nodiscard]] auto recent_items() const -> const std::vector<std::string>&
    {
        return mru_list_;
    }

    /// Get MRU size.
    [[nodiscard]] auto mru_count() const -> int
    {
        return static_cast<int>(mru_list_.size());
    }

    /// Clear MRU list.
    void clear_mru();

private:
    EventBus& event_bus_;
    std::vector<std::string> mru_list_;
    static constexpr int kMaxMruSize = 50;
};

} // namespace markamp::core
