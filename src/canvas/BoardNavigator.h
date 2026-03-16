#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

/// Metadata about a board file discovered on the filesystem.
struct BoardInfo
{
    std::string id;
    std::string name;
    std::filesystem::path path;
    std::chrono::system_clock::time_point modified_at; // (#31)

    /// Whether a filesystem path is set.
    [[nodiscard]] auto has_path() const noexcept -> bool
    {
        return !path.empty();
    }
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

    // ── Improvements (#31-32) ─────────────────────────────────

    /// Return the N most-recently-modified boards.
    [[nodiscard]] auto recent_boards(size_t max_count) const -> std::vector<const BoardInfo*>;

    /// Search boards by name (case-insensitive substring).
    [[nodiscard]] auto search_boards(const std::string& query) const
        -> std::vector<const BoardInfo*>;

    // ── Batch 8 (#47-48) ──────────────────────────────────────────

    /// Mark/unmark a board as favorite.
    auto set_favorite(const std::string& board_id, bool is_favorite) -> void;
    [[nodiscard]] auto is_favorite(const std::string& board_id) const -> bool;
    [[nodiscard]] auto favorite_boards() const -> std::vector<const BoardInfo*>;

    /// Sort boards alphabetically by name.
    auto sort_boards_by_name() -> void;

    /// (#97) Return the total number of discovered boards.
    [[nodiscard]] auto board_count() const -> std::size_t;

    /// Whether any boards have been discovered.
    [[nodiscard]] auto has_boards() const noexcept -> bool
    {
        return !boards_.empty();
    }

    /// Whether any boards are marked as favorites.
    [[nodiscard]] auto has_favorites() const noexcept -> bool
    {
        return !favorite_ids_.empty();
    }

    /// Whether an open callback is registered.
    [[nodiscard]] auto has_open_callback() const noexcept -> bool
    {
        return on_board_open_ != nullptr;
    }

private:
    std::vector<BoardInfo> boards_;
    OnBoardOpenCallback on_board_open_;
    std::unordered_set<std::string> favorite_ids_;
};

} // namespace markamp::canvas
