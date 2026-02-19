#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Accessible object entry for logical traversal.
struct AccessibleObject
{
    std::string object_id;
    std::string readable_name;
    std::string object_type;
    int tab_order{0};
};

/// Testable model for Canvas Accessibility (Phase 73).
///
/// Encapsulates:
/// - Logical tab-order traversal list
/// - Focus ring visibility settings
/// - Assistive labels for selected objects
/// - Reduced-motion mode flag
class AccessibilityModel
{
public:
    // ── Traversal ───────────────────────────────────────────────────

    void set_objects(std::vector<AccessibleObject> objects);
    [[nodiscard]] auto objects() const -> const std::vector<AccessibleObject>&;
    [[nodiscard]] auto object_count() const -> int;

    void focus_next();
    void focus_prev();
    [[nodiscard]] auto focused_index() const -> int;
    [[nodiscard]] auto focused_object() const -> AccessibleObject;

    // ── Focus ring ──────────────────────────────────────────────────

    void set_focus_ring_visible(bool visible);
    [[nodiscard]] auto focus_ring_visible() const -> bool;

    void set_focus_ring_contrast(double contrast);
    [[nodiscard]] auto focus_ring_contrast() const -> double;

    // ── Reduced motion ──────────────────────────────────────────────

    void set_reduced_motion(bool enabled);
    [[nodiscard]] auto reduced_motion() const -> bool;

private:
    std::vector<AccessibleObject> objects_;
    int focused_index_{0};
    bool focus_ring_visible_{true};
    double focus_ring_contrast_{1.0};
    bool reduced_motion_{false};
};

} // namespace markamp::canvas
