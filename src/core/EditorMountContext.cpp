/// @file EditorMountContext.cpp
/// @brief V20 P02-T04: Editor session manager implementation.

#include "EditorMountContext.h"

#include "Logger.h"

namespace markamp::core
{

void EditorSessionManager::save_session(const ArtifactId& artifact_id,
                                          const EditorSessionState& state)
{
    sessions_[artifact_id.value] = state;
    MARKAMP_LOG_DEBUG("Session saved: {} (line={}, col={})", artifact_id.value, state.cursor_line,
                      state.cursor_column);
}

auto EditorSessionManager::get_session(const ArtifactId& artifact_id) const -> EditorSessionState
{
    auto iter = sessions_.find(artifact_id.value);
    if (iter == sessions_.end())
    {
        return {};
    }
    return iter->second;
}

auto EditorSessionManager::has_session(const ArtifactId& artifact_id) const -> bool
{
    return sessions_.contains(artifact_id.value);
}

void EditorSessionManager::remove_session(const ArtifactId& artifact_id)
{
    sessions_.erase(artifact_id.value);
}

auto EditorSessionManager::build_mount_descriptor(const ArtifactRecord& record) const
    -> EditorMountDescriptor
{
    EditorMountDescriptor desc;
    desc.artifact_id = record.id;
    desc.file_path = record.file_path;
    desc.language_id = record.language_id;
    desc.display_name = record.display_name;
    desc.is_unsaved = record.is_unsaved();
    desc.session_state = get_session(record.id);
    return desc;
}

void EditorSessionManager::clear()
{
    sessions_.clear();
    MARKAMP_LOG_DEBUG("All editor sessions cleared");
}

} // namespace markamp::core
