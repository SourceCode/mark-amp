#include "ReplaceUndoBuffer.h"

namespace markamp::core
{

void ReplaceUndoBuffer::begin_batch()
{
    snapshots_.clear();
    has_batch_ = true;
}

void ReplaceUndoBuffer::snapshot(const std::string& file_path, const std::string& content)
{
    if (!has_batch_)
    {
        begin_batch();
    }
    // Only snapshot each file once per batch (first snapshot wins).
    if (snapshots_.find(file_path) == snapshots_.end())
    {
        snapshots_[file_path] = content;
    }
}

auto ReplaceUndoBuffer::undo_all() -> std::vector<FileSnapshot>
{
    std::vector<FileSnapshot> result;
    result.reserve(snapshots_.size());
    for (auto& [path, content] : snapshots_)
    {
        result.push_back(FileSnapshot{path, std::move(content)});
    }
    clear();
    return result;
}

auto ReplaceUndoBuffer::has_undo() const -> bool
{
    return has_batch_ && !snapshots_.empty();
}

auto ReplaceUndoBuffer::file_count() const -> std::size_t
{
    return snapshots_.size();
}

void ReplaceUndoBuffer::clear()
{
    snapshots_.clear();
    has_batch_ = false;
}

} // namespace markamp::core
