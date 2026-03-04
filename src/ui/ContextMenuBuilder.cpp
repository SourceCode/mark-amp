#include "ContextMenuBuilder.h"

#include <algorithm>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════
// Item insertion helpers
// ═══════════════════════════════════════════════════════

auto ContextMenuBuilder::current_items() -> std::vector<ContextMenuItem>&
{
    if (!submenu_stack_.empty())
    {
        return submenu_stack_.top()->children;
    }
    return items_;
}

namespace
{

/// Recursively find an item by action_id across all levels.
auto find_item_recursive(std::vector<ContextMenuItem>& items, const std::string& action_id)
    -> ContextMenuItem*
{
    for (auto& menu_item : items)
    {
        if (menu_item.action_id == action_id)
        {
            return &menu_item;
        }
        if (menu_item.has_submenu)
        {
            auto* found = find_item_recursive(menu_item.children, action_id);
            if (found != nullptr)
            {
                return found;
            }
        }
    }
    return nullptr;
}

/// Recursively count non-separator items.
auto count_items_recursive(const std::vector<ContextMenuItem>& items) -> int
{
    int count = 0;
    for (const auto& menu_item : items)
    {
        if (!menu_item.is_separator && !menu_item.has_submenu)
        {
            ++count;
        }
        if (menu_item.has_submenu)
        {
            count += count_items_recursive(menu_item.children);
        }
    }
    return count;
}

/// Recursively count separators.
auto count_separators_recursive(const std::vector<ContextMenuItem>& items) -> int
{
    int count = 0;
    for (const auto& menu_item : items)
    {
        if (menu_item.is_separator)
        {
            ++count;
        }
        if (menu_item.has_submenu)
        {
            count += count_separators_recursive(menu_item.children);
        }
    }
    return count;
}

/// Recursively apply enablement predicates.
void apply_predicates(std::vector<ContextMenuItem>& items,
                      const std::unordered_map<std::string, EnablementPredicate>& predicates,
                      const ContextKeys& context)
{
    for (auto& menu_item : items)
    {
        if (!menu_item.is_separator)
        {
            auto pred_iter = predicates.find(menu_item.action_id);
            if (pred_iter != predicates.end())
            {
                menu_item.is_enabled = pred_iter->second(context);
            }
        }
        if (menu_item.has_submenu)
        {
            apply_predicates(menu_item.children, predicates, context);
        }
    }
}

} // namespace

// ═══════════════════════════════════════════════════════
// Core builder methods
// ═══════════════════════════════════════════════════════

auto ContextMenuBuilder::item(MenuActionCategory category,
                              const std::string& action_id,
                              const std::string& label,
                              const std::string& shortcut) -> ContextMenuBuilder&
{
    ContextMenuItem menu_item;
    menu_item.action_id = action_id;
    menu_item.label = label;
    menu_item.shortcut = shortcut;
    menu_item.category = category;
    current_items().push_back(menu_item);
    return *this;
}

auto ContextMenuBuilder::separator() -> ContextMenuBuilder&
{
    ContextMenuItem sep;
    sep.is_separator = true;
    current_items().push_back(sep);
    return *this;
}

auto ContextMenuBuilder::when(const std::string& action_id, EnablementPredicate predicate)
    -> ContextMenuBuilder&
{
    predicates_[action_id] = std::move(predicate);
    return *this;
}

// ═══════════════════════════════════════════════════════
// Phase 28: Extended builder API
// ═══════════════════════════════════════════════════════

auto ContextMenuBuilder::begin_submenu(const std::string& label) -> ContextMenuBuilder&
{
    ContextMenuItem submenu_item;
    submenu_item.label = label;
    submenu_item.has_submenu = true;
    submenu_item.action_id = "submenu." + label;
    current_items().push_back(submenu_item);
    submenu_stack_.push(&current_items().back());
    return *this;
}

auto ContextMenuBuilder::end_submenu() -> ContextMenuBuilder&
{
    if (!submenu_stack_.empty())
    {
        submenu_stack_.pop();
    }
    return *this;
}

auto ContextMenuBuilder::icon(const std::string& action_id, const std::string& name)
    -> ContextMenuBuilder&
{
    auto* found = find_item_recursive(items_, action_id);
    if (found != nullptr)
    {
        found->icon_name = name;
    }
    return *this;
}

auto ContextMenuBuilder::checked(const std::string& action_id, bool is_checked)
    -> ContextMenuBuilder&
{
    auto* found = find_item_recursive(items_, action_id);
    if (found != nullptr)
    {
        found->is_checked = is_checked;
    }
    return *this;
}

auto ContextMenuBuilder::disabled_tooltip(const std::string& action_id, const std::string& tooltip)
    -> ContextMenuBuilder&
{
    auto* found = find_item_recursive(items_, action_id);
    if (found != nullptr)
    {
        found->disabled_tooltip = tooltip;
    }
    return *this;
}

auto ContextMenuBuilder::dynamic(DynamicItemGenerator generator) -> ContextMenuBuilder&
{
    generators_.push_back(std::move(generator));
    return *this;
}

auto ContextMenuBuilder::on_action(const std::string& action_id, ActionCallback callback)
    -> ContextMenuBuilder&
{
    callbacks_[action_id] = std::move(callback);
    return *this;
}

// ═══════════════════════════════════════════════════════
// Build
// ═══════════════════════════════════════════════════════

auto ContextMenuBuilder::build() const -> ContextMenuModel
{
    ContextMenuModel model;
    model.set_items(items_);
    return model;
}

auto ContextMenuBuilder::build(const ContextKeys& context) const -> ContextMenuModel
{
    // Start with static items
    auto items = items_;

    // Inject dynamic items
    for (const auto& generator : generators_)
    {
        auto dynamic_items = generator(context);
        items.insert(items.end(), dynamic_items.begin(), dynamic_items.end());
    }

    // Apply enablement predicates recursively
    apply_predicates(items, predicates_, context);

    ContextMenuModel model;
    model.set_items(items);
    return model;
}

auto ContextMenuBuilder::build_callbacks() const -> std::unordered_map<std::string, ActionCallback>
{
    return callbacks_;
}

// ═══════════════════════════════════════════════════════
// Introspection
// ═══════════════════════════════════════════════════════

auto ContextMenuBuilder::item_count() const -> int
{
    return count_items_recursive(items_);
}

auto ContextMenuBuilder::separator_count() const -> int
{
    return count_separators_recursive(items_);
}

auto ContextMenuBuilder::has_predicates() const -> bool
{
    return !predicates_.empty();
}

auto ContextMenuBuilder::submenu_depth() const -> int
{
    return static_cast<int>(submenu_stack_.size());
}

void ContextMenuBuilder::clear()
{
    items_.clear();
    predicates_.clear();
    callbacks_.clear();
    generators_.clear();
    while (!submenu_stack_.empty())
    {
        submenu_stack_.pop();
    }
}

} // namespace markamp::ui
