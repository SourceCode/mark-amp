// ============================================================================
// File: src/core/PeekProvider.h
// Phase 47: Peek View System — Provider interface and registry
// ============================================================================
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A location found by a peek provider.
struct PeekLocation
{
    std::string file_path;
    int line{0};
    int column{0};
    std::string preview_text; ///< Context line for display
    std::string symbol_name;  ///< Name of the found symbol
};

/// A hierarchy node (for call/type hierarchies).
struct PeekHierarchyNode
{
    std::string name;
    std::string file_path;
    int line{0};
    std::vector<PeekHierarchyNode> children;

    /// Count all descendants (recursive).
    [[nodiscard]] auto descendant_count() const -> int
    {
        int count = 0;
        for (const auto& child : children)
        {
            count += 1 + child.descendant_count();
        }
        return count;
    }
};

/// Peek command type.
enum class PeekCommand : uint8_t
{
    Definition,
    References,
    Implementation,
    CallHierarchy,
    TypeHierarchy
};

/// Interface for peek providers.
/// Each file type can register a provider that resolves definitions,
/// references, implementations, and hierarchies.
class IPeekProvider
{
public:
    virtual ~IPeekProvider() = default;

    /// Find definitions for the symbol at the given location.
    [[nodiscard]] virtual auto find_definitions(const std::string& file_path,
                                                int line,
                                                int column) const -> std::vector<PeekLocation> = 0;

    /// Find references to the symbol at the given location.
    [[nodiscard]] virtual auto find_references(const std::string& file_path,
                                               int line,
                                               int column) const -> std::vector<PeekLocation> = 0;

    /// Find implementations of the symbol.
    [[nodiscard]] virtual auto
    find_implementations(const std::string& file_path, int line, int column) const
        -> std::vector<PeekLocation> = 0;

    /// Build call hierarchy for the symbol.
    [[nodiscard]] virtual auto call_hierarchy(const std::string& file_path,
                                              int line,
                                              int column) const -> PeekHierarchyNode = 0;

    /// Build type hierarchy for the symbol.
    [[nodiscard]] virtual auto type_hierarchy(const std::string& file_path,
                                              int line,
                                              int column) const -> PeekHierarchyNode = 0;

    /// Which file extensions this provider supports.
    [[nodiscard]] virtual auto supported_extensions() const -> std::vector<std::string> = 0;
};

/// Markdown-specific peek provider (wiki-links, backlinks).
class MarkdownPeekProvider : public IPeekProvider
{
public:
    MarkdownPeekProvider() = default;

    /// Add a known file for resolution.
    void register_file(const std::string& path, const std::string& title);

    /// Add a backlink reference.
    void add_backlink(const std::string& target, const PeekLocation& source);

    [[nodiscard]] auto find_definitions(const std::string& file_path, int line, int column) const
        -> std::vector<PeekLocation> override;

    [[nodiscard]] auto find_references(const std::string& file_path, int line, int column) const
        -> std::vector<PeekLocation> override;

    [[nodiscard]] auto find_implementations(const std::string& file_path,
                                            int line,
                                            int column) const -> std::vector<PeekLocation> override;

    [[nodiscard]] auto call_hierarchy(const std::string& file_path, int line, int column) const
        -> PeekHierarchyNode override;

    [[nodiscard]] auto type_hierarchy(const std::string& file_path, int line, int column) const
        -> PeekHierarchyNode override;

    [[nodiscard]] auto supported_extensions() const -> std::vector<std::string> override;

private:
    std::unordered_map<std::string, std::string> file_titles_;
    std::unordered_map<std::string, std::vector<PeekLocation>> backlinks_;
};

/// C++ specific peek provider stub.
class CppPeekProvider : public IPeekProvider
{
public:
    CppPeekProvider() = default;

    /// Register a symbol definition for lookup.
    void register_symbol(const std::string& symbol_name, const PeekLocation& location);

    [[nodiscard]] auto find_definitions(const std::string& file_path, int line, int column) const
        -> std::vector<PeekLocation> override;

    [[nodiscard]] auto find_references(const std::string& file_path, int line, int column) const
        -> std::vector<PeekLocation> override;

    [[nodiscard]] auto find_implementations(const std::string& file_path,
                                            int line,
                                            int column) const -> std::vector<PeekLocation> override;

    [[nodiscard]] auto call_hierarchy(const std::string& file_path, int line, int column) const
        -> PeekHierarchyNode override;

    [[nodiscard]] auto type_hierarchy(const std::string& file_path, int line, int column) const
        -> PeekHierarchyNode override;

    [[nodiscard]] auto supported_extensions() const -> std::vector<std::string> override;

private:
    std::unordered_map<std::string, std::vector<PeekLocation>> symbols_;
};

/// Registry mapping file types to peek providers.
class PeekProviderRegistry
{
public:
    PeekProviderRegistry() = default;

    /// Register a provider.
    void register_provider(std::shared_ptr<IPeekProvider> provider);

    /// Find the provider for a given file extension.
    [[nodiscard]] auto provider_for(const std::string& extension) const
        -> std::shared_ptr<IPeekProvider>;

    /// Get all registered providers.
    [[nodiscard]] auto providers() const -> const std::vector<std::shared_ptr<IPeekProvider>>&
    {
        return providers_;
    }

private:
    std::vector<std::shared_ptr<IPeekProvider>> providers_;
    std::unordered_map<std::string, std::shared_ptr<IPeekProvider>> extension_map_;
};

} // namespace markamp::core
