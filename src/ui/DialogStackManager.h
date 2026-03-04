#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 44: Button role in a dialog.
enum class DialogButtonRole : uint8_t
{
    kAccept,      ///< OK / Save / Yes
    kReject,      ///< Cancel / No
    kDestructive, ///< Delete / Remove
    kHelp,
};

/// Phase 44: Dialog button descriptor.
struct DialogButton
{
    std::string label;
    DialogButtonRole role{DialogButtonRole::kAccept};
    bool is_default{false};
};

/// Phase 44: Wizard step descriptor.
struct WizardStep
{
    std::string title;
    std::string description;
    bool is_complete{false};
    bool is_optional{false};
};

/// Phase 44: Testable model for dialog stack management.
///
/// Manages z-ordering and focus routing for multiple simultaneous dialogs.
class DialogStackModel
{
public:
    struct DialogEntry
    {
        uint64_t id{0};
        std::string title;
        bool is_modal{true};
        int z_order{0};
    };

    /// Push a new dialog onto the stack. Returns assigned ID.
    auto push(const std::string& title, bool is_modal = true) -> uint64_t;

    /// Pop the top dialog.
    void pop();

    /// Remove a specific dialog by ID.
    void remove(uint64_t dialog_id);

    /// Get the top (focused) dialog ID.
    [[nodiscard]] auto top_id() const -> uint64_t;

    /// Get all active dialogs (bottom to top).
    [[nodiscard]] auto stack() const -> const std::vector<DialogEntry>&;

    /// Is the stack empty?
    [[nodiscard]] auto is_empty() const -> bool;

    /// Get stack depth.
    [[nodiscard]] auto depth() const -> int;

    /// Bring a dialog to top.
    void bring_to_top(uint64_t dialog_id);

private:
    std::vector<DialogEntry> stack_;
    uint64_t next_id_{1};
    int next_z_order_{0};
};

/// Phase 44: Testable model for wizard state.
class WizardModel
{
public:
    void set_steps(std::vector<WizardStep> steps);
    [[nodiscard]] auto steps() const -> const std::vector<WizardStep>&;

    [[nodiscard]] auto current_step() const -> int;
    [[nodiscard]] auto current_step_info() const -> const WizardStep*;
    [[nodiscard]] auto step_count() const -> int;

    auto next() -> bool;
    auto previous() -> bool;
    void go_to_step(int index);

    void mark_step_complete(int index);
    [[nodiscard]] auto can_proceed() const -> bool;
    [[nodiscard]] auto is_last_step() const -> bool;
    [[nodiscard]] auto is_first_step() const -> bool;

    /// Overall progress [0.0, 1.0].
    [[nodiscard]] auto progress() const -> double;

private:
    std::vector<WizardStep> steps_;
    int current_{0};
};

/// Phase 44: Testable model for input dialog validation.
class InputDialogModel
{
public:
    void set_value(const std::string& value);
    [[nodiscard]] auto value() const -> const std::string&;

    void set_prompt(const std::string& prompt);
    [[nodiscard]] auto prompt() const -> const std::string&;

    using Validator = std::function<std::string(const std::string&)>;
    void set_validator(Validator validator);
    [[nodiscard]] auto validate() const -> std::string;
    [[nodiscard]] auto is_valid() const -> bool;

private:
    std::string value_;
    std::string prompt_;
    Validator validator_;
};

} // namespace markamp::ui
