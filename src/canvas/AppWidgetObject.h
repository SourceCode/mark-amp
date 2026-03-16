#pragma once

/// @file AppWidgetObject.h
/// @brief V8 Phase 8: In-canvas widget host object.
///
/// Represents an embedded app widget on the canvas — data table cards,
/// issue tracker cards, custom dashboards, etc. Each widget is backed by
/// a provider extension and carries its own state, sync status, and
/// data-binding configuration.

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

/// Sync status of a widget's data connection.
enum class WidgetSyncStatus
{
    kIdle,
    kSyncing,
    kSynced,
    kError
};

/// Configuration for data binding between a widget and its provider.
struct WidgetDataBinding
{
    std::string field_name;  ///< Widget field to bind
    std::string source_path; ///< Provider data path (e.g. "issues[0].title")
    std::string transform;   ///< Optional transform expression

    // ── Round 2 Batch 5 (#49) ────────────────────────────────────

    /// (#49) Whether a transform expression is set.
    [[nodiscard]] auto has_transform() const noexcept -> bool
    {
        return !transform.empty();
    }
};

/// In-canvas widget host object.
class AppWidgetObject : public CanvasObject
{
public:
    AppWidgetObject();
    ~AppWidgetObject() override = default;

    // Non-copyable, movable (matches base)
    AppWidgetObject(const AppWidgetObject&) = delete;
    auto operator=(const AppWidgetObject&) -> AppWidgetObject& = delete;
    AppWidgetObject(AppWidgetObject&&) = default;
    auto operator=(AppWidgetObject&&) -> AppWidgetObject& = default;

    /// Return "app_widget" as the object type name.
    [[nodiscard]] auto type_name() const -> std::string;

    /// Bounding box based on configured widget dimensions.
    [[nodiscard]] auto local_bounds() const -> AABB override;

    /// Deep clone.
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

    // ── Widget identity ───────────────────────────────────────────

    [[nodiscard]] auto widget_id() const -> const std::string&
    {
        return widget_id_;
    }
    auto set_widget_id(const std::string& widget_id) -> void
    {
        widget_id_ = widget_id;
    }

    [[nodiscard]] auto app_id() const -> const std::string&
    {
        return app_id_;
    }
    auto set_app_id(const std::string& app_id) -> void
    {
        app_id_ = app_id;
    }

    [[nodiscard]] auto provider_id() const -> const std::string&
    {
        return provider_id_;
    }
    auto set_provider_id(const std::string& provider_id) -> void
    {
        provider_id_ = provider_id;
    }

    // ── Widget state ──────────────────────────────────────────────

    [[nodiscard]] auto state_json() const -> const std::string&
    {
        return state_json_;
    }
    auto set_state_json(const std::string& state_json) -> void
    {
        state_json_ = state_json;
    }

    [[nodiscard]] auto config_json() const -> const std::string&
    {
        return config_json_;
    }
    auto set_config_json(const std::string& config_json) -> void
    {
        config_json_ = config_json;
    }

    // ── Dimensions ────────────────────────────────────────────────

    [[nodiscard]] auto widget_width() const -> int
    {
        return width_;
    }
    [[nodiscard]] auto widget_height() const -> int
    {
        return height_;
    }
    auto set_widget_size(int width, int height) -> void;

    // ── Sync ──────────────────────────────────────────────────────

    [[nodiscard]] auto sync_status() const -> WidgetSyncStatus
    {
        return sync_status_;
    }
    auto set_sync_status(WidgetSyncStatus status) -> void
    {
        sync_status_ = status;
    }

    [[nodiscard]] auto last_sync_error() const -> const std::string&
    {
        return last_sync_error_;
    }
    auto set_last_sync_error(const std::string& error) -> void
    {
        last_sync_error_ = error;
    }

    // ── Data bindings ─────────────────────────────────────────────

    [[nodiscard]] auto bindings() const -> const std::vector<WidgetDataBinding>&
    {
        return bindings_;
    }
    auto add_binding(const WidgetDataBinding& binding) -> void;
    auto clear_bindings() -> void;

    // ── Serialization ─────────────────────────────────────────────

    /// Populate fields from a JSON string.
    auto from_json(const std::string& json) -> void override;

    // ── Round 2 Batch 5 (#41-48) ──────────────────────────────────

    /// (#41) Whether the widget is currently syncing.
    [[nodiscard]] auto is_syncing() const noexcept -> bool
    {
        return sync_status_ == WidgetSyncStatus::kSyncing;
    }

    /// (#42) Whether the widget is synced.
    [[nodiscard]] auto is_synced() const noexcept -> bool
    {
        return sync_status_ == WidgetSyncStatus::kSynced;
    }

    /// (#43) Whether the widget has a sync error.
    [[nodiscard]] auto has_error() const noexcept -> bool
    {
        return sync_status_ == WidgetSyncStatus::kError;
    }

    /// (#44) Whether state JSON is populated.
    [[nodiscard]] auto has_state() const noexcept -> bool
    {
        return !state_json_.empty();
    }

    /// (#45) Whether config JSON is populated.
    [[nodiscard]] auto has_config() const noexcept -> bool
    {
        return !config_json_.empty();
    }

    /// (#46) Number of data bindings.
    [[nodiscard]] auto binding_count() const noexcept -> size_t
    {
        return bindings_.size();
    }

    /// (#47) Whether the widget has data bindings.
    [[nodiscard]] auto has_bindings() const noexcept -> bool
    {
        return !bindings_.empty();
    }

    /// (#48) Widget area in pixels.
    [[nodiscard]] auto widget_area() const noexcept -> int
    {
        return width_ * height_;
    }

private:
    std::string widget_id_;
    std::string app_id_;
    std::string provider_id_;
    std::string state_json_;
    std::string config_json_;
    int width_{200};
    int height_{150};
    WidgetSyncStatus sync_status_{WidgetSyncStatus::kIdle};
    std::string last_sync_error_;
    std::vector<WidgetDataBinding> bindings_;
};

} // namespace markamp::canvas
