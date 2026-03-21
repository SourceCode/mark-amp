/// @file CanvasArtifactLifecycle.cpp
/// @brief V20 P04-T01/T03: Canvas artifact lifecycle implementation.

#include "CanvasArtifactLifecycle.h"

#include "ArtifactNamingPolicy.h"
#include "Config.h"
#include "Events.h"
#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

CanvasArtifactLifecycle::CanvasArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
                                                   ArtifactCreationService& creation_service,
                                                   Config& config)
    : event_bus_(bus)
    , registry_(registry)
    , creation_service_(creation_service)
    , config_(config)
{
}

// ============================================================================
// Creation
// ============================================================================

auto CanvasArtifactLifecycle::create_board(const CanvasPlacementContext& context,
                                             const std::string& name) -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kCanvas;
    req.display_name = name;
    req.language_id = "canvas";
    req.source = context.entry_point;
    req.target_directory = context.target_directory;

    auto creation_result = creation_service_.create(req);
    if (!creation_result.ok())
    {
        result.error_message = creation_result.error_message;
        MARKAMP_LOG_WARN("Failed to create board: {}", result.error_message);
        return result;
    }

    registry_.set_active_artifact(creation_result.id);

    result.success = true;
    result.artifact_id = creation_result.id;
    result.board_id = creation_result.id.value;

    MARKAMP_LOG_INFO("Board created: {} (source='{}')", creation_result.id.value,
                     context.entry_point);
    return result;
}

auto CanvasArtifactLifecycle::create_from_template(const CanvasPlacementContext& context,
                                                     const std::string& template_id,
                                                     const std::string& name)
    -> CanvasOperationResult
{
    ++operation_count_;

    // Create the base board
    auto result = create_board(context, name);
    if (!result.ok())
    {
        return result;
    }

    // Template application is delegated to the workbench layer
    MARKAMP_LOG_INFO("Board created from template '{}': {}", template_id,
                     result.artifact_id.value);
    return result;
}

// ============================================================================
// Open
// ============================================================================

auto CanvasArtifactLifecycle::open_board(const std::string& file_path) -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

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
        result.board_id = existing->id.value;
        result.resolved_path = file_path;
        return result;
    }

    ArtifactRecord record;
    record.id = ArtifactRegistry::generate_id();
    record.kind = ArtifactKind::kCanvas;
    record.state = ArtifactLifecycleState::kSaved;
    record.display_name = fs::path(file_path).stem().string();
    record.file_path = file_path;
    record.language_id = "canvas";
    record.source = "open-board";

    auto artifact_id = registry_.register_artifact(std::move(record));
    registry_.set_active_artifact(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;
    result.board_id = artifact_id.value;
    result.resolved_path = file_path;

    MARKAMP_LOG_INFO("Board opened: {} -> {}", artifact_id.value, file_path);
    return result;
}

auto CanvasArtifactLifecycle::restore_board(const std::string& file_path,
                                              const std::string& /*board_id_hint*/)
    -> CanvasOperationResult
{
    ++operation_count_;
    return open_board(file_path);
}

// ============================================================================
// Save
// ============================================================================

auto CanvasArtifactLifecycle::save(const ArtifactId& artifact_id) -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Board not found: " + artifact_id.value;
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
    result.board_id = artifact_id.value;
    result.resolved_path = record->file_path;

    MARKAMP_LOG_INFO("Board saved: {}", artifact_id.value);
    return result;
}

auto CanvasArtifactLifecycle::save_as(const ArtifactId& artifact_id, const std::string& new_path)
    -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    if (new_path.empty())
    {
        result.error_message = "Save path is empty";
        return result;
    }

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Board not found: " + artifact_id.value;
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
    result.board_id = artifact_id.value;
    result.resolved_path = new_path;

    MARKAMP_LOG_INFO("Board saved as: {} -> {}", artifact_id.value, new_path);
    return result;
}

// ============================================================================
// Rename / Duplicate / Delete
// ============================================================================

auto CanvasArtifactLifecycle::rename(const ArtifactId& artifact_id, const std::string& new_name)
    -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    if (new_name.empty())
    {
        result.error_message = "New name is empty";
        return result;
    }

    if (!registry_.rename(artifact_id, new_name))
    {
        result.error_message = "Failed to rename board: " + artifact_id.value;
        return result;
    }

    result.success = true;
    result.artifact_id = artifact_id;
    result.board_id = artifact_id.value;

    MARKAMP_LOG_INFO("Board renamed: {} -> {}", artifact_id.value, new_name);
    return result;
}

auto CanvasArtifactLifecycle::duplicate(const ArtifactId& artifact_id) -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    const auto* original = registry_.find(artifact_id);
    if (original == nullptr)
    {
        result.error_message = "Board not found: " + artifact_id.value;
        return result;
    }

    CanvasPlacementContext ctx;
    ctx.entry_point = "duplicate";
    if (original->has_path())
    {
        ctx.target_directory = fs::path(original->file_path.value()).parent_path().string();
    }

    auto dup_result = create_board(ctx, original->display_name + " (copy)");
    return dup_result;
}

auto CanvasArtifactLifecycle::remove(const ArtifactId& artifact_id, bool /*delete_from_disk*/)
    -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    if (!registry_.unregister(artifact_id))
    {
        result.error_message = "Board not found: " + artifact_id.value;
        return result;
    }

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Board removed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Close
// ============================================================================

auto CanvasArtifactLifecycle::close(const ArtifactId& artifact_id) -> CanvasOperationResult
{
    ++operation_count_;
    CanvasOperationResult result;

    if (registry_.find(artifact_id) == nullptr)
    {
        result.error_message = "Board not found: " + artifact_id.value;
        return result;
    }

    (void)registry_.unregister(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Board closed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Queries
// ============================================================================

auto CanvasArtifactLifecycle::open_board_count() const -> int
{
    return registry_.count_by_kind(ArtifactKind::kCanvas);
}

auto CanvasArtifactLifecycle::unsaved_board_count() const -> int
{
    int count = 0;
    for (const auto& record : registry_.all_artifacts())
    {
        if (record.kind == ArtifactKind::kCanvas && record.is_unsaved())
        {
            ++count;
        }
    }
    return count;
}

auto CanvasArtifactLifecycle::has_unsaved_changes(const ArtifactId& artifact_id) const -> bool
{
    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        return false;
    }
    return record->is_unsaved() || record->is_dirty();
}

} // namespace markamp::core
