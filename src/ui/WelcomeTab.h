// ============================================================================
// File: src/ui/WelcomeTab.h
// Phase 48: Welcome and Onboarding — Welcome tab model
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Quick action entry on the welcome tab.
struct WelcomeAction
{
    std::string id;
    std::string label;
    std::string icon_name;
    std::string command_id; ///< CommandRegistry command to execute
};

/// A recent file/workspace entry.
struct RecentItem
{
    std::string path;
    std::string name;
    std::chrono::system_clock::time_point last_opened;
    bool is_workspace{false};
};

/// Walkthrough card on the welcome tab.
struct WalkthroughCard
{
    std::string id;
    std::string title;
    std::string description;
    int total_steps{0};
    int completed_steps{0};

    [[nodiscard]] auto completion_percentage() const -> int
    {
        return total_steps > 0 ? (completed_steps * 100) / total_steps : 0;
    }
};

/// Welcome tab layout breakpoint.
enum class WelcomeLayout : uint8_t
{
    Compact, ///< < 800px — single column
    Normal,  ///< 800px – 1200px — two columns
    Wide     ///< > 1200px — two columns with extra space
};

/// Model for the Welcome Tab panel.
class WelcomeTabModel
{
public:
    WelcomeTabModel() = default;

    // --- Quick Actions ---
    void add_action(WelcomeAction action);
    [[nodiscard]] auto actions() const -> const std::vector<WelcomeAction>&
    {
        return actions_;
    }

    // --- Recent Items ---
    void set_recent_items(std::vector<RecentItem> items);
    [[nodiscard]] auto recent_items() const -> const std::vector<RecentItem>&
    {
        return recent_items_;
    }
    void filter_recent(const std::string& query);
    [[nodiscard]] auto filtered_recent() const -> const std::vector<RecentItem>&
    {
        return filtered_items_;
    }

    // --- Walkthroughs ---
    void add_walkthrough(WalkthroughCard card);
    [[nodiscard]] auto walkthroughs() const -> const std::vector<WalkthroughCard>&
    {
        return walkthroughs_;
    }

    // --- Show on startup ---
    void set_show_on_startup(bool show)
    {
        show_on_startup_ = show;
    }
    [[nodiscard]] auto show_on_startup() const -> bool
    {
        return show_on_startup_;
    }

    // --- Layout ---
    void set_width(int width);
    [[nodiscard]] auto layout() const -> WelcomeLayout
    {
        return layout_;
    }

    // --- Version ---
    void set_version(const std::string& version)
    {
        version_ = version;
    }
    [[nodiscard]] auto version() const -> const std::string&
    {
        return version_;
    }

private:
    std::vector<WelcomeAction> actions_;
    std::vector<RecentItem> recent_items_;
    std::vector<RecentItem> filtered_items_;
    std::vector<WalkthroughCard> walkthroughs_;
    bool show_on_startup_{true};
    WelcomeLayout layout_{WelcomeLayout::Normal};
    std::string version_;
};

} // namespace markamp::ui
