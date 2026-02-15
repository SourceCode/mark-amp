#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <optional>
#include <string>

namespace markamp::canvas
{

/// Describes which board (and optionally which frame/object within it) a
/// cross-board link points to.
struct BoardLinkTarget
{
    std::string board_id;
    std::string board_name;
    std::optional<ObjectId> frame_id;  // Optional: link to specific frame
    std::optional<ObjectId> object_id; // Optional: link to specific object
};

/// A canvas object that acts as a hyperlink to another board.
class CrossBoardLinkObject : public CanvasObject
{
public:
    CrossBoardLinkObject();

    [[nodiscard]] auto target() const -> const BoardLinkTarget&;
    auto set_target(const BoardLinkTarget& target) -> void;

    [[nodiscard]] auto display_text() const -> const std::string&;
    auto set_display_text(const std::string& text) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    BoardLinkTarget target_;
    std::string display_text_{"Link to board"};
    double width_{200.0};
    double height_{60.0};
};

} // namespace markamp::canvas
