/// SafePath.cpp — V7 Phase 08: Filesystem canonicalization wrapper

#include "SafePath.h"

#include <algorithm>

namespace markamp::core
{

auto SafePath::resolve(const std::filesystem::path& base, std::string_view relative)
    -> Result<std::filesystem::path>
{
    // Reject null bytes
    if (relative.find('\0') != std::string_view::npos)
    {
        return std::unexpected(
            make_error(ErrorCode::NullByte, SubsystemId::Workspace, "Path contains null byte"));
    }

    const std::filesystem::path rel_path(relative);

    // Reject absolute relative paths
    if (rel_path.is_absolute())
    {
        return std::unexpected(make_error(ErrorCode::DirectoryTraversal,
                                          SubsystemId::Workspace,
                                          "Relative path is absolute: " + std::string(relative)));
    }

    // Reject ".." components
    for (const auto& component : rel_path)
    {
        if (component == "..")
        {
            return std::unexpected(
                make_error(ErrorCode::DirectoryTraversal,
                           SubsystemId::Workspace,
                           "Path contains traversal component: " + std::string(relative)));
        }
    }

    auto combined = base / rel_path;

    // Normalize the path
    std::error_code err_code;
    auto canonical_base = std::filesystem::weakly_canonical(base, err_code);
    if (err_code)
    {
        return std::unexpected(make_io_error(
            "Failed to canonicalize base path: " + err_code.message(), ErrorCode::IoError));
    }

    auto canonical_result = std::filesystem::weakly_canonical(combined, err_code);
    if (err_code)
    {
        return std::unexpected(make_io_error(
            "Failed to canonicalize result path: " + err_code.message(), ErrorCode::IoError));
    }

    // Verify the result is within the base
    auto base_str = canonical_base.string();
    auto result_str = canonical_result.string();

    if (result_str.substr(0, base_str.size()) != base_str)
    {
        return std::unexpected(make_error(ErrorCode::DirectoryTraversal,
                                          SubsystemId::Workspace,
                                          "Resolved path escapes base directory"));
    }

    return canonical_result;
}

auto SafePath::resolve_with_symlink_check(const std::filesystem::path& base,
                                          std::string_view relative)
    -> Result<std::filesystem::path>
{
    auto result = resolve(base, relative);
    if (!result.has_value())
    {
        return result;
    }

    // Check if the resolved path involves symlinks that escape the base
    std::error_code err_code;
    auto real_path = std::filesystem::canonical(*result, err_code);
    if (!err_code)
    {
        auto canonical_base = std::filesystem::canonical(base, err_code);
        if (!err_code)
        {
            auto base_str = canonical_base.string();
            auto real_str = real_path.string();
            if (real_str.substr(0, base_str.size()) != base_str)
            {
                return std::unexpected(make_error(ErrorCode::SymlinkEscape,
                                                  SubsystemId::Workspace,
                                                  "Symlink resolves outside base directory"));
            }
        }
    }
    // If paths don't exist yet, symlink check is not applicable — allow

    return result;
}

auto SafePath::join(const std::filesystem::path& base, std::string_view child)
    -> Result<std::filesystem::path>
{
    return resolve(base, child);
}

auto SafePath::is_within(const std::filesystem::path& path, const std::filesystem::path& root)
    -> bool
{
    std::error_code err_code;
    auto canonical_path = std::filesystem::weakly_canonical(path, err_code);
    if (err_code)
    {
        return false;
    }
    auto canonical_root = std::filesystem::weakly_canonical(root, err_code);
    if (err_code)
    {
        return false;
    }

    auto root_str = canonical_root.string();
    auto path_str = canonical_path.string();

    return path_str.substr(0, root_str.size()) == root_str;
}

} // namespace markamp::core
