// ============================================================================
// File: src/canvas/CanvasAccessibility.h
// Phase 11: Canvas Workbench Shell — accessibility and delete confirmation
// ============================================================================
#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

// ── Delete Confirmation ────────────────────────────────────────────

/// Configuration for the delete confirmation dialog.
struct DeleteConfirmationConfig
{
    size_t threshold{5};       ///< Confirm if deleting >= threshold objects
    bool confirm_locked{true}; ///< Always confirm if selection contains locked objects
    bool confirm_groups{true}; ///< Confirm if deleting a group with children
    bool enabled{true};        ///< Master toggle
};

/// Result of a delete confirmation check.
enum class DeleteConfirmation : uint8_t
{
    kNoConfirmNeeded, ///< Proceed without asking
    kConfirmMultiple, ///< Deleting many objects
    kConfirmLocked,   ///< Deleting locked objects
    kConfirmGroup,    ///< Deleting a group
    kCancelled        ///< User cancelled
};

/// Information for the delete confirmation dialog.
struct DeleteConfirmationInfo
{
    DeleteConfirmation reason{DeleteConfirmation::kNoConfirmNeeded};
    size_t object_count{0};
    size_t locked_count{0};
    size_t group_count{0};
    std::string message; ///< Human-readable explanation
};

// ── Accessibility ──────────────────────────────────────────────────

/// Announcement priority for screen readers.
enum class AnnouncementPriority : uint8_t
{
    kPolite,   ///< Wait for idle (aria-live="polite")
    kAssertive ///< Interrupt immediately (aria-live="assertive")
};

/// A screen reader announcement.
struct AccessibilityAnnouncement
{
    std::string message;
    AnnouncementPriority priority{AnnouncementPriority::kPolite};
};

/// Canvas accessibility service.
///
/// Provides:
///   - Screen reader announcements for canvas operations
///   - Keyboard focus ring tracking
///   - Object descriptions for assistive technology
///   - Delete confirmation logic
class CanvasAccessibility
{
public:
    CanvasAccessibility();
    explicit CanvasAccessibility(const DeleteConfirmationConfig& delete_config);

    // ── Screen Reader Announcements ────────────────────────────────

    /// Announce an object selection.
    [[nodiscard]] auto announce_selection(const CanvasObject& obj) -> AccessibilityAnnouncement;

    /// Announce multiple objects selected.
    [[nodiscard]] auto announce_multi_selection(size_t count) -> AccessibilityAnnouncement;

    /// Announce object creation.
    [[nodiscard]] auto announce_creation(const CanvasObject& obj) -> AccessibilityAnnouncement;

    /// Announce object deletion.
    [[nodiscard]] auto announce_deletion(size_t count) -> AccessibilityAnnouncement;

    /// Announce a tool change.
    [[nodiscard]] auto announce_tool_change(const std::string& tool_name)
        -> AccessibilityAnnouncement;

    /// Announce a zoom level change.
    [[nodiscard]] auto announce_zoom(double zoom_percent) -> AccessibilityAnnouncement;

    /// Announce a custom message.
    [[nodiscard]] static auto
    announce(const std::string& message,
             AnnouncementPriority priority = AnnouncementPriority::kPolite)
        -> AccessibilityAnnouncement;

    // ── Object Description ─────────────────────────────────────────

    /// Generate an accessible description of an object.
    [[nodiscard]] static auto describe_object(const CanvasObject& obj) -> std::string;

    /// Generate a short label for an object (e.g. "Sticky Note at 100,200").
    [[nodiscard]] static auto object_label(const CanvasObject& obj) -> std::string;

    // ── Keyboard Focus Ring ────────────────────────────────────────

    /// Set the currently keyboard-focused object.
    auto set_focused_object(ObjectId obj_id) -> void;

    /// Get the currently focused object.
    [[nodiscard]] auto focused_object() const -> ObjectId;

    /// Move focus to the next/previous object in tab order.
    auto focus_next(const std::vector<ObjectId>& tab_order) -> ObjectId;
    auto focus_previous(const std::vector<ObjectId>& tab_order) -> ObjectId;

    /// Clear keyboard focus.
    auto clear_focus() -> void;

    // ── Delete Confirmation ────────────────────────────────────────

    /// Check whether deletion of the given objects requires confirmation.
    [[nodiscard]] auto
    check_delete_confirmation(const std::vector<const CanvasObject*>& objects) const
        -> DeleteConfirmationInfo;

    /// Get/set the delete confirmation configuration.
    [[nodiscard]] auto delete_config() const -> const DeleteConfirmationConfig&;
    auto set_delete_config(const DeleteConfirmationConfig& config) -> void;

    // ── Announcement History ───────────────────────────────────────

    /// Get the last N announcements (for debugging / testing).
    [[nodiscard]] auto recent_announcements() const
        -> const std::vector<AccessibilityAnnouncement>&;

    /// Maximum number of announcements to keep in history.
    static constexpr size_t kMaxAnnouncementHistory = 50;

private:
    DeleteConfirmationConfig delete_config_;
    ObjectId focused_id_{kInvalidObjectId};
    std::vector<AccessibilityAnnouncement> announcement_history_;

    auto record_announcement(const AccessibilityAnnouncement& ann) -> void;
};

} // namespace markamp::canvas
