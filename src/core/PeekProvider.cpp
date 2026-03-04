// ============================================================================
// File: src/core/PeekProvider.cpp
// Phase 47: Peek View System — Provider implementations
// ============================================================================
#include "PeekProvider.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// MarkdownPeekProvider
// ============================================================================

void MarkdownPeekProvider::register_file(const std::string& path, const std::string& title)
{
    file_titles_[title] = path;
}

void MarkdownPeekProvider::add_backlink(const std::string& target, const PeekLocation& source)
{
    backlinks_[target].push_back(source);
}

auto MarkdownPeekProvider::find_definitions(const std::string& /*file_path*/,
                                            int /*line*/,
                                            int /*column*/) const -> std::vector<PeekLocation>
{
    // In markdown, "definition" resolves wiki-link targets to their files.
    // For the model layer, we return all known files.
    std::vector<PeekLocation> results;
    for (const auto& [title, path] : file_titles_)
    {
        results.push_back({.file_path = path,
                           .line = 0,
                           .column = 0,
                           .preview_text = title,
                           .symbol_name = title});
    }
    return results;
}

auto MarkdownPeekProvider::find_references(const std::string& file_path,
                                           int /*line*/,
                                           int /*column*/) const -> std::vector<PeekLocation>
{
    auto it = backlinks_.find(file_path);
    if (it != backlinks_.end())
    {
        return it->second;
    }
    return {};
}

auto MarkdownPeekProvider::find_implementations(const std::string& /*file_path*/,
                                                int /*line*/,
                                                int /*column*/) const -> std::vector<PeekLocation>
{
    return {}; // No implementations concept for markdown.
}

auto MarkdownPeekProvider::call_hierarchy(const std::string& /*file_path*/,
                                          int /*line*/,
                                          int /*column*/) const -> PeekHierarchyNode
{
    return {.name = "(no hierarchy)"};
}

auto MarkdownPeekProvider::type_hierarchy(const std::string& /*file_path*/,
                                          int /*line*/,
                                          int /*column*/) const -> PeekHierarchyNode
{
    return {.name = "(no hierarchy)"};
}

auto MarkdownPeekProvider::supported_extensions() const -> std::vector<std::string>
{
    return {".md", ".markdown", ".mdx"};
}

// ============================================================================
// CppPeekProvider
// ============================================================================

void CppPeekProvider::register_symbol(const std::string& symbol_name, const PeekLocation& location)
{
    symbols_[symbol_name].push_back(location);
}

auto CppPeekProvider::find_definitions(const std::string& /*file_path*/,
                                       int /*line*/,
                                       int /*column*/) const -> std::vector<PeekLocation>
{
    // Model-layer stub: returns all registered symbols.
    std::vector<PeekLocation> results;
    for (const auto& [name, locations] : symbols_)
    {
        for (const auto& loc : locations)
        {
            results.push_back(loc);
        }
    }
    return results;
}

auto CppPeekProvider::find_references(const std::string& /*file_path*/,
                                      int /*line*/,
                                      int /*column*/) const -> std::vector<PeekLocation>
{
    return {};
}

auto CppPeekProvider::find_implementations(const std::string& /*file_path*/,
                                           int /*line*/,
                                           int /*column*/) const -> std::vector<PeekLocation>
{
    return {};
}

auto CppPeekProvider::call_hierarchy(const std::string& /*file_path*/,
                                     int /*line*/,
                                     int /*column*/) const -> PeekHierarchyNode
{
    return {.name = "(no hierarchy)"};
}

auto CppPeekProvider::type_hierarchy(const std::string& /*file_path*/,
                                     int /*line*/,
                                     int /*column*/) const -> PeekHierarchyNode
{
    return {.name = "(no hierarchy)"};
}

auto CppPeekProvider::supported_extensions() const -> std::vector<std::string>
{
    return {".cpp", ".h", ".hpp", ".cc", ".cxx"};
}

// ============================================================================
// PeekProviderRegistry
// ============================================================================

void PeekProviderRegistry::register_provider(std::shared_ptr<IPeekProvider> provider)
{
    auto extensions = provider->supported_extensions();
    for (const auto& ext : extensions)
    {
        extension_map_[ext] = provider;
    }
    providers_.push_back(std::move(provider));
}

auto PeekProviderRegistry::provider_for(const std::string& extension) const
    -> std::shared_ptr<IPeekProvider>
{
    auto it = extension_map_.find(extension);
    if (it != extension_map_.end())
    {
        return it->second;
    }
    return nullptr;
}

} // namespace markamp::core
