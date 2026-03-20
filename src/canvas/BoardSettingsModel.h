#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Board permission level.
enum class BoardPermission : uint8_t
{
    kOwner,
    kEditor,
    kCommenter,
    kViewer,
};

/// Board-level settings model (Phase 01 W17).
///
/// Encapsulates:
/// - Board background preset selection
/// - Default grid spacing
/// - Permission level
/// - Board dimensions (width × height)
/// - Custom settings key/value pairs
class BoardSettingsModel
{
public:
    // ── Background ──────────────────────────────────────────────────

    void set_background(const std::string& preset);
    [[nodiscard]] auto background() const -> const std::string&;

    // ── Grid spacing ────────────────────────────────────────────────

    void set_grid_spacing(double spacing);
    [[nodiscard]] auto grid_spacing() const -> double;

    // ── Permissions ─────────────────────────────────────────────────

    void set_permission(BoardPermission perm);
    [[nodiscard]] auto permission() const -> BoardPermission;

    // ── Dimensions ──────────────────────────────────────────────────

    void set_dimensions(double width, double height);
    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;

    // ── Custom settings ─────────────────────────────────────────────

    void set_setting(const std::string& key, const std::string& value);
    [[nodiscard]] auto setting(const std::string& key) const -> std::string;
    [[nodiscard]] auto setting_count() const -> int;

    // ── Convenience ─────────────────────────────────────────────────

    [[nodiscard]] auto is_owner() const noexcept -> bool
    {
        return permission_ == BoardPermission::kOwner;
    }

    [[nodiscard]] auto is_read_only() const noexcept -> bool
    {
        return permission_ == BoardPermission::kViewer;
    }

    [[nodiscard]] auto has_background() const noexcept -> bool
    {
        return !background_.empty();
    }

    [[nodiscard]] auto area() const noexcept -> double
    {
        return width_ * height_;
    }

private:
    std::string background_{"white"};
    double grid_spacing_{20.0};
    BoardPermission permission_{BoardPermission::kOwner};
    double width_{4096.0};
    double height_{4096.0};
    std::vector<std::pair<std::string, std::string>> settings_;
};

} // namespace markamp::canvas
