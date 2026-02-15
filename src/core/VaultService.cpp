#include "core/VaultService.h"

#include "core/BlockID.h" // BlockIDGenerator
#include "core/Config.h"
#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <ranges>
#include <set>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// VaultIndexEntry
// ============================================================================

auto VaultIndexEntry::matches_name(const std::string& name) const -> bool
{
    // Case-insensitive comparison helper
    auto iequal = [](const std::string& lhs, const std::string& rhs) -> bool
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }
        return std::equal(lhs.begin(),
                          lhs.end(),
                          rhs.begin(),
                          [](unsigned char lch, unsigned char rch)
                          { return std::tolower(lch) == std::tolower(rch); });
    };

    if (iequal(title, name))
    {
        return true;
    }

    return std::ranges::any_of(aliases,
                               [&](const std::string& alias) { return iequal(alias, name); });
}

// ============================================================================
// VaultService construction / destruction
// ============================================================================

VaultService::VaultService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

VaultService::~VaultService()
{
    if (is_open_)
    {
        close_vault();
    }
}

// ============================================================================
// Utility helpers
// ============================================================================

auto VaultService::to_lower(const std::string& str) -> std::string
{
    std::string result = str;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    return result;
}

auto VaultService::extract_title(const std::string& content,
                                 const std::filesystem::path& file_path,
                                 const Frontmatter& frontmatter) -> std::string
{
    // Priority 1: Frontmatter title
    if (!frontmatter.title.empty())
    {
        return frontmatter.title;
    }

    // Priority 2: First H1 heading
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.starts_with("# ") && line.size() > 2)
        {
            return line.substr(2);
        }
    }

    // Priority 3: Filename without extension
    return file_path.stem().string();
}

// ============================================================================
// Vault open / close
// ============================================================================

auto VaultService::open_vault(const std::filesystem::path& path) -> std::expected<void, std::string>
{
    if (is_open_)
    {
        close_vault();
    }

    if (!std::filesystem::exists(path))
    {
        return std::unexpected("Vault directory does not exist: " + path.string());
    }

    if (!std::filesystem::is_directory(path))
    {
        return std::unexpected("Path is not a directory: " + path.string());
    }

    vault_path_ = path;

    // Scan and index all .md files
    auto files = scan_vault_directory();
    for (const auto& file : files)
    {
        auto result = index_file(file);
        if (!result.has_value())
        {
            // Log warning but continue indexing
        }
    }

    rebuild_name_index();
    rebuild_tag_index();

    is_open_ = true;

    // Publish event
    events::VaultOpenedEvent opened_evt;
    opened_evt.vault_path = vault_path_.string();
    opened_evt.document_count = static_cast<int>(index_by_id_.size());
    event_bus_.publish(opened_evt);

    return {};
}

auto VaultService::close_vault() -> void
{
    stop_file_watcher();

    const std::string closed_path = vault_path_.string();

    {
        const std::unique_lock lock(index_mutex_);
        index_by_id_.clear();
        name_to_id_.clear();
        tag_to_ids_.clear();
        open_documents_.clear();
    }

    vault_path_.clear();
    is_open_ = false;

    events::VaultClosedEvent closed_evt;
    closed_evt.vault_path = closed_path;
    event_bus_.publish(closed_evt);
}

auto VaultService::vault_path() const -> const std::filesystem::path&
{
    return vault_path_;
}

auto VaultService::is_open() const -> bool
{
    return is_open_;
}

// ============================================================================
// File scanning
// ============================================================================

auto VaultService::scan_vault_directory() -> std::vector<std::filesystem::path>
{
    std::vector<std::filesystem::path> files;

    // Parse excluded directories from config
    std::set<std::string> excluded_dirs = {".trash", ".obsidian", ".git"};
    auto config_excluded = config_.get_string("vault.excluded_dirs", ".trash,.obsidian,.git");
    {
        std::istringstream stream(config_excluded);
        std::string dir_name;
        while (std::getline(stream, dir_name, ','))
        {
            if (!dir_name.empty())
            {
                excluded_dirs.insert(dir_name);
            }
        }
    }

    std::error_code error_code;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             vault_path_, std::filesystem::directory_options::skip_permission_denied, error_code))
    {
        if (error_code)
        {
            continue;
        }

        // Skip hidden directories and excluded directories
        if (entry.is_directory())
        {
            const auto dir_name = entry.path().filename().string();
            if (dir_name.starts_with(".") || excluded_dirs.contains(dir_name))
            {
                // Note: recursive_directory_iterator will still descend; we skip files
                continue;
            }
        }

        if (!entry.is_regular_file())
        {
            continue;
        }

        // Check if any parent directory is hidden or excluded
        auto relative = std::filesystem::relative(entry.path(), vault_path_, error_code);
        if (error_code)
        {
            continue;
        }

        bool skip = false;
        for (const auto& component : relative)
        {
            const auto comp_str = component.string();
            if (comp_str.starts_with(".") || excluded_dirs.contains(comp_str))
            {
                skip = true;
                break;
            }
        }
        if (skip)
        {
            continue;
        }

        if (entry.path().extension() == ".md")
        {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

// ============================================================================
// File indexing
// ============================================================================

auto VaultService::index_file(const std::filesystem::path& file_path)
    -> std::expected<void, std::string>
{
    // Read file content
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        return std::unexpected("Cannot open file: " + file_path.string());
    }

    const std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    file.close();

    // Parse frontmatter
    auto frontmatter_opt = link_parser_.parse_frontmatter(content);
    Frontmatter frontmatter;
    if (frontmatter_opt.has_value())
    {
        frontmatter = std::move(frontmatter_opt.value());
    }

    // Extract title
    const auto doc_title = extract_title(content, file_path, frontmatter);

    // Generate document ID
    const auto doc_id = BlockIDGenerator::generate();

    // Parse tags from content
    auto content_tags = link_parser_.parse_tags(content);
    std::vector<std::string> all_tags = frontmatter.tags;
    for (const auto& tag : content_tags)
    {
        // Add content tags that aren't already in frontmatter tags
        const bool already_present = std::ranges::any_of(
            all_tags, [&](const std::string& existing) { return existing == tag.full_tag; });
        if (!already_present)
        {
            all_tags.push_back(tag.full_tag);
        }
    }

    // Get file metadata
    std::error_code error_code;
    auto file_modified = std::filesystem::last_write_time(file_path, error_code);
    auto file_sz = std::filesystem::file_size(file_path, error_code);

    VaultIndexEntry entry;
    entry.document_id = doc_id;
    entry.title = doc_title;
    entry.file_path = file_path;
    entry.notebook_id = "default";
    entry.aliases = frontmatter.aliases;
    entry.tags = all_tags;
    entry.modified_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(file_modified.time_since_epoch())
            .count();
    entry.file_size = static_cast<int64_t>(file_sz);

    {
        std::unique_lock lock(index_mutex_);
        index_by_id_[doc_id] = std::move(entry);
    }

    return {};
}

auto VaultService::remove_from_index(const std::string& document_id) -> void
{
    std::unique_lock lock(index_mutex_);
    index_by_id_.erase(document_id);

    // Clean name map entries pointing to this ID
    std::erase_if(name_to_id_, [&](const auto& pair) { return pair.second == document_id; });

    // Clean tag map entries
    for (auto& [tag, ids] : tag_to_ids_)
    {
        std::erase(ids, document_id);
    }

    // Remove empty tag entries
    std::erase_if(tag_to_ids_, [](const auto& pair) { return pair.second.empty(); });

    // Remove from open documents
    open_documents_.erase(document_id);
}

auto VaultService::rebuild_name_index() -> void
{
    std::unique_lock lock(index_mutex_);
    name_to_id_.clear();

    for (const auto& [doc_id, entry] : index_by_id_)
    {
        name_to_id_[to_lower(entry.title)] = doc_id;
        for (const auto& alias : entry.aliases)
        {
            name_to_id_[to_lower(alias)] = doc_id;
        }
    }
}

auto VaultService::rebuild_tag_index() -> void
{
    std::unique_lock lock(index_mutex_);
    tag_to_ids_.clear();

    for (const auto& [doc_id, entry] : index_by_id_)
    {
        for (const auto& tag : entry.tags)
        {
            tag_to_ids_[tag].push_back(doc_id);
        }
    }
}

// ============================================================================
// Document lifecycle
// ============================================================================

auto VaultService::create_document(const std::string& title,
                                   const std::string& folder,
                                   const Frontmatter& frontmatter)
    -> std::expected<std::shared_ptr<DocumentModel>, std::string>
{
    if (!is_open_)
    {
        return std::unexpected("No vault is open");
    }

    // Determine target directory
    std::filesystem::path target_dir = vault_path_;
    if (!folder.empty())
    {
        target_dir /= folder;
        std::error_code error_code;
        std::filesystem::create_directories(target_dir, error_code);
        if (error_code)
        {
            return std::unexpected("Cannot create directory: " + target_dir.string());
        }
    }

    // Build file path
    const std::filesystem::path file_path = target_dir / (title + ".md");

    // Check if file already exists
    if (std::filesystem::exists(file_path))
    {
        return std::unexpected("File already exists: " + file_path.string());
    }

    // Build frontmatter with title
    Frontmatter doc_frontmatter = frontmatter;
    if (doc_frontmatter.title.empty())
    {
        doc_frontmatter.title = title;
    }

    // Build markdown content
    const std::string markdown = doc_frontmatter.to_yaml() + "\n";

    // Write file
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        return std::unexpected("Cannot create file: " + file_path.string());
    }
    file << markdown;
    file.close();

    // Generate ID and create model
    const auto doc_id = BlockIDGenerator::generate();

    auto doc_model = std::make_shared<DocumentModel>(doc_id, "default");
    doc_model->set_title(title);
    doc_model->set_file_path(file_path);
    doc_model->set_markdown(markdown);
    doc_model->reparse();

    // Add to index
    VaultIndexEntry entry;
    entry.document_id = doc_id;
    entry.title = title;
    entry.file_path = file_path;
    entry.notebook_id = "default";
    entry.aliases = doc_frontmatter.aliases;
    entry.tags = doc_frontmatter.tags;

    {
        std::unique_lock lock(index_mutex_);
        index_by_id_[doc_id] = entry;
        open_documents_[doc_id] = doc_model;
    }

    rebuild_name_index();
    rebuild_tag_index();

    // Publish event
    events::VaultDocumentCreatedEvent created_evt;
    created_evt.document_id = doc_id;
    created_evt.title = title;
    created_evt.file_path = file_path.string();
    event_bus_.publish(created_evt);

    return doc_model;
}

auto VaultService::open_document(const std::filesystem::path& relative_path)
    -> std::expected<std::shared_ptr<DocumentModel>, std::string>
{
    if (!is_open_)
    {
        return std::unexpected("No vault is open");
    }

    auto abs_path = vault_path_ / relative_path;
    if (!std::filesystem::exists(abs_path))
    {
        return std::unexpected("File does not exist: " + abs_path.string());
    }

    // Check if already open (search by file path)
    {
        std::shared_lock lock(index_mutex_);
        for (const auto& [doc_id, doc_model] : open_documents_)
        {
            if (doc_model->file_path() == abs_path)
            {
                return doc_model;
            }
        }
    }

    // Read file
    std::ifstream file(abs_path, std::ios::binary);
    if (!file.is_open())
    {
        return std::unexpected("Cannot open file: " + abs_path.string());
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Find the document ID from the index
    std::string doc_id;
    {
        std::shared_lock lock(index_mutex_);
        for (const auto& [idx_id, entry] : index_by_id_)
        {
            if (entry.file_path == abs_path)
            {
                doc_id = idx_id;
                break;
            }
        }
    }

    if (doc_id.empty())
    {
        doc_id = BlockIDGenerator::generate();
    }

    auto doc_model = std::make_shared<DocumentModel>(doc_id, "default");
    doc_model->set_file_path(abs_path);
    doc_model->set_markdown(content);
    doc_model->reparse();

    {
        std::unique_lock lock(index_mutex_);
        open_documents_[doc_id] = doc_model;
    }

    return doc_model;
}

auto VaultService::save_document(const std::string& document_id) -> std::expected<void, std::string>
{
    std::shared_ptr<DocumentModel> doc_model;
    {
        std::shared_lock lock(index_mutex_);
        auto doc_it = open_documents_.find(document_id);
        if (doc_it == open_documents_.end())
        {
            return std::unexpected("Document not found in open documents: " + document_id);
        }
        doc_model = doc_it->second;
    }

    std::ofstream file(doc_model->file_path(), std::ios::binary);
    if (!file.is_open())
    {
        return std::unexpected("Cannot write to file: " + doc_model->file_path().string());
    }

    file << doc_model->markdown();
    file.close();

    doc_model->mark_clean();

    return {};
}

auto VaultService::delete_document(const std::string& document_id)
    -> std::expected<void, std::string>
{
    if (!is_open_)
    {
        return std::unexpected("No vault is open");
    }

    std::filesystem::path file_path;
    {
        std::shared_lock lock(index_mutex_);
        auto idx_it = index_by_id_.find(document_id);
        if (idx_it == index_by_id_.end())
        {
            return std::unexpected("Document not found: " + document_id);
        }
        file_path = idx_it->second.file_path;
    }

    // Delete file from disk
    std::error_code error_code;
    std::filesystem::remove(file_path, error_code);
    if (error_code)
    {
        return std::unexpected("Cannot delete file: " + file_path.string());
    }

    // Remove from index
    remove_from_index(document_id);

    // Publish event
    events::VaultDocumentDeletedEvent deleted_evt;
    deleted_evt.document_id = document_id;
    deleted_evt.file_path = file_path.string();
    event_bus_.publish(deleted_evt);

    return {};
}

auto VaultService::rename_document(const std::string& document_id, const std::string& new_title)
    -> std::expected<void, std::string>
{
    if (!is_open_)
    {
        return std::unexpected("No vault is open");
    }

    std::filesystem::path old_path;
    std::string old_title;
    {
        std::shared_lock lock(index_mutex_);
        auto idx_it = index_by_id_.find(document_id);
        if (idx_it == index_by_id_.end())
        {
            return std::unexpected("Document not found: " + document_id);
        }
        old_path = idx_it->second.file_path;
        old_title = idx_it->second.title;
    }

    // Build new file path (same directory, new name)
    auto new_path = old_path.parent_path() / (new_title + ".md");

    // Rename file on disk
    std::error_code error_code;
    std::filesystem::rename(old_path, new_path, error_code);
    if (error_code)
    {
        return std::unexpected("Cannot rename file: " + error_code.message());
    }

    // Update index
    {
        std::unique_lock lock(index_mutex_);
        auto idx_it = index_by_id_.find(document_id);
        if (idx_it != index_by_id_.end())
        {
            idx_it->second.title = new_title;
            idx_it->second.file_path = new_path;
        }

        // Update open document model
        auto doc_it = open_documents_.find(document_id);
        if (doc_it != open_documents_.end())
        {
            doc_it->second->set_title(new_title);
            doc_it->second->set_file_path(new_path);
        }
    }

    rebuild_name_index();

    // Update wikilinks in other open documents that reference the old title
    {
        std::shared_lock lock(index_mutex_);
        for (auto& [other_id, other_doc] : open_documents_)
        {
            if (other_id == document_id)
            {
                continue;
            }

            bool has_matching_link = false;
            for (const auto& link : other_doc->outgoing_links())
            {
                if (to_lower(link.target) == to_lower(old_title))
                {
                    has_matching_link = true;
                    break;
                }
            }

            if (has_matching_link)
            {
                // Replace link references in markdown content
                std::string content = other_doc->markdown();
                const std::string old_ref = "[[" + old_title + "]]";
                const std::string new_ref = "[[" + new_title + "]]";
                std::size_t search_pos = 0;
                while ((search_pos = content.find(old_ref, search_pos)) != std::string::npos)
                {
                    content.replace(search_pos, old_ref.size(), new_ref);
                    search_pos += new_ref.size();
                }
                other_doc->set_markdown(content);
                other_doc->reparse();
            }
        }
    }

    // Publish event
    events::VaultDocumentRenamedEvent renamed_evt;
    renamed_evt.document_id = document_id;
    renamed_evt.old_title = old_title;
    renamed_evt.new_title = new_title;
    event_bus_.publish(renamed_evt);

    return {};
}

// ============================================================================
// Index queries
// ============================================================================

auto VaultService::find_by_name(const std::string& name) const -> std::optional<VaultIndexEntry>
{
    std::shared_lock lock(index_mutex_);

    // Look up in name_to_id_ map (case-insensitive)
    auto name_it = name_to_id_.find(to_lower(name));
    if (name_it != name_to_id_.end())
    {
        auto idx_it = index_by_id_.find(name_it->second);
        if (idx_it != index_by_id_.end())
        {
            return idx_it->second;
        }
    }

    return std::nullopt;
}

auto VaultService::find_by_tag(const std::string& tag) const -> std::vector<VaultIndexEntry>
{
    std::shared_lock lock(index_mutex_);
    std::vector<VaultIndexEntry> results;

    auto tag_it = tag_to_ids_.find(tag);
    if (tag_it != tag_to_ids_.end())
    {
        for (const auto& doc_id : tag_it->second)
        {
            auto idx_it = index_by_id_.find(doc_id);
            if (idx_it != index_by_id_.end())
            {
                results.push_back(idx_it->second);
            }
        }
    }

    return results;
}

auto VaultService::list_documents() const -> std::vector<VaultIndexEntry>
{
    std::shared_lock lock(index_mutex_);
    std::vector<VaultIndexEntry> results;
    results.reserve(index_by_id_.size());

    for (const auto& [doc_id, entry] : index_by_id_)
    {
        results.push_back(entry);
    }

    // Sort by title for deterministic output
    std::sort(results.begin(),
              results.end(),
              [](const VaultIndexEntry& lhs, const VaultIndexEntry& rhs)
              { return lhs.title < rhs.title; });

    return results;
}

auto VaultService::list_all_tags() const -> std::vector<std::string>
{
    std::shared_lock lock(index_mutex_);
    std::set<std::string> unique_tags;

    for (const auto& [doc_id, entry] : index_by_id_)
    {
        for (const auto& tag : entry.tags)
        {
            unique_tags.insert(tag);
        }
    }

    return {unique_tags.begin(), unique_tags.end()};
}

auto VaultService::list_tag_tree() const
    -> std::vector<std::pair<std::string, std::vector<std::string>>>
{
    std::shared_lock lock(index_mutex_);
    std::map<std::string, std::set<std::string>> tree;

    for (const auto& [doc_id, entry] : index_by_id_)
    {
        for (const auto& tag : entry.tags)
        {
            auto slash_pos = tag.find('/');
            if (slash_pos != std::string::npos)
            {
                std::string root = tag.substr(0, slash_pos);
                tree[root].insert(tag);
            }
            else
            {
                tree[tag]; // Ensure entry exists even without children
            }
        }
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> result;
    for (const auto& [root, children] : tree)
    {
        result.emplace_back(root, std::vector<std::string>(children.begin(), children.end()));
    }
    return result;
}

auto VaultService::document_count() const -> int
{
    std::shared_lock lock(index_mutex_);
    return static_cast<int>(index_by_id_.size());
}

// ============================================================================
// Link resolution
// ============================================================================

auto VaultService::resolve_wikilink(const std::string& target) const -> std::optional<std::string>
{
    std::shared_lock lock(index_mutex_);

    // Case-insensitive name lookup
    auto name_it = name_to_id_.find(to_lower(target));
    if (name_it != name_to_id_.end())
    {
        return name_it->second;
    }

    // Try path-based resolution: target might match end of a file path
    const std::string target_lower = to_lower(target);
    for (const auto& [doc_id, entry] : index_by_id_)
    {
        auto stem = entry.file_path.stem().string();
        if (to_lower(stem) == target_lower)
        {
            return doc_id;
        }
    }

    return std::nullopt;
}

auto VaultService::find_broken_links() const -> std::vector<std::pair<std::string, WikiLink>>
{
    std::shared_lock lock(index_mutex_);
    std::vector<std::pair<std::string, WikiLink>> broken;

    for (const auto& [doc_id, doc_model] : open_documents_)
    {
        for (const auto& link : doc_model->outgoing_links())
        {
            if (link.is_embed)
            {
                continue; // Skip embeds for broken link checking
            }

            auto resolved = name_to_id_.find(to_lower(link.target));
            if (resolved == name_to_id_.end())
            {
                broken.emplace_back(doc_id, link);
            }
        }
    }

    return broken;
}

// ============================================================================
// File watching (stub — platform-specific implementation is separate)
// ============================================================================

auto VaultService::start_file_watcher() -> void
{
    watcher_running_ = true;
    // Actual file watcher implementation uses PlatformAbstraction
    // and is integrated in the UI layer. This is a flag for test purposes.
}

auto VaultService::stop_file_watcher() -> void
{
    watcher_running_ = false;
}

// ============================================================================
// Re-indexing
// ============================================================================

auto VaultService::reindex() -> void
{
    if (!is_open_)
    {
        return;
    }

    auto start = std::chrono::steady_clock::now();

    // Clear existing index
    {
        std::unique_lock lock(index_mutex_);
        index_by_id_.clear();
    }

    // Re-scan and re-index
    auto files = scan_vault_directory();
    for (const auto& file : files)
    {
        (void)index_file(file);
    }

    rebuild_name_index();
    rebuild_tag_index();

    auto end = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    events::VaultReindexedEvent evt;
    evt.document_count = static_cast<int>(index_by_id_.size());
    evt.elapsed_ms = elapsed;
    event_bus_.publish(evt);
}

auto VaultService::reindex_document(const std::string& document_id) -> void
{
    std::filesystem::path file_path;
    {
        std::shared_lock lock(index_mutex_);
        auto idx_it = index_by_id_.find(document_id);
        if (idx_it == index_by_id_.end())
        {
            return;
        }
        file_path = idx_it->second.file_path;
    }

    // Remove old entry and re-index
    {
        std::unique_lock lock(index_mutex_);
        index_by_id_.erase(document_id);
    }

    (void)index_file(file_path);
    rebuild_name_index();
    rebuild_tag_index();
}

} // namespace markamp::core
