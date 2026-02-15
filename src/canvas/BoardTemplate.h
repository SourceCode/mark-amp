#pragma once

#include "canvas/BoardSerializer.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;

/// Metadata for a board template.
struct TemplateInfo
{
    std::string id;
    std::string name;
    std::string description;
    std::string category; // "Agile", "Design", "Strategy", "Education", "Custom"
    std::string author;
    std::string thumbnail_path;
    std::filesystem::path template_path;
    bool is_builtin{false};
};

/// Manages a library of board templates (built-in and user-created).
class BoardTemplateLibrary
{
public:
    /// Load built-in templates from the resources directory.
    auto load_builtins(const std::filesystem::path& resources_dir) -> void;

    /// Load user-created templates from the config directory.
    auto load_user_templates(const std::filesystem::path& user_dir) -> void;

    /// Get all templates in a category.
    [[nodiscard]] auto templates_in_category(const std::string& category) const
        -> std::vector<const TemplateInfo*>;

    /// Get all category names.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Search templates by name (case-insensitive substring match).
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<const TemplateInfo*>;

    /// Apply a template: returns a new Board created from the template.
    [[nodiscard]] auto apply_template(const std::string& template_id) const
        -> std::unique_ptr<Board>;

    /// Save a board as a user template.
    auto save_as_template(const Board& board,
                          const TemplateInfo& info,
                          const std::filesystem::path& user_dir) -> bool;

    /// Get all templates.
    [[nodiscard]] auto all_templates() const -> const std::vector<TemplateInfo>&;

    /// Total number of templates.
    [[nodiscard]] auto template_count() const -> size_t;

private:
    std::vector<TemplateInfo> templates_;

    void load_from_directory(const std::filesystem::path& dir, bool builtin);
};

} // namespace markamp::canvas
