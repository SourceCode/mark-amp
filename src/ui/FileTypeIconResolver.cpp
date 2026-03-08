/// @file FileTypeIconResolver.cpp
/// @brief V16 Phase 04 — Refactored to use manifest-driven icon resolution.
///
/// Delegates to IconManifest for file→icon mapping, falling back to
/// hardcoded mappings for backward compatibility.

#include "ui/FileTypeIconResolver.h"

#include "ui/IconManifest.h"

#include <algorithm>
#include <filesystem>

namespace markamp::ui
{

namespace
{

/// Return a shared manifest instance loaded once on first use.
/// In production, this will be replaced by the IconPipeline singleton.
auto get_manifest() -> const IconManifest&
{
    static const IconManifest manifest = []() -> IconManifest
    {
        IconManifest result;
        const std::filesystem::path manifest_path = "resources/icons/icon_manifest.json";
        if (std::filesystem::exists(manifest_path))
        {
            [[maybe_unused]] auto loaded = result.load_from_file(manifest_path);
        }
        return result;
    }();
    return manifest;
}

/// Convert manifest canonical IDs (e.g., "file_cpp") to registry-style names
/// (e.g., "filetype-cpp") for backward compatibility with existing SVG registry.
auto canonical_to_registry_name(const std::string& canonical_id) -> std::string
{
    if (canonical_id.empty() || canonical_id == IconManifest::kFallbackFileIcon)
    {
        return "filetype-default";
    }

    // Convert "file_cpp" → "filetype-cpp", "file_header" → "filetype-header", etc.
    std::string result = canonical_id;
    if (result.substr(0, 5) == "file_")
    {
        result = "filetype-" + result.substr(5);
    }
    return result;
}

} // namespace

auto FileTypeIconResolver::GetFileIcon(const std::string& filename) -> std::string
{
    if (filename.empty())
    {
        return "filetype-default";
    }

    // Use manifest for resolution
    const auto& manifest = get_manifest();
    if (manifest.icon_count() > 0)
    {
        auto canonical_id = manifest.resolve_file_icon(filename);
        return canonical_to_registry_name(canonical_id);
    }

    // Fallback: legacy hardcoded resolution for when manifest isn't available
    const std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    std::transform(
        ext.begin(), ext.end(), ext.begin(), [](unsigned char chr) { return std::tolower(chr); });

    if (ext == ".md" || ext == ".markdown")
    {
        return "filetype-markdown";
    }
    if (ext == ".cpp" || ext == ".cxx" || ext == ".cc" || ext == ".c")
    {
        return "filetype-cpp";
    }
    if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
    {
        return "filetype-header";
    }
    if (ext == ".py" || ext == ".pyw")
    {
        return "filetype-python";
    }
    if (ext == ".js" || ext == ".mjs" || ext == ".cjs")
    {
        return "filetype-javascript";
    }
    if (ext == ".ts" || ext == ".mts" || ext == ".cts")
    {
        return "filetype-typescript";
    }
    if (ext == ".json")
    {
        return "filetype-json";
    }
    if (ext == ".yaml" || ext == ".yml")
    {
        return "filetype-yaml";
    }
    if (ext == ".html" || ext == ".htm")
    {
        return "filetype-html";
    }
    if (ext == ".css")
    {
        return "filetype-css";
    }
    if (ext == ".rs")
    {
        return "filetype-rust";
    }
    if (ext == ".go")
    {
        return "filetype-go";
    }
    if (ext == ".java")
    {
        return "filetype-java";
    }
    if (ext == ".rb")
    {
        return "filetype-ruby";
    }
    if (ext == ".sh" || ext == ".bash" || ext == ".zsh")
    {
        return "filetype-shell";
    }
    if (ext == ".toml")
    {
        return "filetype-toml";
    }
    if (ext == ".xml" || ext == ".svg")
    {
        return "filetype-xml";
    }

    return "filetype-default";
}

auto FileTypeIconResolver::GetFolderIcon(bool is_open) -> std::string
{
    if (is_open)
    {
        return "filetype-folder-open";
    }
    return "filetype-folder";
}

/// V16 Phase 05: Resolve folder icon by name using manifest.
auto FileTypeIconResolver::GetFolderIconByName(const std::string& folder_name, bool is_open)
    -> std::string
{
    const auto& manifest = get_manifest();
    if (manifest.icon_count() > 0)
    {
        auto folder_entry = manifest.resolve_folder_icon(folder_name);
        if (folder_entry.has_value())
        {
            // Return the canonical folder icon ID for use with IconPipeline
            return folder_entry->canonical_id + (is_open ? "_open" : "_closed");
        }
    }

    // Fallback to generic folder icons
    return is_open ? "filetype-folder-open" : "filetype-folder";
}

} // namespace markamp::ui
