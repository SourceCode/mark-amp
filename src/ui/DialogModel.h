#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Dialog severity level (Phase 19 Task 2).
enum class DialogSeverity : uint8_t
{
    kInfo,
    kWarning,
    kDanger, ///< Destructive actions
};

/// Standard button role for dialog (Phase 19 Task 1).
enum class DialogButtonRole : uint8_t
{
    kPrimary,   ///< Confirm/accept action
    kSecondary, ///< Alternative action
    kCancel,    ///< Dismiss/cancel
};

/// A dialog button definition.
struct DialogButton
{
    std::string label;
    DialogButtonRole role{DialogButtonRole::kCancel};
    bool is_default{false};     ///< Focused on open
    bool is_destructive{false}; ///< Renders with danger styling
};

/// Testable model for dialogs and confirmations (Phase 19).
///
/// Encapsulates:
/// - Button ordering conventions (Cancel last on macOS)
/// - Severity-to-styling mapping
/// - "Undo instead of confirm" policy decisions
/// - "Do not ask again" remembered choice management
class DialogModel
{
public:
    /// Set dialog severity.
    void set_severity(DialogSeverity severity);

    /// Set dialog title and message.
    void set_content(const std::string& title, const std::string& message);

    /// Set buttons.
    void set_buttons(std::vector<DialogButton> buttons);

    // ── Queries ─────────────────────────────────────────────────────

    [[nodiscard]] auto severity() const -> DialogSeverity;
    [[nodiscard]] auto title() const -> const std::string&;
    [[nodiscard]] auto message() const -> const std::string&;

    /// Get buttons in platform-correct order (Cancel rightmost on macOS).
    [[nodiscard]] auto ordered_buttons() const -> std::vector<DialogButton>;

    /// Get the default (focused) button index.
    [[nodiscard]] auto default_button_index() const -> int;

    // ── Undo-instead-of-confirm ─────────────────────────────────────

    /// Should this action use undo toast instead of confirmation dialog?
    [[nodiscard]] static auto prefer_undo(DialogSeverity severity) -> bool;

    // ── Remembered choices ──────────────────────────────────────────

    /// Remember a choice for a dialog key.
    void remember_choice(const std::string& dialog_key, int button_index);

    /// Get remembered choice (returns -1 if not remembered).
    [[nodiscard]] auto remembered_choice(const std::string& dialog_key) const -> int;

    /// Has a remembered choice?
    [[nodiscard]] auto has_remembered(const std::string& dialog_key) const -> bool;

    /// Reset a specific remembered choice.
    void reset_remembered(const std::string& dialog_key);

    /// Reset all remembered choices.
    void reset_all_remembered();

private:
    DialogSeverity severity_{DialogSeverity::kInfo};
    std::string title_;
    std::string message_;
    std::vector<DialogButton> buttons_;

    struct RememberedChoice
    {
        std::string dialog_key;
        int button_index{0};
    };
    std::vector<RememberedChoice> remembered_;
};

} // namespace markamp::ui
