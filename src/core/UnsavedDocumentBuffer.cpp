/// @file UnsavedDocumentBuffer.cpp
/// @brief V20 P02-T01: Unsaved document buffer implementation.

#include "UnsavedDocumentBuffer.h"

#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

auto UnsavedBuffer::line_count() const noexcept -> int
{
    if (content.empty())
    {
        return 0;
    }
    return static_cast<int>(std::count(content.begin(), content.end(), '\n')) + 1;
}

auto UnsavedDocumentBufferManager::create_buffer(const ArtifactId& artifact_id,
                                                   const std::string& language_id) -> bool
{
    if (artifact_id.empty())
    {
        MARKAMP_LOG_WARN("Cannot create buffer for empty artifact ID");
        return false;
    }

    if (buffers_.contains(artifact_id.value))
    {
        MARKAMP_LOG_WARN("Buffer already exists for artifact: {}", artifact_id.value);
        return false;
    }

    UnsavedBuffer buffer;
    buffer.artifact_id = artifact_id;
    buffer.language_id = language_id;
    buffer.is_modified = false;

    buffers_.emplace(artifact_id.value, std::move(buffer));
    MARKAMP_LOG_DEBUG("Unsaved buffer created for artifact: {}", artifact_id.value);
    return true;
}

auto UnsavedDocumentBufferManager::get_buffer(const ArtifactId& artifact_id) -> UnsavedBuffer*
{
    auto iter = buffers_.find(artifact_id.value);
    if (iter == buffers_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto UnsavedDocumentBufferManager::get_buffer(const ArtifactId& artifact_id) const
    -> const UnsavedBuffer*
{
    auto iter = buffers_.find(artifact_id.value);
    if (iter == buffers_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto UnsavedDocumentBufferManager::set_content(const ArtifactId& artifact_id,
                                                 const std::string& content) -> bool
{
    auto* buffer = get_buffer(artifact_id);
    if (buffer == nullptr)
    {
        MARKAMP_LOG_WARN("Cannot set content: buffer not found for {}", artifact_id.value);
        return false;
    }

    buffer->content = content;
    buffer->is_modified = true;
    buffer->last_edit = std::chrono::steady_clock::now();

    MARKAMP_LOG_DEBUG("Buffer content updated: {} ({} bytes)", artifact_id.value, content.size());
    return true;
}

auto UnsavedDocumentBufferManager::set_session_state(const ArtifactId& artifact_id,
                                                       const EditorSessionState& state) -> bool
{
    auto* buffer = get_buffer(artifact_id);
    if (buffer == nullptr)
    {
        return false;
    }

    buffer->session_state = state;
    return true;
}

void UnsavedDocumentBufferManager::mark_clean(const ArtifactId& artifact_id)
{
    auto* buffer = get_buffer(artifact_id);
    if (buffer != nullptr)
    {
        buffer->is_modified = false;
    }
}

auto UnsavedDocumentBufferManager::remove_buffer(const ArtifactId& artifact_id) -> bool
{
    auto iter = buffers_.find(artifact_id.value);
    if (iter == buffers_.end())
    {
        return false;
    }

    buffers_.erase(iter);
    MARKAMP_LOG_DEBUG("Unsaved buffer removed: {}", artifact_id.value);
    return true;
}

auto UnsavedDocumentBufferManager::has_buffer(const ArtifactId& artifact_id) const -> bool
{
    return buffers_.contains(artifact_id.value);
}

auto UnsavedDocumentBufferManager::all_artifact_ids() const -> std::vector<ArtifactId>
{
    std::vector<ArtifactId> ids;
    ids.reserve(buffers_.size());
    for (const auto& [key, buffer] : buffers_)
    {
        ids.push_back(buffer.artifact_id);
    }
    return ids;
}

void UnsavedDocumentBufferManager::clear()
{
    buffers_.clear();
    MARKAMP_LOG_DEBUG("All unsaved buffers cleared");
}

} // namespace markamp::core
