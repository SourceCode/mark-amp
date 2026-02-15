#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>

namespace markamp::canvas
{

/// An SVG icon placed on the canvas with configurable color and size.
class IconObject : public CanvasObject
{
public:
    IconObject();

    [[nodiscard]] auto icon_id() const -> const std::string&;
    auto set_icon_id(const std::string& icon_identifier) -> void;

    [[nodiscard]] auto svg_content() const -> const std::string&;
    auto set_svg_content(const std::string& svg) -> void;

    [[nodiscard]] auto icon_color() const -> const CanvasColor&;
    auto set_icon_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto icon_size() const -> double;
    auto set_icon_size(double size) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string icon_id_;
    std::string svg_content_;
    CanvasColor icon_color_{0, 0, 0, 255};
    double icon_size_{48.0};
};

} // namespace markamp::canvas
