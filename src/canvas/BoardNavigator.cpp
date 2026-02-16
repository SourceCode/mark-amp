#include "BoardNavigator.h"

#include <algorithm>
#include <cctype>

namespace markamp::canvas
{

auto BoardNavigator::scan_boards(const std::filesystem::path& directory) -> void
{
    boards_.clear();

    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        if (entry.path().extension() != ".markboard")
        {
            continue;
        }

        BoardInfo info;
        info.id = entry.path().stem().string();
        info.name = entry.path().stem().string();
        info.path = entry.path();
        info.modified_at =
            std::chrono::system_clock::now(); // Use filesystem last_write_time if available
        boards_.push_back(std::move(info));
    }

    // Sort alphabetically by name for consistent ordering.
    std::sort(boards_.begin(),
              boards_.end(),
              [](const BoardInfo& lhs, const BoardInfo& rhs) { return lhs.name < rhs.name; });
}

auto BoardNavigator::all_boards() const -> const std::vector<BoardInfo>&
{
    return boards_;
}

auto BoardNavigator::find_board(const std::string& board_id) const -> const BoardInfo*
{
    for (const auto& board : boards_)
    {
        if (board.id == board_id)
        {
            return &board;
        }
    }
    return nullptr;
}

auto BoardNavigator::navigate_to(const std::string& board_id) -> void
{
    if (on_board_open_)
    {
        on_board_open_(board_id);
    }
}

auto BoardNavigator::set_on_board_open(OnBoardOpenCallback callback) -> void
{
    on_board_open_ = std::move(callback);
}

// ── Improvements (#31-32) ───────────────────────────────────────

auto BoardNavigator::recent_boards(size_t max_count) const -> std::vector<const BoardInfo*>
{
    // Build a sorted copy by modified_at descending.
    std::vector<const BoardInfo*> sorted;
    sorted.reserve(boards_.size());
    for (const auto& board_info : boards_)
    {
        sorted.push_back(&board_info);
    }
    std::sort(sorted.begin(),
              sorted.end(),
              [](const BoardInfo* lhs, const BoardInfo* rhs)
              { return lhs->modified_at > rhs->modified_at; });

    if (sorted.size() > max_count)
    {
        sorted.resize(max_count);
    }
    return sorted;
}

auto BoardNavigator::search_boards(const std::string& query) const -> std::vector<const BoardInfo*>
{
    std::vector<const BoardInfo*> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& board_info : boards_)
    {
        std::string lower_name = board_info.name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        if (lower_name.find(lower_query) != std::string::npos)
        {
            result.push_back(&board_info);
        }
    }
    return result;
}

// ── Batch 8 (#47-48) ──────────────────────────────────────────────

auto BoardNavigator::set_favorite(const std::string& board_id, bool is_fav) -> void
{
    if (is_fav)
    {
        favorite_ids_.insert(board_id);
    }
    else
    {
        favorite_ids_.erase(board_id);
    }
}

auto BoardNavigator::is_favorite(const std::string& board_id) const -> bool
{
    return favorite_ids_.count(board_id) > 0;
}

auto BoardNavigator::favorite_boards() const -> std::vector<const BoardInfo*>
{
    std::vector<const BoardInfo*> result;
    for (const auto& board_info : boards_)
    {
        if (favorite_ids_.count(board_info.id) > 0)
        {
            result.push_back(&board_info);
        }
    }
    return result;
}

auto BoardNavigator::sort_boards_by_name() -> void
{
    std::sort(boards_.begin(),
              boards_.end(),
              [](const BoardInfo& lhs, const BoardInfo& rhs) { return lhs.name < rhs.name; });
}

} // namespace markamp::canvas
