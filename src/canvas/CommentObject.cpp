#include "CommentObject.h"

#include <algorithm>

namespace markamp::canvas
{

CommentObject::CommentObject()
    : CanvasObject(CanvasObjectType::Comment)
{
}

auto CommentObject::type_name() const -> std::string
{
    return "comment";
}

auto CommentObject::local_bounds() const -> AABB
{
    // Comment marker is a fixed 24×24 icon.
    constexpr double kCommentSize = 24.0;
    return AABB{0.0, 0.0, kCommentSize, kCommentSize};
}

auto CommentObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<CommentObject>();
    copy->resolved_ = resolved_;
    copy->author_id_ = author_id_;
    copy->author_name_ = author_name_;
    copy->comments_ = comments_;
    copy->pinned_ = pinned_;
    copy->priority_ = priority_;
    return copy;
}

auto CommentObject::add_comment(const Comment& comment) -> void
{
    comments_.push_back(comment);
}

auto CommentObject::resolve() -> void
{
    resolved_ = true;
}

auto CommentObject::unresolve() -> void
{
    resolved_ = false;
}

auto CommentObject::is_resolved() const -> bool
{
    return resolved_;
}

auto CommentObject::comments() const -> const std::vector<Comment>&
{
    return comments_;
}

auto CommentObject::comment_count() const -> size_t
{
    return comments_.size();
}

auto CommentObject::root_comment() const -> const Comment*
{
    if (comments_.empty())
    {
        return nullptr;
    }
    return &comments_.front();
}

auto CommentObject::set_author(const std::string& author_id, const std::string& author_name) -> void
{
    author_id_ = author_id;
    author_name_ = author_name;
}

auto CommentObject::author_id() const -> const std::string&
{
    return author_id_;
}

auto CommentObject::author_name() const -> const std::string&
{
    return author_name_;
}

// ── Enhancements (#19-24) ───────────────────────────────────────

auto CommentObject::is_pinned() const -> bool
{
    return pinned_;
}
auto CommentObject::set_pinned(bool pinned) -> void
{
    pinned_ = pinned;
}
auto CommentObject::priority() const -> CommentPriority
{
    return priority_;
}
auto CommentObject::set_priority(CommentPriority priority) -> void
{
    priority_ = priority;
}

auto CommentObject::unresolved_count() const -> size_t
{
    size_t count = 0;
    for (const auto& comment : comments_)
    {
        if (!comment.is_resolved)
        {
            ++count;
        }
    }
    return count;
}

auto CommentObject::last_activity_at() const -> const std::string&
{
    static const std::string kEmpty;
    if (comments_.empty())
    {
        return kEmpty;
    }
    // Return the latest created_at timestamp by simple string comparison (ISO-8601).
    const auto* latest = &comments_.front();
    for (const auto& comment : comments_)
    {
        if (comment.created_at > latest->created_at)
        {
            latest = &comment;
        }
    }
    return latest->created_at;
}

auto CommentObject::edit_comment(const std::string& comment_id, const std::string& new_body) -> bool
{
    auto iter = std::find_if(comments_.begin(),
                             comments_.end(),
                             [&comment_id](const Comment& comment)
                             { return comment.comment_id == comment_id; });
    if (iter == comments_.end())
    {
        return false;
    }
    iter->body = new_body;
    return true;
}

auto CommentObject::delete_comment(const std::string& comment_id) -> bool
{
    auto iter = std::find_if(comments_.begin(),
                             comments_.end(),
                             [&comment_id](const Comment& comment)
                             { return comment.comment_id == comment_id; });
    if (iter == comments_.end())
    {
        return false;
    }
    comments_.erase(iter);
    return true;
}

// --- Batch 8 (#43-45) ---

auto CommentObject::has_mentions() const -> bool
{
    for (const auto& comment : comments_)
    {
        if (!comment.mentions.empty())
        {
            return true;
        }
    }
    return false;
}

auto CommentObject::all_mentions() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& comment : comments_)
    {
        for (const auto& mention : comment.mentions)
        {
            if (std::find(result.begin(), result.end(), mention) == result.end())
            {
                result.push_back(mention);
            }
        }
    }
    return result;
}

auto CommentObject::comment_by_id(const std::string& comment_id) const -> const Comment*
{
    auto iter = std::find_if(comments_.begin(),
                             comments_.end(),
                             [&comment_id](const Comment& comment)
                             { return comment.comment_id == comment_id; });
    if (iter == comments_.end())
    {
        return nullptr;
    }
    return &(*iter);
}

auto CommentObject::from_json(const std::string& json) -> void
{
    // Simple JSON field extraction for Comment object properties.
    auto extract_string = [&](const std::string& key) -> std::string
    {
        const auto key_pos = json.find("\"" + key + "\"");
        if (key_pos == std::string::npos) return "";
        const auto colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) return "";
        const auto quote_start = json.find('"', colon_pos + 1);
        if (quote_start == std::string::npos) return "";
        const auto quote_end = json.find('"', quote_start + 1);
        if (quote_end == std::string::npos) return "";
        return json.substr(quote_start + 1, quote_end - quote_start - 1);
    };
    auto extract_bool = [&](const std::string& key) -> std::pair<bool, bool>
    {
        const auto key_pos = json.find("\"" + key + "\"");
        if (key_pos == std::string::npos) return {false, false};
        const auto colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) return {false, false};
        auto val_start = colon_pos + 1;
        while (val_start < json.size() && json[val_start] == ' ')
        {
            ++val_start;
        }
        if (json.substr(val_start, 4) == "true") return {true, true};
        if (json.substr(val_start, 5) == "false") return {true, false};
        return {false, false};
    };

    auto author_id_val = extract_string("author_id");
    if (!author_id_val.empty()) author_id_ = author_id_val;

    auto author_name_val = extract_string("author_name");
    if (!author_name_val.empty()) author_name_ = author_name_val;

    auto body = extract_string("body");
    if (!body.empty())
    {
        Comment root;
        root.body = body;
        root.author_id = author_id_;
        root.author_name = author_name_;
        root.comment_id = extract_string("thread_id");
        root.created_at = extract_string("timestamp");
        if (root.comment_id.empty()) root.comment_id = "root";
        comments_.push_back(std::move(root));
    }

    auto [resolved_found, resolved_val] = extract_bool("resolved");
    if (resolved_found) resolved_ = resolved_val;

    auto [pinned_found, pinned_val] = extract_bool("pinned");
    if (pinned_found) pinned_ = pinned_val;
}

} // namespace markamp::canvas
