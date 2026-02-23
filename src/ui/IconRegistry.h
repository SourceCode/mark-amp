#pragma once

#include "ui/SvgDocument.h"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Central registry that maps icon names to their parsed SVG definitions.
/// Used to store the available icons for the application globally.
class IconRegistry
{
public:
    IconRegistry() = default;
    ~IconRegistry() = default;

    /// Registers an icon from raw SVG XML string data.
    /// Returns true if successfully parsed and registered.
    auto register_icon(const std::string& name, const std::string& svg_data) -> bool;

    /// Registers an icon by loading it from a file on disk.
    /// Returns true if successfully loaded, parsed, and registered.
    auto load_icon(const std::string& name, const std::filesystem::path& path) -> bool;

    /// Retrieves a parsed SvgDocument for the given icon name, if registered.
    [[nodiscard]] auto get_icon(const std::string& name) const -> std::optional<SvgDocument>;

    /// Checks if an icon is registered under the given name.
    [[nodiscard]] auto has_icon(const std::string& name) const -> bool;

    /// Returns a sorted list of all registered icon names.
    [[nodiscard]] auto get_icon_names() const -> std::vector<std::string>;

    /// Clears all registered icons.
    auto clear() -> void;

    /// Returns the number of registered icons.
    [[nodiscard]] auto size() const -> size_t;

private:
    std::unordered_map<std::string, SvgDocument> icons_;
};

} // namespace markamp::ui
