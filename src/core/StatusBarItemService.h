#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Alignment of a status bar item (mirrors VS Code's StatusBarAlignment).
enum class StatusBarAlignment
{
    kLeft,
    kRight,
};

/// Priority determines ordering within the same alignment group.
/// Higher priority = further from edge.
struct StatusBarItemOptions
{
    std::string id;
    StatusBarAlignment alignment{StatusBarAlignment::kLeft};
    int priority{0};
};

/// A managed status bar item created by an extension.
/// Mirrors VS Code's `vscode.StatusBarItem`.
class StatusBarItem
{
public:
    explicit StatusBarItem(StatusBarItemOptions options);

    // ── Accessors ──
    [[nodiscard]] auto id() const -> const std::string&
    {
        return options_.id;
    }
    [[nodiscard]] auto text() const -> const std::string&
    {
        return text_;
    }
    [[nodiscard]] auto tooltip() const -> const std::string&
    {
        return tooltip_;
    }
    [[nodiscard]] auto command() const -> const std::string&
    {
        return command_;
    }
    [[nodiscard]] auto alignment() const -> StatusBarAlignment
    {
        return options_.alignment;
    }
    [[nodiscard]] auto priority() const -> int
    {
        return options_.priority;
    }
    [[nodiscard]] auto is_visible() const -> bool
    {
        return visible_;
    }

    // ── Mutators ──
    void set_text(const std::string& text)
    {
        text_ = text;
    }
    void set_tooltip(const std::string& tooltip)
    {
        tooltip_ = tooltip;
    }
    void set_command(const std::string& command)
    {
        command_ = command;
    }
    void show()
    {
        visible_ = true;
    }
    void hide()
    {
        visible_ = false;
    }
    void dispose()
    {
        disposed_ = true;
        visible_ = false;
    }
    [[nodiscard]] auto is_disposed() const -> bool
    {
        return disposed_;
    }

private:
    StatusBarItemOptions options_;
    std::string text_;
    std::string tooltip_;
    std::string command_;
    bool visible_{false};
    bool disposed_{false};
};

/// Service for extensions to create and manage status bar items.
///
/// Bridges the gap between extension-contributed status bar items
/// (declared in ExtensionManifest or created at runtime) and the
/// StatusBarPanel UI component.
///
/// Injected into `PluginContext` so extensions can call:
///   `auto* item = ctx.status_bar_item_service->create_item({.id = "myExt.status"});`
class StatusBarItemService
{
public:
    StatusBarItemService() = default;

    /// Create a new status bar item owned by the calling extension.
    auto create_item(const StatusBarItemOptions& options) -> StatusBarItem*;

    /// Get an existing item by its ID.
    [[nodiscard]] auto get_item(const std::string& item_id) -> StatusBarItem*;

    /// Dispose (remove) an item by its ID.
    void dispose_item(const std::string& item_id);

    /// Get all visible status bar items for rendering.
    [[nodiscard]] auto get_visible_items() const -> std::vector<const StatusBarItem*>;

    /// Get all items (visible or not).
    [[nodiscard]] auto get_all_items() const -> std::vector<const StatusBarItem*>;

    /// Number of items.
    [[nodiscard]] auto item_count() const -> std::size_t
    {
        return items_.size();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<StatusBarItem>> items_;
};

} // namespace markamp::core
