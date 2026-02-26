#pragma once

#include <string>
#include <vector>

namespace markamp::ui
{

/**
 * @brief Represents a single decoration item to be rendered in a Scintilla margin.
 */
struct GutterDecoration
{
    int line{0};
    int margin_index{0};
    int marker_index{0};
    std::string tooltip;
};

/**
 * @brief Interface for providing custom logic to populate Scintilla margins.
 *
 * Implementations of this interface can be registered to project dynamic items
 * across the 5 allocated margins, like Breakpoints, Code Lenses, or Git diffs.
 */
class IGutterDecorationProvider
{
public:
    IGutterDecorationProvider() = default;
    virtual ~IGutterDecorationProvider() = default;
    IGutterDecorationProvider(const IGutterDecorationProvider&) = delete;
    auto operator=(const IGutterDecorationProvider&) -> IGutterDecorationProvider& = delete;
    IGutterDecorationProvider(IGutterDecorationProvider&&) = delete;
    auto operator=(IGutterDecorationProvider&&) -> IGutterDecorationProvider& = delete;

    /**
     * @brief The unique identifier for this provider.
     */
    [[nodiscard]] virtual auto GetProviderId() const -> std::string = 0;

    /**
     * @brief Update the provider with the latest buffer content.
     */
    virtual void UpdateContent(const std::string& content) = 0;

    /**
     * @brief Build and return the set of decorations to render.
     */
    [[nodiscard]] virtual auto GetDecorations() const -> std::vector<GutterDecoration> = 0;
};

} // namespace markamp::ui
