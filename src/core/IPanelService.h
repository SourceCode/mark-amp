#pragma once

#include <string>

namespace markamp::core
{

/**
 * @brief Extension API for contributing new panels to the bottom area.
 */
class IPanelService
{
public:
    virtual ~IPanelService() = default;

    /**
     * @brief Registers a new panel area tab (contribution).
     *
     * @param id The unique identifier for the panel.
     * @param title The human-readable title.
     * @param icon_name The icon name or character code.
     */
    virtual void register_panel(const std::string& id,
                                const std::string& title,
                                const std::string& icon_name) = 0;

    /**
     * @brief Unregisters a previously contributed panel.
     *
     * @param id The unique identifier.
     */
    virtual void unregister_panel(const std::string& id) = 0;

    /**
     * @brief Sets the currently active panel.
     *
     * @param id The unique identifier.
     */
    virtual void set_active_panel(const std::string& id) = 0;

    /**
     * @brief Gets the currently active panel ID.
     */
    [[nodiscard]] virtual auto get_active_panel() const -> std::string = 0;
};

} // namespace markamp::core
