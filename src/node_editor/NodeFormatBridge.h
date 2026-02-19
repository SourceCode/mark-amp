#pragma once

// V11 Phase 46: Import Export Interchange And Format Bridges
// Manages import/export adapters for graph interchange formats.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed adapter identifier.
struct AdapterId
{
    uint64_t value{0};

    constexpr AdapterId() = default;
    constexpr explicit AdapterId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const AdapterId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const AdapterId&) const noexcept = default;
};

/// A format adapter describes import/export support for a file format.
struct FormatAdapter
{
    AdapterId adapter_id;
    std::string format_name;
    std::vector<std::string> file_extensions;
    bool supports_import{true};
    bool supports_export{true};
    std::string version{"1.0"};
};

/// Result of an import or export operation.
struct FormatBridgeResult
{
    bool success{false};
    std::string data;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

/// Manages import/export format bridges.
class NodeFormatBridge
{
public:
    NodeFormatBridge();

    // --- Adapter registration ---
    auto register_adapter(FormatAdapter adapter) -> AdapterId;
    auto unregister_adapter(AdapterId adapter_id) -> bool;
    [[nodiscard]] auto find_adapter(AdapterId adapter_id) const -> const FormatAdapter*;
    [[nodiscard]] auto adapters_for_extension(const std::string& ext) const
        -> std::vector<AdapterId>;
    [[nodiscard]] auto adapter_count() const -> std::size_t;

    // --- Export ---
    [[nodiscard]] auto export_graph(const NodeGraph& graph, AdapterId adapter_id) const
        -> FormatBridgeResult;

    // --- Import ---
    [[nodiscard]] auto import_graph(const std::string& data, AdapterId adapter_id) const
        -> FormatBridgeResult;

    // --- Format queries ---
    [[nodiscard]] auto supported_import_extensions() const -> std::vector<std::string>;
    [[nodiscard]] auto supported_export_extensions() const -> std::vector<std::string>;

    // --- Bulk ---
    void clear();

private:
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, FormatAdapter> adapters_;

    void register_builtin_adapters();
};

} // namespace markamp::node_editor
