#pragma once

#include "core/BlockID.h"
#include "core/Config.h"
#include "core/EventBus.h"

#include <expected>
#include <filesystem>
#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>

namespace markamp::core
{

// Configuration for a single notebook, persisted to .markamp/conf.json
struct NotebookConf
{
    std::string name;
    int32_t sort = 0;
    bool closed = false;

    std::string ref_create_save_path;
    std::string create_doc_name_template;
    std::string daily_note_save_path;
    std::string daily_note_template_path;
};

// Represents a single notebook (top-level container for documents).
struct Notebook
{
    std::string id;
    std::string name;
    std::string icon;
    int32_t sort = 0;
    bool closed = false;

    // Flashcard statistics (populated on demand)
    int32_t new_flashcard_count = 0;
    int32_t due_flashcard_count = 0;
    int32_t flashcard_count = 0;

    // Computed statistics (populated on demand)
    int64_t doc_count = 0;
    int64_t block_count = 0;
    int64_t size_bytes = 0;

    NotebookConf conf;

    [[nodiscard]] auto is_open() const -> bool;
};

// Manages all notebooks: creation, deletion, opening/closing, sorting.
class NotebookManager
{
public:
    explicit NotebookManager(EventBus& event_bus, Config& config);

    // CRUD
    [[nodiscard]] auto create_notebook(const std::string& name, const std::string& save_path = "")
        -> std::expected<std::string, std::string>;

    auto rename_notebook(const std::string& id, const std::string& new_name)
        -> std::expected<void, std::string>;

    auto remove_notebook(const std::string& id) -> std::expected<void, std::string>;

    auto set_notebook_icon(const std::string& id, const std::string& icon) -> void;

    // Open/Close
    auto open_notebook(const std::string& id) -> std::expected<void, std::string>;
    auto close_notebook(const std::string& id) -> std::expected<void, std::string>;

    // Query
    [[nodiscard]] auto list_notebooks() const -> std::vector<Notebook>;
    [[nodiscard]] auto get_notebook(const std::string& id) const -> std::optional<Notebook>;
    [[nodiscard]] auto get_open_notebooks() const -> std::vector<Notebook>;

    // Sorting
    auto sort_notebooks(const std::vector<std::string>& ordered_ids) -> void;

    // Configuration
    [[nodiscard]] auto get_notebook_conf(const std::string& id) const
        -> std::optional<NotebookConf>;
    auto set_notebook_conf(const std::string& id, const NotebookConf& conf) -> void;

    // Initialization
    auto load_notebooks() -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    std::vector<Notebook> notebooks_;
    mutable std::shared_mutex mutex_;

    auto load_notebooks_from_disk(const std::filesystem::path& data_dir) -> void;
    auto save_notebook_conf(const std::string& id) -> void;

    [[nodiscard]] auto notebook_data_path(const std::string& id) const -> std::filesystem::path;
    [[nodiscard]] auto notebook_conf_path(const std::string& id) const -> std::filesystem::path;
    [[nodiscard]] auto data_dir() const -> std::filesystem::path;

    [[nodiscard]] auto find_notebook_unlocked(const std::string& id)
        -> std::vector<Notebook>::iterator;
    [[nodiscard]] auto find_notebook_unlocked(const std::string& id) const
        -> std::vector<Notebook>::const_iterator;
};

} // namespace markamp::core
