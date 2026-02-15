#pragma once

#include "Block.h"
#include "WikiLink.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

/// A unified document model that wraps a Block tree with parsed metadata.
/// This is the primary in-memory representation of an open document.
class DocumentModel
{
public:
    explicit DocumentModel(const std::string& id, const std::string& notebook_id);

    // --- Identity ---
    [[nodiscard]] auto id() const -> const std::string&;
    [[nodiscard]] auto notebook_id() const -> const std::string&;
    [[nodiscard]] auto title() const -> const std::string&;
    [[nodiscard]] auto file_path() const -> const std::filesystem::path&;

    auto set_title(const std::string& title) -> void;
    auto set_file_path(const std::filesystem::path& path) -> void;

    // --- Content ---
    [[nodiscard]] auto markdown() const -> const std::string&;
    auto set_markdown(const std::string& content) -> void;

    /// Re-parse markdown content: extracts frontmatter, wikilinks, tags, block tree
    auto reparse() -> void;

    // --- Frontmatter ---
    [[nodiscard]] auto frontmatter() const -> const Frontmatter&;
    auto set_frontmatter(const Frontmatter& fm) -> void;

    // --- Outgoing Links ---
    [[nodiscard]] auto outgoing_links() const -> const std::vector<WikiLink>&;
    [[nodiscard]] auto outgoing_embeds() const -> std::vector<WikiLink>;

    // --- Tags ---
    [[nodiscard]] auto tags() const -> const std::vector<ParsedTag>&;
    [[nodiscard]] auto all_tag_strings() const -> std::vector<std::string>;

    // --- Block Tree ---
    [[nodiscard]] auto root_block() const -> const std::shared_ptr<Block>&;
    auto set_root_block(std::shared_ptr<Block> root) -> void;

    // --- Aliases ---
    [[nodiscard]] auto aliases() const -> const std::vector<std::string>&;

    // --- Dirty tracking ---
    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto mark_clean() -> void;

private:
    std::string id_;
    std::string notebook_id_;
    std::string title_;
    std::filesystem::path file_path_;
    std::string markdown_;
    Frontmatter frontmatter_;
    std::vector<WikiLink> outgoing_links_;
    std::vector<ParsedTag> tags_;
    std::shared_ptr<Block> root_block_;
    bool dirty_{false};
};

} // namespace markamp::core
