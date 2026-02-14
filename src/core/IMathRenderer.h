#pragma once

#include <string>
#include <string_view>

namespace markamp::core
{

/// Interface for LaTeX math rendering.
/// Converts LaTeX math expressions to HTML output.
class IMathRenderer
{
public:
    virtual ~IMathRenderer() = default;

    /// Render a LaTeX math expression to HTML.
    /// @param latex The LaTeX source (without delimiters)
    /// @param is_display True for display (block) math, false for inline math
    /// @return HTML string representing the rendered math
    [[nodiscard]] virtual auto render(std::string_view latex, bool is_display) -> std::string = 0;

    /// Whether this renderer is available and ready to use.
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

} // namespace markamp::core
