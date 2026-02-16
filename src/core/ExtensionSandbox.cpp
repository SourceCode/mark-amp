#include "ExtensionSandbox.h"

#include <stdexcept>

namespace markamp::core
{

void ExtensionSandbox::set_permissions(const std::string& extension_id,
                                       const std::vector<ExtensionPermission>& permissions)
{
    auto& perm_set = permissions_[extension_id];
    perm_set.clear();
    for (auto perm : permissions)
    {
        perm_set.insert(perm);
    }
}

auto ExtensionSandbox::has_permission(const std::string& extension_id,
                                      ExtensionPermission permission) const -> bool
{
    auto found = permissions_.find(extension_id);
    if (found == permissions_.end())
    {
        return false;
    }
    return found->second.contains(permission);
}

auto ExtensionSandbox::get_permissions(const std::string& extension_id) const
    -> std::vector<ExtensionPermission>
{
    auto found = permissions_.find(extension_id);
    if (found == permissions_.end())
    {
        return {};
    }
    return {found->second.begin(), found->second.end()};
}

auto ExtensionSandbox::from_string(const std::string& perm_str) -> ExtensionPermission
{
    if (perm_str == "filesystem")
    {
        return ExtensionPermission::kFilesystem;
    }
    if (perm_str == "network")
    {
        return ExtensionPermission::kNetwork;
    }
    if (perm_str == "clipboard")
    {
        return ExtensionPermission::kClipboard;
    }
    if (perm_str == "terminal")
    {
        return ExtensionPermission::kTerminal;
    }
    if (perm_str == "debug")
    {
        return ExtensionPermission::kDebug;
    }
    if (perm_str == "workspaceSettings")
    {
        return ExtensionPermission::kWorkspaceSettings;
    }
    if (perm_str == "processExecution")
    {
        return ExtensionPermission::kProcessExecution;
    }
    // V8 Phase 8: Canvas marketplace scopes
    if (perm_str == "canvas.read")
    {
        return ExtensionPermission::kCanvasRead;
    }
    if (perm_str == "canvas.write")
    {
        return ExtensionPermission::kCanvasWrite;
    }
    if (perm_str == "canvas.selection")
    {
        return ExtensionPermission::kCanvasSelection;
    }
    if (perm_str == "canvas.comments")
    {
        return ExtensionPermission::kCanvasComments;
    }
    if (perm_str == "canvas.templates")
    {
        return ExtensionPermission::kCanvasTemplates;
    }
    if (perm_str == "canvas.network")
    {
        return ExtensionPermission::kCanvasNetwork;
    }
    throw std::invalid_argument("Unknown permission: " + perm_str);
}

auto ExtensionSandbox::to_string(ExtensionPermission perm) -> std::string
{
    switch (perm)
    {
        case ExtensionPermission::kFilesystem:
            return "filesystem";
        case ExtensionPermission::kNetwork:
            return "network";
        case ExtensionPermission::kClipboard:
            return "clipboard";
        case ExtensionPermission::kTerminal:
            return "terminal";
        case ExtensionPermission::kDebug:
            return "debug";
        case ExtensionPermission::kWorkspaceSettings:
            return "workspaceSettings";
        case ExtensionPermission::kProcessExecution:
            return "processExecution";
        // V8 Phase 8: Canvas marketplace scopes
        case ExtensionPermission::kCanvasRead:
            return "canvas.read";
        case ExtensionPermission::kCanvasWrite:
            return "canvas.write";
        case ExtensionPermission::kCanvasSelection:
            return "canvas.selection";
        case ExtensionPermission::kCanvasComments:
            return "canvas.comments";
        case ExtensionPermission::kCanvasTemplates:
            return "canvas.templates";
        case ExtensionPermission::kCanvasNetwork:
            return "canvas.network";
    }
    return "unknown";
}

auto ExtensionSandbox::is_registered(const std::string& extension_id) const -> bool
{
    return permissions_.contains(extension_id);
}

void ExtensionSandbox::remove_extension(const std::string& extension_id)
{
    permissions_.erase(extension_id);
}

} // namespace markamp::core
