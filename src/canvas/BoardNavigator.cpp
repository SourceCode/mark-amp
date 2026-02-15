#include "BoardNavigator.h"

#include <algorithm>

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

} // namespace markamp::canvas
