/// @file TextArtifactLifecycle.cpp
/// @brief V20 P02-T03: Text artifact lifecycle implementation.

#include "TextArtifactLifecycle.h"

#include "ArtifactNamingPolicy.h"
#include "Config.h"
#include "Events.h"
#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

TextArtifactLifecycle::TextArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
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

auto TextArtifactLifecycle::create_new_file(const FilePlacementContext& context,
                                              const std::string& display_name,
                                              const std::string& language_id)
    -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    // Create the artifact through the unified creation service
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = display_name;
    req.language_id = language_id;
    req.source = context.entry_point;
    req.target_directory = context.target_directory;

    auto creation_result = creation_service_.create(req);
    if (!creation_result.ok())
    {
        result.error_message = creation_result.error_message;
        MARKAMP_LOG_WARN("Failed to create text file: {}", result.error_message);
        return result;
    }

    // Create an unsaved buffer for the new document
    (void)buffer_manager_.create_buffer(creation_result.id, language_id);

    // Set as active artifact
    registry_.set_active_artifact(creation_result.id);

    result.success = true;
    result.artifact_id = creation_result.id;

    MARKAMP_LOG_INFO("New text file created: {} (source='{}')", creation_result.id.value,
                     context.entry_point);
    return result;
}

// ============================================================================
// Open
// ============================================================================

auto TextArtifactLifecycle::open_file(const std::string& file_path) -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    if (file_path.empty())
    {
        result.error_message = "File path is empty";
        return result;
    }

    // Check if already registered
    const auto* existing = registry_.find_by_path(file_path);
    if (existing != nullptr)
    {
        // Already open — just activate it
        registry_.set_active_artifact(existing->id);
        result.success = true;
        result.artifact_id = existing->id;
        result.resolved_path = file_path;
        return result;
    }

    // Register as a saved artifact (it already exists on disk)
    ArtifactRecord record;
    record.id = ArtifactRegistry::generate_id();
    record.kind = ArtifactKind::kTextFile;
    record.state = ArtifactLifecycleState::kSaved;
    record.display_name = fs::path(file_path).filename().string();
    record.file_path = file_path;
    record.source = "open-file";

    // Detect language from extension
    auto ext = fs::path(file_path).extension().string();
    if (!ext.empty() && ext[0] == '.')
    {
        ext = ext.substr(1);
    }
    record.language_id = ext.empty() ? "plaintext" : ext;

    auto artifact_id = registry_.register_artifact(std::move(record));
    registry_.set_active_artifact(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = file_path;

    MARKAMP_LOG_INFO("File opened: {} -> {}", artifact_id.value, file_path);
    return result;
}

auto TextArtifactLifecycle::reopen_file(const std::string& file_path) -> LifecycleOperationResult
{
    // Reopen delegates to open_file — same logic, just a different entry point
    ++operation_count_;
    auto result = open_file(file_path);
    if (result.ok())
    {
        // Update source to indicate reopen
        auto* record_ptr = const_cast<ArtifactRecord*>(registry_.find(result.artifact_id));
        if (record_ptr != nullptr)
        {
            record_ptr->source = "reopen";
        }
    }
    return result;
}

// ============================================================================
// Save
// ============================================================================

auto TextArtifactLifecycle::save(const ArtifactId& artifact_id) -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    if (record->is_unsaved() && !record->has_path())
    {
        // First save — needs a path. Publish save request event for UI to show dialog.
        events::ArtifactSaveRequestEvent evt;
        evt.artifact_id = artifact_id.value;
        event_bus_.publish(evt);

        // The actual save will be handled by save_as when the UI provides a path
        result.success = true;
        result.artifact_id = artifact_id;
        return result;
    }

    // Already has a path — save to existing location
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);

    // Publish save request for the persistence layer
    events::TabSaveRequestEvent save_evt{record->file_path.value_or("")};
    event_bus_.publish(save_evt);

    // Mark as saved
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaved);

    // Clean the unsaved buffer if any
    (void)buffer_manager_.mark_clean(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = record->file_path;

    MARKAMP_LOG_INFO("Artifact saved: {}", artifact_id.value);
    return result;
}

auto TextArtifactLifecycle::save_as(const ArtifactId& artifact_id, const std::string& new_path)
    -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    if (new_path.empty())
    {
        result.error_message = "Save path is empty";
        return result;
    }

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    // Set saving state
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);

    // If unsaved, promote to saved with the new path
    if (record->is_unsaved() || !record->has_path())
    {
        (void)registry_.promote_to_saved(artifact_id, new_path);
    }
    else
    {
        // Already saved — update path (Save As to new location)
        ArtifactRecord updated = *record;
        updated.file_path = new_path;
        updated.display_name = fs::path(new_path).filename().string();
        updated.state = ArtifactLifecycleState::kSaved;
        (void)registry_.update(artifact_id, std::move(updated));
    }

    // Publish save event
    events::TabSaveRequestEvent save_evt{new_path};
    event_bus_.publish(save_evt);

    // Clean buffer
    (void)buffer_manager_.mark_clean(artifact_id);

    // Remove unsaved buffer after first save
    if (buffer_manager_.has_buffer(artifact_id))
    {
        (void)buffer_manager_.remove_buffer(artifact_id);
    }

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = new_path;

    MARKAMP_LOG_INFO("Artifact saved as: {} -> {}", artifact_id.value, new_path);
    return result;
}

// ============================================================================
// Rename / Move
// ============================================================================

auto TextArtifactLifecycle::rename(const ArtifactId& artifact_id, const std::string& new_name)
    -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    if (new_name.empty())
    {
        result.error_message = "New name is empty";
        return result;
    }

    if (!registry_.rename(artifact_id, new_name))
    {
        result.error_message = "Failed to rename artifact: " + artifact_id.value;
        return result;
    }

    // If the artifact has a path, update the file_path too
    const auto* record = registry_.find(artifact_id);
    if (record != nullptr && record->has_path())
    {
        auto old_path = fs::path(record->file_path.value());
        auto new_path = old_path.parent_path() / new_name;

        ArtifactRecord updated = *record;
        updated.file_path = new_path.string();
        (void)registry_.update(artifact_id, std::move(updated));

        result.resolved_path = new_path.string();
    }

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Artifact renamed: {} -> {}", artifact_id.value, new_name);
    return result;
}

auto TextArtifactLifecycle::move_to(const ArtifactId& artifact_id,
                                      const std::string& new_directory)
    -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    if (!record->has_path())
    {
        result.error_message = "Cannot move unsaved artifact";
        return result;
    }

    auto old_path = fs::path(record->file_path.value());
    auto new_path = fs::path(new_directory) / old_path.filename();

    ArtifactRecord updated = *record;
    updated.file_path = new_path.string();
    (void)registry_.update(artifact_id, std::move(updated));

    result.success = true;
    result.artifact_id = artifact_id;
    result.resolved_path = new_path.string();

    MARKAMP_LOG_INFO("Artifact moved: {} -> {}", artifact_id.value, new_path.string());
    return result;
}

// ============================================================================
// Duplicate
// ============================================================================

auto TextArtifactLifecycle::duplicate(const ArtifactId& artifact_id) -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    const auto* original = registry_.find(artifact_id);
    if (original == nullptr)
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    // Create a new artifact with the same content
    FilePlacementContext ctx;
    ctx.entry_point = "duplicate";
    if (original->has_path())
    {
        ctx.target_directory = fs::path(original->file_path.value()).parent_path().string();
    }

    auto new_name = original->display_name + " (copy)";
    auto dup_result = create_new_file(ctx, new_name, original->language_id);

    if (!dup_result.ok())
    {
        return dup_result;
    }

    // Copy content if original has unsaved buffer
    const auto* orig_buffer = buffer_manager_.get_buffer(artifact_id);
    if (orig_buffer != nullptr)
    {
        (void)buffer_manager_.set_content(dup_result.artifact_id, orig_buffer->content);
    }

    MARKAMP_LOG_INFO("Artifact duplicated: {} -> {}", artifact_id.value,
                     dup_result.artifact_id.value);
    return dup_result;
}

// ============================================================================
// Delete
// ============================================================================

auto TextArtifactLifecycle::remove(const ArtifactId& artifact_id, bool /*delete_from_disk*/)
    -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    // Remove unsaved buffer if any
    (void)buffer_manager_.remove_buffer(artifact_id);

    // Unregister from registry
    if (!registry_.unregister(artifact_id))
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Artifact removed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Close
// ============================================================================

auto TextArtifactLifecycle::close(const ArtifactId& artifact_id) -> LifecycleOperationResult
{
    ++operation_count_;
    LifecycleOperationResult result;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    // Remove unsaved buffer  
    (void)buffer_manager_.remove_buffer(artifact_id);

    // Unregister from the artifact registry
    (void)registry_.unregister(artifact_id);

    result.success = true;
    result.artifact_id = artifact_id;

    MARKAMP_LOG_INFO("Artifact closed: {}", artifact_id.value);
    return result;
}

// ============================================================================
// Queries
// ============================================================================

auto TextArtifactLifecycle::has_unsaved_changes(const ArtifactId& artifact_id) const -> bool
{
    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        return false;
    }

    if (record->is_unsaved() || record->is_dirty())
    {
        return true;
    }

    const auto* buffer = buffer_manager_.get_buffer(artifact_id);
    return buffer != nullptr && buffer->is_modified;
}

auto TextArtifactLifecycle::get_buffer(const ArtifactId& artifact_id) const
    -> const UnsavedBuffer*
{
    return buffer_manager_.get_buffer(artifact_id);
}

auto TextArtifactLifecycle::open_text_file_count() const -> int
{
    return registry_.count_by_kind(ArtifactKind::kTextFile);
}

auto TextArtifactLifecycle::unsaved_count() const -> int
{
    return registry_.count_by_state(ArtifactLifecycleState::kUnsaved);
}

auto TextArtifactLifecycle::dirty_count() const -> int
{
    return registry_.count_by_state(ArtifactLifecycleState::kDirty);
}

} // namespace markamp::core
