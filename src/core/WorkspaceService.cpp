#include "WorkspaceService.h"

#include <algorithm>
#include <filesystem>
#include <regex>

namespace markamp::core
{

// ── Document Tracking ──

void WorkspaceService::open_document(TextDocumentItem document)
{
    const auto uri = document.uri;
    documents_[uri] = std::move(document);

    // Fire on_did_open listeners
    for (const auto& entry : listeners_)
    {
        if (entry.type == ListenerEntry::Type::kOpen && entry.doc_callback)
        {
            entry.doc_callback(documents_[uri]);
        }
    }
}

void WorkspaceService::close_document(const std::string& uri)
{
    documents_.erase(uri);

    // Fire on_did_close listeners
    for (const auto& entry : listeners_)
    {
        if (entry.type == ListenerEntry::Type::kClose && entry.uri_callback)
        {
            entry.uri_callback(uri);
        }
    }
}

void WorkspaceService::change_document(const std::string& uri,
                                       std::int64_t version,
                                       std::vector<ContentChange> changes)
{
    auto iter = documents_.find(uri);
    if (iter == documents_.end())
    {
        return;
    }

    iter->second.version = version;

    // For simplicity, if changes contain full content replacement (empty range),
    // use the last change's text as the new content.
    if (!changes.empty())
    {
        // If this is a full-content replacement (range is 0,0,0,0), set content directly
        const auto& last_change = changes.back();
        if (last_change.range.start_line == 0 && last_change.range.start_character == 0 &&
            last_change.range.end_line == 0 && last_change.range.end_character == 0)
        {
            iter->second.content = last_change.text;
        }
    }

    // Fire on_did_change listeners
    TextDocumentChangeEvent event{uri, version, std::move(changes)};
    for (const auto& entry : listeners_)
    {
        if (entry.type == ListenerEntry::Type::kChange && entry.change_callback)
        {
            entry.change_callback(event);
        }
    }
}

void WorkspaceService::save_document(const std::string& uri)
{
    // Fire on_did_save listeners
    for (const auto& entry : listeners_)
    {
        if (entry.type == ListenerEntry::Type::kSave && entry.uri_callback)
        {
            entry.uri_callback(uri);
        }
    }
}

auto WorkspaceService::get_document(const std::string& uri) const -> const TextDocumentItem*
{
    auto iter = documents_.find(uri);
    if (iter == documents_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto WorkspaceService::text_documents() const -> std::vector<TextDocumentItem>
{
    std::vector<TextDocumentItem> result;
    result.reserve(documents_.size());
    for (const auto& [uri_key, doc] : documents_)
    {
        result.push_back(doc);
    }
    return result;
}

// ── Document Events ──

auto WorkspaceService::on_did_open(DocumentCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back(
        {listener_id, ListenerEntry::Type::kOpen, std::move(callback), nullptr, nullptr});
    return listener_id;
}

auto WorkspaceService::on_did_close(UriCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back(
        {listener_id, ListenerEntry::Type::kClose, nullptr, nullptr, std::move(callback)});
    return listener_id;
}

auto WorkspaceService::on_did_change(ChangeCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back(
        {listener_id, ListenerEntry::Type::kChange, nullptr, std::move(callback), nullptr});
    return listener_id;
}

auto WorkspaceService::on_did_save(UriCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back(
        {listener_id, ListenerEntry::Type::kSave, nullptr, nullptr, std::move(callback)});
    return listener_id;
}

void WorkspaceService::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const ListenerEntry& entry)
                                    { return entry.listener_id == listener_id; }),
                     listeners_.end());
}

// ── Workspace Folders ──

auto WorkspaceService::workspace_folders() const -> const std::vector<WorkspaceFolder>&
{
    return workspace_folders_;
}

void WorkspaceService::add_workspace_folder(WorkspaceFolder folder)
{
    workspace_folders_.push_back(std::move(folder));
}

void WorkspaceService::remove_workspace_folder(const std::string& uri)
{
    workspace_folders_.erase(std::remove_if(workspace_folders_.begin(),
                                            workspace_folders_.end(),
                                            [&uri](const WorkspaceFolder& folder)
                                            { return folder.uri == uri; }),
                             workspace_folders_.end());
}

// ── Workspace Edit ──

auto WorkspaceService::apply_edit(const WorkspaceEditBatch& edit) -> bool
{
    // Apply edits to tracked documents
    for (const auto& text_edit : edit.edits)
    {
        auto iter = documents_.find(text_edit.uri);
        if (iter == documents_.end())
        {
            return false; // Document not found
        }
        // For simplicity: replace entire content if range is zero
        // A full implementation would apply line/character-level edits
        iter->second.content = text_edit.new_text;
        iter->second.version++;
    }
    return true;
}

// ── File Search ──

auto WorkspaceService::find_files(const std::string& glob_pattern) const -> std::vector<std::string>
{
    std::vector<std::string> results;

    // Search within workspace folders
    for (const auto& folder : workspace_folders_)
    {
        std::string folder_path = folder.uri;

        // Strip file:// prefix if present
        const std::string file_prefix = "file://";
        if (folder_path.starts_with(file_prefix))
        {
            folder_path = folder_path.substr(file_prefix.size());
        }

        std::error_code error_code;
        if (!std::filesystem::exists(folder_path, error_code))
        {
            continue;
        }

        // Simple glob matching: convert glob to regex
        // Support: *, ?, **
        std::string regex_pattern;
        for (const char character : glob_pattern)
        {
            switch (character)
            {
                case '*':
                    regex_pattern += ".*";
                    break;
                case '?':
                    regex_pattern += '.';
                    break;
                case '.':
                    regex_pattern += "\\.";
                    break;
                default:
                    regex_pattern += character;
                    break;
            }
        }

        try
        {
            const std::regex pattern(regex_pattern);
            for (const auto& entry :
                 std::filesystem::recursive_directory_iterator(folder_path, error_code))
            {
                if (entry.is_regular_file())
                {
                    const auto filename = entry.path().filename().string();
                    if (std::regex_match(filename, pattern))
                    {
                        results.push_back("file://" + entry.path().string());
                    }
                }
            }
        }
        catch (const std::regex_error&)
        {
            // Invalid pattern — skip
        }
    }

    return results;
}

} // namespace markamp::core
