/// @file BacklinkFinder.h
/// @brief V13 Phase 32 Task 10 — Finds all references to a document/heading.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// A reference from one document to another.
struct Backlink
{
    std::string source_file; ///< File containing the reference
    int source_line{0};      ///< Line number of the reference
    std::string link_text;   ///< Raw link text as it appears in the source
    std::string link_type;   ///< "wiki", "markdown", or "block"
};

/// Finds all references (backlinks) to a document or heading across workspace.
///
/// Scans workspace files for wiki-links, markdown links, and block references
/// pointing to a target document.
class BacklinkFinder
{
public:
    /// Construct with workspace root.
    explicit BacklinkFinder(std::string workspace_root);

    /// Find all backlinks to a specific document.
    /// @param target_name   Document name (without extension) or path.
    /// @param workspace_files  All workspace file paths to scan.
    [[nodiscard]] auto find_backlinks(const std::string& target_name,
                                      const std::vector<std::string>& workspace_files) const
        -> std::vector<Backlink>;

    /// Find references to a specific heading within a document.
    /// @param target_name   Document name.
    /// @param heading       Heading text to search for.
    /// @param workspace_files  All workspace file paths to scan.
    [[nodiscard]] auto
    find_heading_references(const std::string& target_name,
                            const std::string& heading,
                            const std::vector<std::string>& workspace_files) const
        -> std::vector<Backlink>;

    /// Get workspace root.
    [[nodiscard]] auto workspace_root() const -> const std::string&;

    /// (#176) Return the total number of backlinks for a target.
    [[nodiscard]] auto backlink_count(const std::string& target_name,
                                      const std::vector<std::string>& workspace_files) const
        -> std::size_t;

    /// (#177) Check if any references exist for a target.
    [[nodiscard]] auto has_references(const std::string& target_name,
                                      const std::vector<std::string>& workspace_files) const
        -> bool;

private:
    std::string workspace_root_;

    /// Scan a single file for references to the target.
    [[nodiscard]] auto scan_file(const std::string& file_path,
                                 const std::string& target_name,
                                 const std::string& heading = "") const -> std::vector<Backlink>;
};

} // namespace markamp::core
