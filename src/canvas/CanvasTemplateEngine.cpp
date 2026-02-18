// ============================================================================
// File: src/canvas/CanvasTemplateEngine.cpp
// Phase 14: Canvas Extensibility — board template engine
// ============================================================================
#include "canvas/CanvasTemplateEngine.h"

#include "core/Events.h"

#include <algorithm>
#include <set>

namespace markamp::canvas
{

CanvasTemplateEngine::CanvasTemplateEngine(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Registration ──────────────────────────────────────────────────

auto CanvasTemplateEngine::register_template(const CanvasTemplate& tmpl) -> bool
{
    if (tmpl.template_id.empty())
    {
        return false;
    }

    if (templates_.contains(tmpl.template_id))
    {
        return false; // Duplicate
    }

    templates_.emplace(tmpl.template_id, tmpl);
    return true;
}

auto CanvasTemplateEngine::unregister_template(const std::string& template_id) -> bool
{
    return templates_.erase(template_id) > 0;
}

auto CanvasTemplateEngine::unregister_extension_templates(const std::string& extension_id) -> size_t
{
    size_t removed = 0;
    for (auto iter = templates_.begin(); iter != templates_.end();)
    {
        if (iter->second.extension_id == extension_id && !iter->second.is_built_in)
        {
            iter = templates_.erase(iter);
            ++removed;
        }
        else
        {
            ++iter;
        }
    }
    return removed;
}

// ── Application ───────────────────────────────────────────────────

auto CanvasTemplateEngine::apply_template(const std::string& template_id,
                                          double offset_x,
                                          double offset_y) -> TemplateApplicationResult
{
    auto iter = templates_.find(template_id);
    if (iter == templates_.end())
    {
        return {false, "Template not found: " + template_id, template_id, 0};
    }

    const auto& tmpl = iter->second;

    // Count approximate objects from board_json
    // In a real implementation this would parse JSON and create objects.
    // For now, we estimate based on content size.
    int estimated_objects = 0;
    if (!tmpl.board_json.empty())
    {
        // Simple heuristic: count "{" in JSON as rough object count
        estimated_objects = static_cast<int>(std::ranges::count(tmpl.board_json, '{'));
        if (estimated_objects == 0)
        {
            estimated_objects = 1;
        }
    }

    // Apply offset (recorded but actual object creation is UI-side)
    static_cast<void>(offset_x);
    static_cast<void>(offset_y);

    // Publish event
    core::events::CanvasAppRegisteredEvent evt;
    evt.app_id = template_id;
    evt.app_name = tmpl.name;
    evt.extension_id = tmpl.extension_id;
    event_bus_.publish(evt);

    return {true, "", template_id, estimated_objects};
}

// ── Query ─────────────────────────────────────────────────────────

auto CanvasTemplateEngine::find_template(const std::string& template_id) const
    -> const CanvasTemplate*
{
    auto iter = templates_.find(template_id);
    return iter != templates_.end() ? &iter->second : nullptr;
}

auto CanvasTemplateEngine::list_templates() const
    -> const std::unordered_map<std::string, CanvasTemplate>&
{
    return templates_;
}

auto CanvasTemplateEngine::templates_in_category(const std::string& category) const
    -> std::vector<const CanvasTemplate*>
{
    std::vector<const CanvasTemplate*> result;
    for (const auto& [tid, tmpl] : templates_)
    {
        if (tmpl.category == category)
        {
            result.push_back(&tmpl);
        }
    }
    return result;
}

auto CanvasTemplateEngine::search_templates(const std::string& query) const
    -> std::vector<const CanvasTemplate*>
{
    std::vector<const CanvasTemplate*> result;

    if (query.empty())
    {
        // Return all templates
        for (const auto& [tid, tmpl] : templates_)
        {
            result.push_back(&tmpl);
        }
        return result;
    }

    // Case-insensitive search across name, description, and tags
    std::string lower_query = query;
    std::ranges::transform(
        lower_query, lower_query.begin(), [](unsigned char chr) { return std::tolower(chr); });

    for (const auto& [tid, tmpl] : templates_)
    {
        // Search name
        std::string lower_name = tmpl.name;
        std::ranges::transform(
            lower_name, lower_name.begin(), [](unsigned char chr) { return std::tolower(chr); });
        if (lower_name.find(lower_query) != std::string::npos)
        {
            result.push_back(&tmpl);
            continue;
        }

        // Search description
        std::string lower_desc = tmpl.description;
        std::ranges::transform(
            lower_desc, lower_desc.begin(), [](unsigned char chr) { return std::tolower(chr); });
        if (lower_desc.find(lower_query) != std::string::npos)
        {
            result.push_back(&tmpl);
            continue;
        }

        // Search tags
        bool tag_match = false;
        for (const auto& tag : tmpl.tags)
        {
            std::string lower_tag = tag;
            std::ranges::transform(
                lower_tag, lower_tag.begin(), [](unsigned char chr) { return std::tolower(chr); });
            if (lower_tag.find(lower_query) != std::string::npos)
            {
                tag_match = true;
                break;
            }
        }
        if (tag_match)
        {
            result.push_back(&tmpl);
        }
    }

    return result;
}

auto CanvasTemplateEngine::template_categories() const -> std::vector<std::string>
{
    std::set<std::string> categories;
    for (const auto& [tid, tmpl] : templates_)
    {
        if (!tmpl.category.empty())
        {
            categories.insert(tmpl.category);
        }
    }
    return {categories.begin(), categories.end()};
}

auto CanvasTemplateEngine::is_registered(const std::string& template_id) const -> bool
{
    return templates_.contains(template_id);
}

auto CanvasTemplateEngine::template_count() const -> size_t
{
    return templates_.size();
}

// ── Cleanup ───────────────────────────────────────────────────────

auto CanvasTemplateEngine::clear() -> void
{
    templates_.clear();
}

} // namespace markamp::canvas
