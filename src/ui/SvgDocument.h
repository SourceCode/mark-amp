#pragma once

#include <string>
#include <variant>
#include <vector>

namespace markamp::ui
{

/// Represents a single drawing command within an SVG <path d="..."> component.
struct SvgPathCommand
{
    char type; // 'M', 'm', 'L', 'l', 'H', 'h', 'V', 'v', 'C', 'c', 'S', 's', 'Q', 'q', 'T', 't',
               // 'A', 'a', 'Z', 'z'
    std::vector<float> args;
};

/// Parse an SVG 'd' path string into a sequence of commands.
auto ParseSvgPath(const std::string& d) -> std::vector<SvgPathCommand>;

/// Represents an SVG <path> element.
struct SvgPath
{
    std::string d;
    std::vector<SvgPathCommand> commands;
    std::string fill;
    std::string stroke;
    float stroke_width{0.0f};
};

/// Represents an SVG <circle> element.
struct SvgCircle
{
    float cx{0.0f};
    float cy{0.0f};
    float r{0.0f};
    std::string fill;
    std::string stroke;
    float stroke_width{0.0f};
};

/// Represents an SVG <rect> element.
struct SvgRect
{
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
    float rx{0.0f};
    float ry{0.0f};
    std::string fill;
    std::string stroke;
    float stroke_width{0.0f};
};

/// Represents an SVG <line> element.
struct SvgLine
{
    float x1{0.0f};
    float y1{0.0f};
    float x2{0.0f};
    float y2{0.0f};
    std::string stroke;
    float stroke_width{1.0f};
};

/// Represents an SVG <polyline> element.
struct SvgPolyline
{
    std::vector<float> points;
    std::string fill;
    std::string stroke;
    float stroke_width{1.0f};
};

/// A variant holding any supported SVG shape.
using SvgShape = std::variant<SvgPath, SvgCircle, SvgRect, SvgLine, SvgPolyline>;

/// Represents the viewBox attribute of an <svg> element.
struct SvgViewBox
{
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    [[nodiscard]] auto is_valid() const -> bool
    {
        return width > 0.0f && height > 0.0f;
    }
};

/// Represents a parsed, minimal SVG document suitable for icon rendering.
class SvgDocument
{
public:
    virtual ~SvgDocument() = default;

    /// Gets the viewBox of the generic SVG.
    [[nodiscard]] auto viewBox() const -> const SvgViewBox&
    {
        return viewBox_;
    }

    /// Sets the viewBox.
    auto set_viewBox(const SvgViewBox& vb) -> void
    {
        viewBox_ = vb;
    }

    /// Parses a raw SVG XML string. Returns true if valid geometry was found.
    auto parse(const std::string& xml) -> bool;

    /// Gets the shapes comprising the document.
    [[nodiscard]] auto shapes() const -> const std::vector<SvgShape>&
    {
        return shapes_;
    }

    /// Adds a shape to the document.
    auto add_shape(SvgShape shape) -> void
    {
        shapes_.push_back(std::move(shape));
    }

    /// Clears all shapes.
    auto clear() -> void
    {
        shapes_.clear();
    }

private:
    SvgViewBox viewBox_{};
    std::vector<SvgShape> shapes_;
};

} // namespace markamp::ui
