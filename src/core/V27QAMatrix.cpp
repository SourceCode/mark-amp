/// @file V27QAMatrix.cpp
/// @brief V27 Phase 20 — QA matrix implementation.
#include "core/V27QAMatrix.h"
#include <algorithm>

namespace markamp::core
{

void V27QAMatrix::register_surface(const V27QASurfaceEntry& entry)
{
    entries_.push_back(entry);
}

auto V27QAMatrix::surface(const std::string& name) const -> const V27QASurfaceEntry*
{
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&name](const V27QASurfaceEntry& e) { return e.surface_name == name; });
    return it != entries_.end() ? &(*it) : nullptr;
}

auto V27QAMatrix::all_pass() const noexcept -> bool
{
    return std::all_of(entries_.begin(), entries_.end(),
        [](const V27QASurfaceEntry& e) { return e.all_pass(); });
}

auto V27QAMatrix::pass_count() const noexcept -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [](const V27QASurfaceEntry& e) { return e.all_pass(); }));
}

auto V27QAMatrix::incomplete_surfaces() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& e : entries_)
        if (!e.all_pass()) result.push_back(e.surface_name);
    return result;
}

void V27QAMatrix::populate_v27_surfaces()
{
    const char* surfaces[] = {
        "Shell", "ActivityBar", "Toolbar", "TabBar", "Breadcrumb",
        "CommandPalette", "ContextMenu", "FileTree", "Editor",
        "Notebook", "Canvas", "Panel", "Settings", "ThemeGallery",
        "Dialog", "Tooltip", "Notification", "StatusBar",
    };
    for (const auto* name : surfaces)
        register_surface({name});
}

} // namespace markamp::core
