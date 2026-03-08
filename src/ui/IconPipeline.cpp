/// @file IconPipeline.cpp
/// @brief V16 Phase 03-04 — Unified icon pipeline implementation.

#include "ui/IconPipeline.h"

#include "ui/IconRenderer.h"
#include "ui/SvgDocument.h"

#include <fstream>
#include <sstream>

namespace markamp::ui
{

auto IconPipeline::initialize(const IconPipelineConfig& config) -> bool
{
    config_ = config;
    errors_.clear();

    if (!manifest_.load_from_file(config_.manifest_path))
    {
        for (const auto& err : manifest_.validation_errors())
        {
            errors_.push_back("Manifest: " + err);
        }
    }

    initialized_ = true;
    return manifest_.validation_errors().empty();
}

auto IconPipeline::is_initialized() const -> bool
{
    return initialized_;
}

// ── File icon rendering ──

auto IconPipeline::get_file_icon(const std::string& filename,
                                 const wxSize& size,
                                 const wxColour& color,
                                 double scale) -> wxBitmap
{
    auto icon_id = manifest_.resolve_file_icon(filename);
    return get_icon_by_id(icon_id, size, color, scale);
}

auto IconPipeline::get_icon_by_id(const std::string& canonical_id,
                                  const wxSize& size,
                                  const wxColour& color,
                                  double scale) -> wxBitmap
{
    // Try cache first, then load and render
    if (ensure_icon_loaded(canonical_id))
    {
        return render_cached(canonical_id, size, color, scale);
    }

    // Fallback: try default icon
    if (canonical_id != IconManifest::kFallbackFileIcon)
    {
        if (ensure_icon_loaded(IconManifest::kFallbackFileIcon))
        {
            return render_cached(IconManifest::kFallbackFileIcon, size, color, scale);
        }
    }

    // Last resort: return empty bitmap
    return {size.GetWidth(), size.GetHeight()};
}

// ── Folder icon rendering ──

auto IconPipeline::get_open_folder_icon(const std::string& folder_name,
                                        const wxSize& size,
                                        const wxColour& color,
                                        double scale) -> wxBitmap
{
    auto folder_entry = manifest_.resolve_folder_icon(folder_name);
    std::string asset_path;

    if (folder_entry.has_value())
    {
        asset_path = folder_entry->open_asset;
    }
    else
    {
        asset_path = manifest_.default_folder_icon().open_asset;
    }

    // Use asset path as the icon name in registry
    auto icon_name = "folder_open_" + folder_name;
    if (!registry_.has_icon(icon_name))
    {
        auto full_path = resolve_asset_path(asset_path);
        registry_.load_icon(icon_name, full_path);
    }

    return render_cached(icon_name, size, color, scale);
}

auto IconPipeline::get_closed_folder_icon(const std::string& folder_name,
                                          const wxSize& size,
                                          const wxColour& color,
                                          double scale) -> wxBitmap
{
    auto folder_entry = manifest_.resolve_folder_icon(folder_name);
    std::string asset_path;

    if (folder_entry.has_value())
    {
        asset_path = folder_entry->closed_asset;
    }
    else
    {
        asset_path = manifest_.default_folder_icon().closed_asset;
    }

    auto icon_name = "folder_closed_" + folder_name;
    if (!registry_.has_icon(icon_name))
    {
        auto full_path = resolve_asset_path(asset_path);
        registry_.load_icon(icon_name, full_path);
    }

    return render_cached(icon_name, size, color, scale);
}

// ── Command icon rendering ──

auto IconPipeline::get_command_icon(const std::string& command_id,
                                    const wxSize& size,
                                    const wxColour& color,
                                    double scale) -> wxBitmap
{
    auto icon_id = manifest_.resolve_command_icon(command_id);
    return get_icon_by_id(icon_id, size, color, scale);
}

// ── Pipeline management ──

void IconPipeline::invalidate_cache()
{
    cache_.clear();
}

auto IconPipeline::cache_size() const -> size_t
{
    return cache_.size();
}

void IconPipeline::warm_cache(const wxSize& size, const wxColour& color, double scale)
{
    // Pre-load and render the most commonly used icons
    static const std::vector<std::string> kCommonIds = {
        "file_default",
        "file_cpp",
        "file_header",
        "file_python",
        "file_javascript",
        "file_typescript",
        "file_markdown",
        "file_json",
        "file_yaml",
        "file_html",
        "file_css",
        "file_rust",
        "file_go",
        "file_shell",
        "folder_default",
    };

    for (const auto& icon_id : kCommonIds)
    {
        if (ensure_icon_loaded(icon_id))
        {
            [[maybe_unused]] auto loaded = render_cached(icon_id, size, color, scale);
        }
    }
}

auto IconPipeline::manifest() const -> const IconManifest&
{
    return manifest_;
}

auto IconPipeline::errors() const -> const std::vector<std::string>&
{
    return errors_;
}

// ── Private ──

auto IconPipeline::ensure_icon_loaded(const std::string& canonical_id) -> bool
{
    if (registry_.has_icon(canonical_id))
    {
        return true;
    }

    // Look up asset path in manifest
    auto entry = manifest_.get_entry(canonical_id);
    if (!entry.has_value())
    {
        return false;
    }

    auto full_path = resolve_asset_path(entry->asset_path);
    return registry_.load_icon(canonical_id, full_path);
}

auto IconPipeline::resolve_asset_path(const std::string& relative_path) const
    -> std::filesystem::path
{
    return config_.asset_root / relative_path;
}

auto IconPipeline::render_cached(const std::string& icon_name,
                                 const wxSize& size,
                                 const wxColour& color,
                                 double scale) -> wxBitmap
{
    // Build cache key
    IconCacheKey key;
    key.icon_name = icon_name;
    key.size_x = size.GetWidth();
    key.size_y = size.GetHeight();
    key.color_rgba = IconCache::color_to_rgba(color);
    key.scale = scale;

    // Check cache
    auto cached = cache_.get(key);
    if (cached.has_value())
    {
        return *cached;
    }

    // Render from registry
    auto svg_doc = registry_.get_icon(icon_name);
    if (!svg_doc.has_value())
    {
        return {size.GetWidth(), size.GetHeight()};
    }

    auto bmp = IconRenderer::RenderIcon(*svg_doc, size, color, scale);
    cache_.put(key, bmp);
    return bmp;
}

} // namespace markamp::ui
