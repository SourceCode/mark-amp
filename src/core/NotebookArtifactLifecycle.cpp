/// @file NotebookArtifactLifecycle.cpp
/// @brief V20 P03-T01/T03: Notebook artifact lifecycle implementation.

#include "NotebookArtifactLifecycle.h"

#include "ArtifactNamingPolicy.h"
#include "Config.h"
#include "Events.h"
#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

const std::vector<NotebookCellRecord> NotebookArtifactLifecycle::kEmptyCells{};

NotebookArtifactLifecycle::NotebookArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
                                                       ArtifactCreationService& creation_service,
                                                       UnsavedDocumentBufferManager& buffer_manager,
                                                       Config& config)
    : event_bus_(bus)
    , registry_(registry)
    , creation_service_(creation_service)
    , buffer_manager_(buffer_manager)
    , config_(config)
{
}

// ============================================================================
// Creation
// ============================================================================

auto NotebookArtifactLifecycle::create_notebook(const NotebookPlacementContext& context,
                                                  const std::string& title,
                                                  const std::string& kernel_language)
    -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kNotebook;
    req.display_name = title;
    req.language_id = kernel_language;
    req.source = context.entry_point;
    req.target_directory = context.target_directory;

    auto creation_result = creation_service_.create(req);
    if (!creation_result.ok())
    {
        result.error_message = creation_result.error_message;
        MARKAMP_LOG_WARN("Failed to create notebook: {}", result.error_message);
        return result;
    }

    // Initialize cell storage with a default starter cell
    std::vector<NotebookCellRecord> cells;
    NotebookCellRecord starter;
    starter.cell_id = "cell-" + std::to_string(next_cell_id_++);
    starter.kind = NotebookCellKind::kMarkdown;
    starter.source = "# " + (title.empty() ? "New Notebook" : title);
    starter.language_id = "markdown";
    cells.push_back(std::move(starter));

    NotebookCellRecord code_cell;
    code_cell.cell_id = "cell-" + std::to_string(next_cell_id_++);
    code_cell.kind = NotebookCellKind::kCode;
    code_cell.language_id = kernel_language;
    cells.push_back(std::move(code_cell));

    notebook_cells_[creation_result.id.value] = std::move(cells);

    registry_.set_active_artifact(creation_result.id);

    result.success = true;
    result.artifact_id = creation_result.id;

    MARKAMP_LOG_INFO("Notebook created: {} (source='{}', kernel='{}')", creation_result.id.value,
                     context.entry_point, kernel_language);
    return result;
}

// ============================================================================
// Open
// ============================================================================

auto NotebookArtifactLifecycle::open_notebook(const std::string& file_path)
    -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    if (file_path.empty())
    {
        result.error_message = "File path is empty";
        return result;
    }

    // Check if already registered
    const auto* existing = registry_.find_by_path(file_path);
    if (existing != nullptr)
    {
        registry_.set_active_artifact(existing->id);
        result.success = true;
        result.artifact_id = existing->id;
        result.resolved_path = file_path;
        return result;
    }

    ArtifactRecord record;
    record.id = ArtifactRegistry::generate_id();
    record.kind = ArtifactKind::kNotebook;
    record.state = ArtifactLifecycleState::kSaved;
    record.display_name = fs::path(file_path).stem().string();
    record.file_path = file_path;
    record.language_id = "python";
    record.source = "open-notebook";

    auto artifact_id = registry_.register_artifact(std::move(record));
    registry_.set_active_artifact(artifact_id);

    // Initialize empty cell storage (would be populated from file on read)
    notebook_cells_[artifact_id.value] = {};

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = file_path;

    MARKAMP_LOG_INFO("Notebook opened: {} -> {}", artifact_id.value, file_path);
    return result;
}

auto NotebookArtifactLifecycle::restore_notebook(const std::string& file_path,
                                                   const std::string& /*artifact_id_hint*/)
    -> NotebookOperationResult
{
    ++operation_count_;
    return open_notebook(file_path);
}

// ============================================================================
// Save
// ============================================================================

auto NotebookArtifactLifecycle::save(const ArtifactId& artifact_id) -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Notebook not found: " + artifact_id.value;
        return result;
    }

    if (record->is_unsaved() && !record->has_path())
    {
        events::ArtifactSaveRequestEvent evt;
        evt.artifact_id = artifact_id.value;
        event_bus_.publish(evt);

        result.success = true;
        result.artifact_id = artifact_id;
        return result;
    }

    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaved);

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = record->file_path;

    MARKAMP_LOG_INFO("Notebook saved: {}", artifact_id.value);
    return result;
}

auto NotebookArtifactLifecycle::save_as(const ArtifactId& artifact_id,
                                          const std::string& new_path) -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    if (new_path.empty())
    {
        result.error_message = "Save path is empty";
        return result;
    }

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Notebook not found: " + artifact_id.value;
        return result;
    }

    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);

    if (record->is_unsaved() || !record->has_path())
    {
        (void)registry_.promote_to_saved(artifact_id, new_path);
    }
    else
    {
        ArtifactRecord updated = *record;
        updated.file_path = new_path;
        updated.display_name = fs::path(new_path).stem().string();
        updated.state = ArtifactLifecycleState::kSaved;
        (void)registry_.update(artifact_id, std::move(updated));
    }

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = new_path;

    MARKAMP_LOG_INFO("Notebook saved as: {} -> {}", artifact_id.value, new_path);
    return result;
}

// ============================================================================
// Rename / Duplicate / Delete
// ============================================================================

auto NotebookArtifactLifecycle::rename(const ArtifactId& artifact_id,
                                         const std::string& new_title) -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    if (new_title.empty())
    {
        result.error_message = "New title is empty";
        return result;
    }

    if (!registry_.rename(artifact_id, new_title))
    {
        result.error_message = "Failed to rename notebook: " + artifact_id.value;
        return result;
    }

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Notebook renamed: {} -> {}", artifact_id.value, new_title);
    return result;
}

auto NotebookArtifactLifecycle::duplicate(const ArtifactId& artifact_id)
    -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    const auto* original = registry_.find(artifact_id);
    if (original == nullptr)
    {
        result.error_message = "Notebook not found: " + artifact_id.value;
        return result;
    }

    NotebookPlacementContext ctx;
    ctx.entry_point = "duplicate";
    if (original->has_path())
    {
        ctx.target_directory = fs::path(original->file_path.value()).parent_path().string();
    }

    auto dup_result = create_notebook(ctx, original->display_name + " (copy)",
                                        original->language_id);

    if (dup_result.ok())
    {
        // Copy cells from original
        auto iter = notebook_cells_.find(artifact_id.value);
        if (iter != notebook_cells_.end())
        {
            notebook_cells_[dup_result.artifact_id.value] = iter->second;
        }
    }

    return dup_result;
}

auto NotebookArtifactLifecycle::remove(const ArtifactId& artifact_id, bool /*delete_from_disk*/)
    -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    notebook_cells_.erase(artifact_id.value);

    if (!registry_.unregister(artifact_id))
    {
        result.error_message = "Notebook not found: " + artifact_id.value;
        return result;
    }

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Notebook removed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Close
// ============================================================================

auto NotebookArtifactLifecycle::close(const ArtifactId& artifact_id) -> NotebookOperationResult
{
    ++operation_count_;
    NotebookOperationResult result;

    if (registry_.find(artifact_id) == nullptr)
    {
        result.error_message = "Notebook not found: " + artifact_id.value;
        return result;
    }

    notebook_cells_.erase(artifact_id.value);
    (void)registry_.unregister(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Notebook closed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Cell management
// ============================================================================

auto NotebookArtifactLifecycle::add_cell(const ArtifactId& artifact_id, NotebookCellKind kind,
                                           const std::string& source) -> bool
{
    if (registry_.find(artifact_id) == nullptr)
    {
        return false;
    }

    NotebookCellRecord cell;
    cell.cell_id = "cell-" + std::to_string(next_cell_id_++);
    cell.kind = kind;
    cell.source = source;
    cell.language_id = (kind == NotebookCellKind::kCode) ? "python" : "markdown";

    notebook_cells_[artifact_id.value].push_back(std::move(cell));
    return true;
}

auto NotebookArtifactLifecycle::cell_count(const ArtifactId& artifact_id) const -> int
{
    auto iter = notebook_cells_.find(artifact_id.value);
    if (iter == notebook_cells_.end())
    {
        return 0;
    }
    return static_cast<int>(iter->second.size());
}

auto NotebookArtifactLifecycle::get_cells(const ArtifactId& artifact_id) const
    -> const std::vector<NotebookCellRecord>&
{
    auto iter = notebook_cells_.find(artifact_id.value);
    if (iter == notebook_cells_.end())
    {
        return kEmptyCells;
    }
    return iter->second;
}

// ============================================================================
// Queries
// ============================================================================

auto NotebookArtifactLifecycle::open_notebook_count() const -> int
{
    return registry_.count_by_kind(ArtifactKind::kNotebook);
}

auto NotebookArtifactLifecycle::unsaved_notebook_count() const -> int
{
    int count = 0;
    for (const auto& record : registry_.all_artifacts())
    {
        if (record.kind == ArtifactKind::kNotebook && record.is_unsaved())
        {
            ++count;
        }
    }
    return count;
}

auto NotebookArtifactLifecycle::dirty_notebook_count() const -> int
{
    int count = 0;
    for (const auto& record : registry_.all_artifacts())
    {
        if (record.kind == ArtifactKind::kNotebook && record.is_dirty())
        {
            ++count;
        }
    }
    return count;
}

auto NotebookArtifactLifecycle::has_unsaved_changes(const ArtifactId& artifact_id) const -> bool
{
    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        return false;
    }
    return record->is_unsaved() || record->is_dirty();
}

} // namespace markamp::core
