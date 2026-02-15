// ============================================================================
// File: src/core/DiagramRendererRegistry.cpp
// Phase 30: Diagram Rendering Extended — Registry implementation
// ============================================================================
#include "DiagramRendererRegistry.h"

namespace markamp::core
{

void DiagramRendererRegistry::register_renderer(std::unique_ptr<IDiagramRenderer> renderer)
{
    if (!renderer)
    {
        return;
    }

    const auto lang = renderer->language_id();
    std::lock_guard lock(mutex_);
    renderers_[lang] = std::move(renderer);
}

auto DiagramRendererRegistry::get_renderer(const std::string& language_id) const
    -> IDiagramRenderer*
{
    std::lock_guard lock(mutex_);
    const auto found = renderers_.find(language_id);
    if (found != renderers_.end())
    {
        return found->second.get();
    }
    return nullptr;
}

auto DiagramRendererRegistry::registered_languages() const -> std::vector<std::string>
{
    std::lock_guard lock(mutex_);
    std::vector<std::string> languages;
    languages.reserve(renderers_.size());

    for (const auto& [lang_id, renderer] : renderers_)
    {
        languages.push_back(lang_id);
    }

    return languages;
}

auto DiagramRendererRegistry::has_renderer(const std::string& language_id) const -> bool
{
    std::lock_guard lock(mutex_);
    return renderers_.contains(language_id);
}

auto DiagramRendererRegistry::renderer_status() const -> std::vector<std::pair<std::string, bool>>
{
    std::lock_guard lock(mutex_);
    std::vector<std::pair<std::string, bool>> status;
    status.reserve(renderers_.size());

    for (const auto& [lang_id, renderer] : renderers_)
    {
        status.emplace_back(lang_id, renderer->is_available());
    }

    return status;
}

} // namespace markamp::core
