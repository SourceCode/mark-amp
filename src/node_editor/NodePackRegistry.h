#pragma once

// V11 Phase 41: Plugin API Contribution Points And Node Packs
// Registry for external node packs contributed by extensions.

#include "NodeEditorTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed pack identifier.
struct PackId
{
    uint64_t value{0};

    constexpr PackId() = default;
    constexpr explicit PackId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const PackId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const PackId&) const noexcept = default;
};

/// Trust level for an extension pack.
enum class TrustLevel : uint8_t
{
    kBuiltIn,
    kTrusted,
    kUntrusted
};

/// Permissions a node pack may require.
enum class PackPermission : uint8_t
{
    kFileAccess,
    kNetworkAccess,
    kProcessExec,
    kClipboard,
    kEnvironment
};

/// Node type entry within a pack.
struct PackNodeEntry
{
    std::string type_name;
    std::string display_name;
    std::string category;
    std::string description;
    std::vector<SocketDataType> default_inputs;
    std::vector<SocketDataType> default_outputs;
};

/// Result from manifest / pack validation.
struct PackValidationResult
{
    bool ok{true};
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

/// External node pack contributed by an extension.
struct ExtensionNodePack
{
    PackId pack_id;
    std::string display_name;
    std::string version;
    std::string vendor;
    TrustLevel trust{TrustLevel::kUntrusted};
    std::vector<PackNodeEntry> node_entries;
    std::vector<PackPermission> required_permissions;
    std::string contribution_category; // e.g. "Audio", "Math"
};

/// Registry managing external node packs.
class NodePackRegistry
{
public:
    NodePackRegistry() = default;

    // --- Registration ---
    auto register_pack(ExtensionNodePack pack) -> PackId;
    auto unregister_pack(PackId pack_id) -> bool;

    // --- Queries ---
    [[nodiscard]] auto find_pack(PackId pack_id) const -> const ExtensionNodePack*;
    [[nodiscard]] auto all_packs() const -> std::vector<PackId>;
    [[nodiscard]] auto pack_count() const -> std::size_t;
    [[nodiscard]] auto nodes_from_pack(PackId pack_id) const -> std::vector<PackNodeEntry>;
    [[nodiscard]] auto is_trusted(PackId pack_id) const -> bool;

    // --- Validation ---
    [[nodiscard]] static auto validate_pack(const ExtensionNodePack& pack) -> PackValidationResult;

    // --- Contribution points ---
    [[nodiscard]] auto contribution_point_count() const -> std::size_t;
    [[nodiscard]] auto has_contribution(const std::string& category) const -> bool;

    // --- Bulk ---
    void clear();

private:
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, ExtensionNodePack> packs_;
};

} // namespace markamp::node_editor
