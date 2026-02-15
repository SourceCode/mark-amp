#include "TagService.h"

#include "Events.h"
#include "VaultService.h"

#include <algorithm>
#include <ranges>
#include <set>

namespace markamp::core
{

// ============================================================================
// TagTreeNode
// ============================================================================

auto TagTreeNode::is_leaf() const -> bool
{
    return children.empty();
}

auto TagTreeNode::child_count() const -> int
{
    return static_cast<int>(children.size());
}

// ============================================================================
// TagService
// ============================================================================

TagService::TagService(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
    vault_opened_sub_ = event_bus_.subscribe<events::VaultOpenedEvent>(
        [this](const events::VaultOpenedEvent& /*event*/) { rebuild(); });

    doc_created_sub_ = event_bus_.subscribe<events::VaultDocumentCreatedEvent>(
        [this](const events::VaultDocumentCreatedEvent& event)
        { update_document(event.document_id); });

    doc_deleted_sub_ = event_bus_.subscribe<events::VaultDocumentDeletedEvent>(
        [this](const events::VaultDocumentDeletedEvent& event)
        { remove_document(event.document_id); });
}

// ============================================================================
// Index Management
// ============================================================================

auto TagService::rebuild() -> void
{
    const std::lock_guard lock(mutex_);

    tag_to_docs_.clear();
    doc_to_tags_.clear();
    metadata_index_.clear();

    const auto documents = vault_service_.list_documents();
    int total_assignments = 0;

    for (const auto& entry : documents)
    {
        const auto& doc_id = entry.document_id;
        std::vector<std::string> tags;

        // Collect tags from VaultIndexEntry
        for (const auto& tag : entry.tags)
        {
            tags.push_back(tag);
            tag_to_docs_[tag].push_back(doc_id);
            ++total_assignments;
        }

        if (!tags.empty())
        {
            doc_to_tags_[doc_id] = std::move(tags);
        }

        // Index frontmatter metadata fields
        auto doc_result = vault_service_.open_document(entry.file_path);
        if (doc_result.has_value())
        {
            const auto& fm = doc_result.value()->frontmatter();
            for (const auto& [key, value] : fm.string_fields)
            {
                metadata_index_[key][value].push_back(doc_id);
            }
        }
    }

    events::TagIndexRebuiltEvent evt;
    evt.unique_tags = static_cast<int>(tag_to_docs_.size());
    evt.total_tag_assignments = total_assignments;
    event_bus_.publish(evt);
}

auto TagService::update_document(const std::string& document_id) -> void
{
    const std::lock_guard lock(mutex_);

    // Remove old entries for this document
    auto old_tags_it = doc_to_tags_.find(document_id);
    if (old_tags_it != doc_to_tags_.end())
    {
        for (const auto& tag : old_tags_it->second)
        {
            auto& docs = tag_to_docs_[tag];
            std::erase(docs, document_id);
            if (docs.empty())
            {
                tag_to_docs_.erase(tag);
            }
        }
        doc_to_tags_.erase(old_tags_it);
    }

    // Remove old metadata entries
    for (auto& [field_name, value_map] : metadata_index_)
    {
        for (auto& [value, doc_ids] : value_map)
        {
            std::erase(doc_ids, document_id);
        }
    }

    // Look up document in the vault index
    const auto documents = vault_service_.list_documents();
    for (const auto& entry : documents)
    {
        if (entry.document_id != document_id)
        {
            continue;
        }

        // Re-add tags
        std::vector<std::string> tags;
        for (const auto& tag : entry.tags)
        {
            tags.push_back(tag);
            tag_to_docs_[tag].push_back(document_id);
        }
        if (!tags.empty())
        {
            doc_to_tags_[document_id] = std::move(tags);
        }

        // Re-add metadata
        auto doc_result = vault_service_.open_document(entry.file_path);
        if (doc_result.has_value())
        {
            const auto& fm = doc_result.value()->frontmatter();
            for (const auto& [key, value] : fm.string_fields)
            {
                metadata_index_[key][value].push_back(document_id);
            }
        }
        break;
    }
}

auto TagService::remove_document(const std::string& document_id) -> void
{
    const std::lock_guard lock(mutex_);

    auto tags_it = doc_to_tags_.find(document_id);
    if (tags_it != doc_to_tags_.end())
    {
        for (const auto& tag : tags_it->second)
        {
            auto& docs = tag_to_docs_[tag];
            std::erase(docs, document_id);
            if (docs.empty())
            {
                tag_to_docs_.erase(tag);
            }
        }
        doc_to_tags_.erase(tags_it);
    }

    // Remove from metadata index
    for (auto& [field_name, value_map] : metadata_index_)
    {
        for (auto& [value, doc_ids] : value_map)
        {
            std::erase(doc_ids, document_id);
        }
    }
}

// ============================================================================
// Tag Queries
// ============================================================================

auto TagService::get_tag_tree() const -> std::vector<TagTreeNode>
{
    const std::lock_guard lock(mutex_);

    auto tree = build_tag_tree_recursive("");

    // Compute recursive counts
    for (auto& node : tree)
    {
        compute_recursive_count(node);
    }

    return tree;
}

auto TagService::build_tag_tree_recursive(const std::string& prefix) const
    -> std::vector<TagTreeNode>
{
    // Collect unique first-level names at this prefix
    std::set<std::string> level_names;

    for (const auto& [tag_path, doc_ids] : tag_to_docs_)
    {
        std::string remainder;
        if (prefix.empty())
        {
            remainder = tag_path;
        }
        else if (tag_path.size() > prefix.size() + 1 &&
                 tag_path.substr(0, prefix.size()) == prefix && tag_path[prefix.size()] == '/')
        {
            remainder = tag_path.substr(prefix.size() + 1);
        }
        else
        {
            continue;
        }

        // Get the first segment
        const auto slash_pos = remainder.find('/');
        const std::string first_segment =
            (slash_pos != std::string::npos) ? remainder.substr(0, slash_pos) : remainder;

        if (!first_segment.empty())
        {
            level_names.insert(first_segment);
        }
    }

    std::vector<TagTreeNode> nodes;
    for (const auto& name : level_names)
    {
        TagTreeNode node;
        node.name = name;
        node.full_path = prefix.empty() ? name : (prefix + "/" + name);

        // Direct document count (exact tag match)
        auto it = tag_to_docs_.find(node.full_path);
        if (it != tag_to_docs_.end())
        {
            node.document_count = static_cast<int>(it->second.size());
        }

        // Recurse for children
        node.children = build_tag_tree_recursive(node.full_path);

        nodes.push_back(std::move(node));
    }

    return nodes;
}

auto TagService::compute_recursive_count(TagTreeNode& node) const -> void
{
    node.recursive_count = node.document_count;
    for (auto& child : node.children)
    {
        compute_recursive_count(child);
        node.recursive_count += child.recursive_count;
    }
}

auto TagService::get_all_tags() const -> std::vector<TagStats>
{
    const std::lock_guard lock(mutex_);

    std::vector<TagStats> result;
    result.reserve(tag_to_docs_.size());

    for (const auto& [tag, doc_ids] : tag_to_docs_)
    {
        TagStats stats;
        stats.tag = tag;
        stats.count = static_cast<int>(doc_ids.size());
        stats.document_ids = doc_ids;
        result.push_back(std::move(stats));
    }

    std::ranges::sort(result, {}, &TagStats::tag);
    return result;
}

auto TagService::get_documents_for_tag(const std::string& tag, bool recursive) const
    -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);

    if (!recursive)
    {
        auto it = tag_to_docs_.find(tag);
        if (it != tag_to_docs_.end())
        {
            return it->second;
        }
        return {};
    }

    // Recursive: collect all tags starting with tag + "/"
    std::set<std::string> doc_set;
    const std::string prefix = tag + "/";

    for (const auto& [tag_path, doc_ids] : tag_to_docs_)
    {
        if (tag_path == tag || tag_path.starts_with(prefix))
        {
            doc_set.insert(doc_ids.begin(), doc_ids.end());
        }
    }

    return {doc_set.begin(), doc_set.end()};
}

auto TagService::find_tags_by_prefix(const std::string& prefix) const -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);

    std::vector<std::string> matches;
    for (const auto& [tag_path, doc_ids] : tag_to_docs_)
    {
        if (tag_path.starts_with(prefix))
        {
            matches.push_back(tag_path);
        }
    }

    std::ranges::sort(matches);
    return matches;
}

auto TagService::tag_count() const -> int
{
    const std::lock_guard lock(mutex_);
    return static_cast<int>(tag_to_docs_.size());
}

// ============================================================================
// Metadata Queries
// ============================================================================

auto TagService::get_field_values(const std::string& field_name) const -> MetadataFieldStats
{
    const std::lock_guard lock(mutex_);

    MetadataFieldStats stats;
    stats.field_name = field_name;

    auto field_it = metadata_index_.find(field_name);
    if (field_it != metadata_index_.end())
    {
        std::set<std::string> unique_docs;
        for (const auto& [value, doc_ids] : field_it->second)
        {
            stats.value_counts[value] = static_cast<int>(doc_ids.size());
            unique_docs.insert(doc_ids.begin(), doc_ids.end());
        }
        stats.total_documents = static_cast<int>(unique_docs.size());
    }

    return stats;
}

auto TagService::find_by_field(const std::string& field_name, const std::string& value) const
    -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);

    auto field_it = metadata_index_.find(field_name);
    if (field_it == metadata_index_.end())
    {
        return {};
    }

    auto value_it = field_it->second.find(value);
    if (value_it == field_it->second.end())
    {
        return {};
    }

    return value_it->second;
}

auto TagService::get_all_field_names() const -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);

    std::vector<std::string> names;
    names.reserve(metadata_index_.size());

    for (const auto& [field_name, value_map] : metadata_index_)
    {
        names.push_back(field_name);
    }

    std::ranges::sort(names);
    return names;
}

// ============================================================================
// Tag Operations
// ============================================================================

auto TagService::rename_tag(const std::string& old_tag, const std::string& new_tag) -> int
{
    const std::lock_guard lock(mutex_);

    auto it = tag_to_docs_.find(old_tag);
    if (it == tag_to_docs_.end())
    {
        return 0;
    }

    const auto doc_ids = it->second; // Copy since we're modifying
    const int modified_count = static_cast<int>(doc_ids.size());

    // Move documents from old tag to new tag
    for (const auto& doc_id : doc_ids)
    {
        tag_to_docs_[new_tag].push_back(doc_id);

        // Update doc_to_tags_
        auto& tags = doc_to_tags_[doc_id];
        std::replace(tags.begin(), tags.end(), old_tag, new_tag);
    }

    tag_to_docs_.erase(old_tag);

    events::VaultTagRenamedEvent evt;
    evt.old_tag = old_tag;
    evt.new_tag = new_tag;
    evt.documents_modified = modified_count;
    event_bus_.publish(evt);

    return modified_count;
}

auto TagService::delete_tag(const std::string& tag) -> int
{
    const std::lock_guard lock(mutex_);

    auto it = tag_to_docs_.find(tag);
    if (it == tag_to_docs_.end())
    {
        return 0;
    }

    const auto doc_ids = it->second;
    const int modified_count = static_cast<int>(doc_ids.size());

    // Remove tag from each document's tag list
    for (const auto& doc_id : doc_ids)
    {
        auto& tags = doc_to_tags_[doc_id];
        std::erase(tags, tag);
        if (tags.empty())
        {
            doc_to_tags_.erase(doc_id);
        }
    }

    tag_to_docs_.erase(it);

    events::VaultTagDeletedEvent evt;
    evt.tag = tag;
    evt.documents_modified = modified_count;
    event_bus_.publish(evt);

    return modified_count;
}

auto TagService::merge_tags(const std::string& source_tag, const std::string& target_tag) -> int
{
    // merge is rename source -> target, handling dedup
    const std::lock_guard lock(mutex_);

    auto src_it = tag_to_docs_.find(source_tag);
    if (src_it == tag_to_docs_.end())
    {
        return 0;
    }

    const auto src_doc_ids = src_it->second;
    const int modified_count = static_cast<int>(src_doc_ids.size());

    // Merge source docs into target
    auto& target_docs = tag_to_docs_[target_tag];
    for (const auto& doc_id : src_doc_ids)
    {
        // Avoid duplicates
        if (std::ranges::find(target_docs, doc_id) == target_docs.end())
        {
            target_docs.push_back(doc_id);
        }

        // Update doc_to_tags_
        auto& tags = doc_to_tags_[doc_id];
        std::replace(tags.begin(), tags.end(), source_tag, target_tag);

        // Remove duplicate tags within the doc
        std::ranges::sort(tags);
        const auto [first, last] = std::ranges::unique(tags);
        tags.erase(first, last);
    }

    tag_to_docs_.erase(source_tag);

    return modified_count;
}

} // namespace markamp::core
