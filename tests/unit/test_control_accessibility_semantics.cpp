/**
 * @file test_control_accessibility_semantics.cpp
 * @brief Phase 35: Tests for AccessibilitySemanticsController and ScreenReaderAnnouncer.
 */

#include "ui/AccessibilitySemanticsController.h"
#include "ui/ScreenReaderAnnouncer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// ControlSemantics
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlSemantics - role name", "[accessibility][semantics]")
{
    ControlSemantics sem;
    sem.role = AccessibleRole::kButton;
    CHECK(sem.role_name() == "button");

    sem.role = AccessibleRole::kTreeItem;
    CHECK(sem.role_name() == "treeitem");

    sem.role = AccessibleRole::kProgressBar;
    CHECK(sem.role_name() == "progressbar");
}

TEST_CASE("ControlSemantics - state flags", "[accessibility][semantics]")
{
    ControlSemantics sem;
    sem.state_flags = static_cast<uint8_t>(AccessibleState::kDisabled) |
                      static_cast<uint8_t>(AccessibleState::kFocused);

    CHECK(sem.has_state(AccessibleState::kDisabled));
    CHECK(sem.has_state(AccessibleState::kFocused));
    CHECK_FALSE(sem.has_state(AccessibleState::kSelected));
    CHECK_FALSE(sem.has_state(AccessibleState::kExpanded));
}

// ═══════════════════════════════════════════════════════
// AccessibilitySemanticsController
// ═══════════════════════════════════════════════════════

TEST_CASE("AccessibilitySemanticsController - empty state", "[accessibility][controller]")
{
    AccessibilitySemanticsController ctrl;
    CHECK(ctrl.control_count() == 0);
    CHECK_FALSE(ctrl.is_high_contrast());
    CHECK_FALSE(ctrl.is_reduced_complexity());
    CHECK(ctrl.is_focus_visible());
}

TEST_CASE("AccessibilitySemanticsController - register and query", "[accessibility][controller]")
{
    AccessibilitySemanticsController ctrl;

    ControlSemantics sem;
    sem.control_id = "btn_save";
    sem.accessible_name = "Save";
    sem.role = AccessibleRole::kButton;

    ctrl.register_control(sem);
    CHECK(ctrl.control_count() == 1);

    const auto* found = ctrl.semantics_for("btn_save");
    REQUIRE(found != nullptr);
    CHECK(found->accessible_name == "Save");
    CHECK(found->role == AccessibleRole::kButton);
}

TEST_CASE("AccessibilitySemanticsController - update state", "[accessibility][controller]")
{
    AccessibilitySemanticsController ctrl;

    ControlSemantics sem;
    sem.control_id = "chk_option";
    sem.role = AccessibleRole::kCheckbox;
    ctrl.register_control(sem);

    ctrl.update_state("chk_option", static_cast<uint8_t>(AccessibleState::kChecked));
    const auto* found = ctrl.semantics_for("chk_option");
    REQUIRE(found != nullptr);
    CHECK(found->has_state(AccessibleState::kChecked));
}

TEST_CASE("AccessibilitySemanticsController - remove control", "[accessibility][controller]")
{
    AccessibilitySemanticsController ctrl;

    ControlSemantics sem;
    sem.control_id = "btn_delete";
    sem.role = AccessibleRole::kButton;
    ctrl.register_control(sem);
    CHECK(ctrl.control_count() == 1);

    ctrl.remove_control("btn_delete");
    CHECK(ctrl.control_count() == 0);
    CHECK(ctrl.semantics_for("btn_delete") == nullptr);
}

TEST_CASE("AccessibilitySemanticsController - modes", "[accessibility][controller]")
{
    AccessibilitySemanticsController ctrl;

    ctrl.set_high_contrast(true);
    CHECK(ctrl.is_high_contrast());

    ctrl.set_reduced_complexity(true);
    CHECK(ctrl.is_reduced_complexity());

    ctrl.set_focus_visible(false);
    CHECK_FALSE(ctrl.is_focus_visible());
}

TEST_CASE("AccessibilitySemanticsController - contrast check", "[accessibility][contrast]")
{
    // Black on white: (1.0 + 0.05) / (0.0 + 0.05) = 21:1
    auto result = AccessibilitySemanticsController::check_contrast(0.0, 1.0);
    CHECK(result.ratio > 20.0);
    CHECK(result.meets_aa);
    CHECK(result.meets_aaa);

    // Similar grays: (0.5 + 0.05) / (0.4 + 0.05) = 1.22:1
    auto low = AccessibilitySemanticsController::check_contrast(0.5, 0.4);
    CHECK(low.ratio < 2.0);
    CHECK_FALSE(low.meets_aa);
    CHECK_FALSE(low.meets_aaa);

    // Borderline AA: ratio ~4.5
    auto borderline = AccessibilitySemanticsController::check_contrast(0.0, 0.1825);
    CHECK(borderline.meets_aa);
}

// ═══════════════════════════════════════════════════════
// Announcement
// ═══════════════════════════════════════════════════════

TEST_CASE("Announcement - priority name", "[accessibility][announcer]")
{
    Announcement ann;
    ann.priority = AnnouncementPriority::kPolite;
    CHECK(ann.priority_name() == "polite");

    ann.priority = AnnouncementPriority::kAssertive;
    CHECK(ann.priority_name() == "assertive");
}

// ═══════════════════════════════════════════════════════
// ScreenReaderAnnouncer
// ═══════════════════════════════════════════════════════

TEST_CASE("ScreenReaderAnnouncer - empty state", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    CHECK(announcer.pending_count() == 0);
    CHECK(announcer.total_announced() == 0);
    CHECK(announcer.is_dedup_enabled());
}

TEST_CASE("ScreenReaderAnnouncer - polite announce", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("File saved");
    CHECK(announcer.pending_count() == 1);
    CHECK(announcer.pending()[0].message == "File saved");
    CHECK(announcer.pending()[0].priority == AnnouncementPriority::kPolite);
}

TEST_CASE("ScreenReaderAnnouncer - assertive goes to front", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("Info message");
    announcer.announce_assertive("Error occurred");

    CHECK(announcer.pending_count() == 2);
    CHECK(announcer.pending()[0].message == "Error occurred");
    CHECK(announcer.pending()[1].message == "Info message");
}

TEST_CASE("ScreenReaderAnnouncer - pop next", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("First");
    announcer.announce("Second");

    auto first = announcer.pop_next();
    CHECK(first.message == "First");
    CHECK(announcer.pending_count() == 1);
    CHECK(announcer.total_announced() == 1);
}

TEST_CASE("ScreenReaderAnnouncer - dedup consecutive", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("Saved");
    announcer.announce("Saved"); // Deduped
    announcer.announce("Saved"); // Deduped

    CHECK(announcer.pending_count() == 1);
}

TEST_CASE("ScreenReaderAnnouncer - dedup disabled", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.set_dedup_enabled(false);
    announcer.announce("Saved");
    announcer.announce("Saved");

    CHECK(announcer.pending_count() == 2);
}

TEST_CASE("ScreenReaderAnnouncer - history", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("A");
    announcer.announce("B");
    announcer.announce("C");

    announcer.pop_next();
    announcer.pop_next();

    auto hist = announcer.history(5);
    CHECK(hist.size() == 2);
    CHECK(hist[0].message == "A");
    CHECK(hist[1].message == "B");
    CHECK(announcer.total_announced() == 2);
}

TEST_CASE("ScreenReaderAnnouncer - throttle config", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    CHECK(announcer.throttle_ms() == 0);

    announcer.set_throttle_ms(200);
    CHECK(announcer.throttle_ms() == 200);
}

TEST_CASE("ScreenReaderAnnouncer - clear", "[accessibility][announcer]")
{
    ScreenReaderAnnouncer announcer;
    announcer.announce("A");
    announcer.announce("B");
    announcer.clear();
    CHECK(announcer.pending_count() == 0);
}
