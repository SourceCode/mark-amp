#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Metadata about a board file discovered on the filesystem.
struct BoardInfo
{
    std::string id;
    std::string name;
    std::filesystem::path path;
};

/// Scans a workspace directory for board files and provides navigation
/// between boards via callback.
class BoardNavigator
{
public:
    using OnBoardOpenCallback = std::function<void(const std::string& board_id)>;

    /// Scan a directory for .markboard files.
    auto scan_boards(const std::filesystem::path& directory) -> void;

    [[nodiscard]] auto all_boards() const -> const std::vector<BoardInfo>&;
    [[nodiscard]] auto find_board(const std::string& board_id) const -> const BoardInfo*;

    auto navigate_to(const std::string& board_id) -> void;
    auto set_on_board_open(OnBoardOpenCallback callback) -> void;

private:
    std::vector<BoardInfo> boards_;
    OnBoardOpenCallback on_board_open_;
};

} // namespace markamp::canvas
