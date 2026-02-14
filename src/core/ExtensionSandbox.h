#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Known permission/capability types for extensions (#42).
enum class ExtensionPermission
{
    kFilesystem,
    kNetwork,
    kClipboard,
    kTerminal,
    kDebug,
    kWorkspaceSettings,
    kProcessExecution
};

/// Extension sandbox permissions model (#42).
/// Extensions declare capabilities; runtime checks restrict unauthorized API calls.
class ExtensionSandbox
{
public:
    ExtensionSandbox() = default;

    /// Set permissions for an extension (from manifest parsing).
    void set_permissions(const std::string& extension_id,
                         const std::vector<ExtensionPermission>& permissions);

    /// Check if an extension has a specific permission.
    [[nodiscard]] auto has_permission(const std::string& extension_id,
                                      ExtensionPermission permission) const -> bool;

    /// Get all permissions for an extension.
    [[nodiscard]] auto get_permissions(const std::string& extension_id) const
        -> std::vector<ExtensionPermission>;

    /// Convert a permission string to enum.
    static auto from_string(const std::string& perm_str) -> ExtensionPermission;

    /// Convert a permission enum to string.
    static auto to_string(ExtensionPermission perm) -> std::string;

    /// Check if an extension is registered (has any permissions set).
    [[nodiscard]] auto is_registered(const std::string& extension_id) const -> bool;

    /// Remove all permissions for an extension.
    void remove_extension(const std::string& extension_id);

private:
    std::unordered_map<std::string, std::unordered_set<ExtensionPermission>> permissions_;
};

} // namespace markamp::core

// Hash specialization for ExtensionPermission to use in unordered_set/map
template <>
struct std::hash<markamp::core::ExtensionPermission>
{
    auto operator()(markamp::core::ExtensionPermission perm) const noexcept -> std::size_t
    {
        return std::hash<int>{}(static_cast<int>(perm));
    }
};
