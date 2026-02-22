# Phase 44 -- Dialog and Modal System

## Objective

Build a comprehensive dialog and modal system that replaces all native wxMessageBox and wxDialog usage with custom-drawn, theme-aware dialogs that match VSCode/WebStorm quality. The system must support confirmation dialogs with "don't ask again", file pickers, input dialogs with validation, progress dialogs with cancellation, multi-step wizards, modal backdrop with animation, dialog stacking, and full keyboard navigation. Build on the existing DialogModel data layer to create the rendering and interaction layer.

## Prerequisites

- Phase 43 complete (Notification System V2)
- `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.h` -- existing model with severity, buttons, remembered choices
- `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h` -- existing wizard implementation (4-page, wxSimplebook)
- Phase 41 components: ThemedButton, ThemedTextInput, ThemedCheckbox, ThemedProgressBar, ThemedDropdown

## VSCode / WebStorm Reference Behavior

- Modal backdrop: semi-transparent dark overlay (40% black) with 200ms fade-in
- Dialog: centered in window, white/dark background, 8px border-radius, subtle box-shadow
- Confirmation dialog: title, message, buttons (primary right-aligned), optional "Don't show again" checkbox
- Input dialog: single text input with label, validation, OK/Cancel buttons
- Progress dialog: progress bar, message, cancel button, elapsed time
- Save changes dialog: "Save", "Don't Save", "Cancel" -- matches platform convention (Cancel rightmost on macOS)
- Multi-step wizard: step indicator dots/breadcrumb, Back/Next/Finish buttons
- Dialog stacking: multiple dialogs stack with increasing z-index and backdrop opacity
- Keyboard: Tab through controls, Enter for primary, Escape for cancel

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ProgressDialog.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ProgressDialog.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/WizardDialog.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/WizardDialog.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogBackdrop.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogBackdrop.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/UnsavedChangesGuard.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/UnsavedChangesGuard.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_dialog_system.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Create DialogBackdrop with Fade Animation

**Description:** Implement a modal backdrop overlay that covers the entire main window with a semi-transparent dark fill, fading in when a dialog opens and fading out when it closes.

**Implementation Details:** DialogBackdrop is a wxPanel that covers the full MainFrame client area. It renders a filled rectangle with `wxColour(0, 0, 0, alpha)` where alpha animates from 0 to 102 (40% opacity) over 200ms using AnimationDriver. The backdrop intercepts all mouse events (preventing interaction with underlying content). It does not intercept keyboard events (those are routed to the dialog). The backdrop is created lazily when the first dialog opens and destroyed when the last dialog closes. Z-order: backdrop sits above all panels but below the dialog.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DialogBackdrop.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DialogBackdrop.cpp` (create)

**Acceptance Criteria:**
- Backdrop fades in over 200ms when dialog opens
- Backdrop fades out over 150ms when dialog closes
- Clicking the backdrop does not interact with content below
- Backdrop covers the entire main window client area
- Backdrop renders at 40% black opacity when fully visible

**Dependencies:** Phase 41 Task 19 (AnimationDriver)

---

### Task 02 -- Create ThemedDialog Base Class

**Description:** Create a base dialog class that all themed dialogs inherit from. Provides common rendering (rounded corners, shadow, title bar), theme integration, keyboard handling (Enter/Escape), and integration with DialogBackdrop.

**Implementation Details:** ThemedDialog extends wxDialog with custom rendering. Override OnPaint to draw: white/dark background with 8px border-radius using wxGraphicsContext::CreateRoundedRectangle(), 1px border in border_default color, and a box shadow (4px blur, 8px offset-y, 15% black). Title bar area: 48px height with title text (16px bold) and close X button (IconButton). Content area below title bar. Button bar at bottom (32px padding). ThemedDialog automatically shows DialogBackdrop on ShowModal() and hides it on EndModal(). Auto-centers in the parent window.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (create)

**Acceptance Criteria:**
- Dialog renders with rounded corners and box shadow
- Title bar shows title text and close button
- Enter key activates the default button
- Escape key closes the dialog (returns cancel)
- Dialog centers in parent window on show
- Theme switch updates dialog colors immediately

**Dependencies:** Task 01

---

### Task 03 -- Create DialogStackManager for Multiple Dialogs

**Description:** Implement a stack manager that handles multiple concurrent dialogs, increasing backdrop opacity for each stacked dialog and managing z-order.

**Implementation Details:** DialogStackManager is a singleton that maintains a `std::vector<ThemedDialog*>` stack. When a dialog is pushed, backdrop opacity increases by 15% (from 40% base to 55%, 70%, etc., capped at 85%). Each new dialog renders above the previous one. When a dialog is popped (closed), backdrop reduces by 15%. If the stack is empty, the backdrop is hidden. The manager provides `push(dialog)`, `pop()`, `top()`, and `is_empty()` methods. It also blocks focus from passing to dialogs below the top of the stack.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp` (create)

**Acceptance Criteria:**
- Multiple dialogs stack with increasing backdrop opacity
- Only the top dialog receives keyboard input
- Closing the top dialog reveals the dialog below
- Closing all dialogs removes the backdrop
- Backdrop opacity is capped at 85%

**Dependencies:** Task 01, Task 02

---

### Task 04 -- Implement ConfirmDialog

**Description:** Create a confirmation dialog with title, message, configurable buttons, severity-based icon, and optional "Don't ask again" checkbox.

**Implementation Details:** ConfirmDialog extends ThemedDialog. Layout: [severity icon 32px | title + message area]. Below message: optional "Don't ask again" checkbox (ThemedCheckbox). Button bar: buttons from DialogModel, rendered using ThemedButton with variants mapped from DialogButtonRole (Primary -> Primary variant, Secondary -> Secondary variant, Cancel -> Ghost variant, Destructive -> Danger variant). The dialog uses DialogModel's `ordered_buttons()` to get platform-correct button order. If "Don't ask again" is checked and user clicks a button, the choice is saved via `DialogModel::remember_choice()`. Before showing, check `has_remembered()` -- if true, return the remembered choice without showing the dialog.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` (create)

**Acceptance Criteria:**
- Dialog shows severity icon (info/warning/danger)
- Buttons render in platform-correct order
- "Don't ask again" checkbox saves the choice
- Remembered choices bypass the dialog entirely
- Enter activates default button, Escape activates cancel button

**Dependencies:** Task 02

---

### Task 05 -- Implement InputDialog with Validation

**Description:** Create an input dialog with a label, text input, real-time validation, and OK/Cancel buttons.

**Implementation Details:** InputDialog extends ThemedDialog. Layout: label text above a ThemedTextInput. The input supports a `validate` callback (`std::function<std::string(const std::string&)>`) that returns an error message (empty string = valid). Validation runs on every keystroke (debounced 300ms). When validation fails, the input shows error state and the OK button is disabled. The dialog can be configured with: placeholder text, initial value, max length, input type (text, password, number). Password mode hides input with dots. Number mode validates numeric input.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.cpp` (create)

**Acceptance Criteria:**
- Input dialog shows label and text input
- Real-time validation shows error messages
- OK button disables when validation fails
- Password mode masks input
- Enter submits (if valid), Escape cancels
- Initial value is pre-filled and selected

**Dependencies:** Task 02, Phase 41 Task 05 (ThemedTextInput)

---

### Task 06 -- Implement ProgressDialog with Cancel

**Description:** Create a progress dialog showing a progress bar, message, elapsed time, and optional cancel button.

**Implementation Details:** ProgressDialog extends ThemedDialog. Layout: message text, ThemedProgressBar (linear), elapsed time label ("Elapsed: 0:05"), and Cancel button. The dialog runs modally but allows the progress to be updated from a background thread via `SetProgress(double value, const std::string& message)` (thread-safe via `wxGetApp().CallAfter()`). Cancel button sets a `cancelled_` flag that the background task checks periodically. The dialog supports both determinate (0-100%) and indeterminate modes. Elapsed time updates every second via wxTimer.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ProgressDialog.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ProgressDialog.cpp` (create)

**Acceptance Criteria:**
- Progress bar updates smoothly during operation
- Elapsed time counts up correctly
- Cancel button sets cancelled flag (background task checks it)
- Indeterminate mode shows shimmer animation
- Thread-safe progress updates work without crashes

**Dependencies:** Task 02, Phase 41 Task 11 (ThemedProgressBar)

---

### Task 07 -- Implement WizardDialog with Step Indicator

**Description:** Create a multi-step wizard dialog with step indicator, Back/Next/Finish navigation, and per-step validation.

**Implementation Details:** WizardDialog extends ThemedDialog. Top area: step indicator showing dots or numbered circles for each step (current step filled with accent, completed steps with checkmark, future steps as empty circles). Content area: wxSimplebook containing one wxPanel per step. Bottom area: Back button (disabled on first step), Next button (advances to next step, validates current step first), Finish button (replaces Next on last step). Each step provides a `validate() -> bool` callback. If validation fails, the step shows an error and Next is blocked. Step count and content are configured via `add_step(title, panel, validate_fn)`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WizardDialog.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/WizardDialog.cpp` (create)

**Acceptance Criteria:**
- Step indicator shows correct state for all steps
- Back/Next navigation works correctly
- Validation blocks Next when step is invalid
- Finish appears on last step and closes dialog
- Step indicator updates when navigating between steps

**Dependencies:** Task 02

---

### Task 08 -- Implement Save Changes Dialog

**Description:** Create the standard "Save Changes" dialog shown when closing a file with unsaved modifications, with Save, Don't Save, and Cancel buttons.

**Implementation Details:** The Save Changes dialog is a specialized ConfirmDialog with: title "Do you want to save changes to {filename}?", message "Your changes will be lost if you don't save them.", buttons: Save (Primary), Don't Save (Secondary), Cancel (Ghost). Button order follows platform convention (via DialogModel::ordered_buttons()): on macOS, order is "Don't Save | Cancel | Save" with Save as default. On Windows/Linux, order is "Save | Don't Save | Cancel". The dialog returns an enum: `SaveChangesResult::Save`, `SaveChangesResult::DontSave`, `SaveChangesResult::Cancel`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.h` (extend)
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` (extend)

**Acceptance Criteria:**
- Dialog shows correct message with filename
- Button order follows platform convention
- Save returns SaveChangesResult::Save
- Don't Save returns SaveChangesResult::DontSave
- Escape and Cancel button return SaveChangesResult::Cancel

**Dependencies:** Task 04

---

### Task 09 -- Implement About Dialog

**Description:** Create an About dialog showing application name, version, build info, credits, and license information.

**Implementation Details:** About dialog extends ThemedDialog. Layout: centered application icon (64x64), application name "MarkAmp Studio" (24px bold), version string from build configuration, build date and commit hash, "Built with wxWidgets" attribution, copyright notice. Below, a scrollable credits section listing contributors. A "Copy Info" button copies version + build info to clipboard for bug reports. License link opens the LICENSE file in the editor. Dialog is non-resizable, fixed size (400x500).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (extend with AboutDialog)

**Acceptance Criteria:**
- About dialog shows correct version and build info
- Copy Info button copies version info to clipboard
- License link opens LICENSE file
- Credits section scrolls if content exceeds height
- Dialog is non-resizable

**Dependencies:** Task 02

---

### Task 10 -- Implement UnsavedChangesGuard

**Description:** Create a guard that automatically prompts the user to save unsaved changes when closing tabs, closing the application, or switching workspaces. Handles multiple unsaved files with a batch dialog.

**Implementation Details:** UnsavedChangesGuard is a service that hooks into: `wxCloseEvent` (app closing), `TabCloseEvent` (tab closing), `WorkspaceSwitchEvent` (workspace change). For a single file, it shows the Save Changes dialog (Task 08). For multiple files, it shows a batch dialog with a list of unsaved files (ThemedCheckbox per file, all checked by default) and Save Selected / Don't Save / Cancel buttons. The guard's `check_unsaved() -> bool` method returns false if the user cancelled (blocking the close/switch operation).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/UnsavedChangesGuard.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/UnsavedChangesGuard.cpp` (create)

**Acceptance Criteria:**
- Single unsaved file shows standard Save Changes dialog
- Multiple unsaved files show batch dialog with checkboxes
- Cancel blocks the close/switch operation
- Save Selected saves only checked files
- Don't Save proceeds without saving any files

**Dependencies:** Task 08

---

### Task 11 -- Implement File Picker Dialog (Custom)

**Description:** Create a custom-rendered file picker dialog that matches the application theme, used when native file dialogs are inappropriate (e.g., for workspace/project selection with preview).

**Implementation Details:** The custom file picker extends ThemedDialog. Layout: sidebar with bookmarks (Home, Documents, Desktop, Recent), main area with file/folder list rendered using TreeViewItem (Phase 41), breadcrumb path bar at top, file name input at bottom, file type filter dropdown, and Open/Cancel buttons. The picker supports: folder-only mode, file-only mode, multi-select mode, file type filtering, and path text input with autocomplete. For simple open/save operations, the native wxFileDialog is still preferred (configurable via setting).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (extend with FilePickerDialog)

**Acceptance Criteria:**
- File picker renders with themed appearance
- Navigation works via breadcrumb, sidebar, and double-click
- File type filter restricts visible files
- Multi-select mode allows selecting multiple files
- Path input supports autocomplete
- Setting allows falling back to native file dialog

**Dependencies:** Task 02, Phase 41 Task 18 (TreeViewItem), Phase 41 Task 06 (ThemedDropdown)

---

### Task 12 -- Implement Message Dialog (Info/Warning/Error)

**Description:** Create a simple message dialog for displaying information, warnings, and errors with an icon and OK button.

**Implementation Details:** MessageDialog is a simplified ConfirmDialog with a single OK button. The severity determines the icon: info (blue circle-i), warning (yellow triangle-!), error (red circle-x). The dialog auto-sizes to fit the message content (min 300px wide, max 500px wide). Long messages wrap and the dialog grows vertically. A "Copy" button in the title bar copies the message text to clipboard (useful for error messages). The OK button is focused by default and responds to Enter.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` (extend)

**Acceptance Criteria:**
- Correct icon renders for each severity
- OK button is focused by default
- Enter closes the dialog
- Long messages wrap correctly
- Copy button copies message to clipboard

**Dependencies:** Task 04

---

### Task 13 -- Implement Dialog Responsive Sizing

**Description:** Make all dialogs responsive to window size: dialogs should never exceed 90% of the parent window's dimensions, and should center correctly even when the parent is resized.

**Implementation Details:** ThemedDialog overrides `ShowModal()` to calculate max size as 90% of parent width and height. If the dialog's preferred size exceeds this, it shrinks proportionally. Content becomes scrollable when the dialog is too small to show everything. On parent resize (while dialog is open), the dialog re-centers itself. Minimum dialog size: 300x200. Maximum dialog size: 800x600 (for non-wizard dialogs). Wizard dialogs can be larger (up to 900x700).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (modify)

**Acceptance Criteria:**
- Dialog never exceeds 90% of parent window size
- Dialog re-centers on parent resize
- Content scrolls when dialog is too small
- Minimum size constraint is enforced
- Resizing behavior is smooth (no flicker)

**Dependencies:** Task 02

---

### Task 14 -- Implement Dialog Theming

**Description:** Ensure all dialogs respond to theme changes and render correctly in both light and dark themes.

**Implementation Details:** ThemedDialog subscribes to ThemeChangedEvent (via ThemeEngine, not ThemeAwareWindow since it extends wxDialog). On theme change: re-resolve all color tokens, refresh all child controls, repaint the dialog. The box shadow renders differently in light mode (darker shadow) vs dark mode (lighter shadow). Border color uses `border_default` token. Background uses `bg_surface` token. Title text uses `fg_primary` token. All child controls (ThemedButton, ThemedTextInput, etc.) handle their own theme updates via ThemeAwareWindow inheritance.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (modify)

**Acceptance Criteria:**
- Theme switch updates dialog colors immediately
- Box shadow adjusts for light vs dark themes
- All child controls update their themes
- No stale colors after theme change
- Dialog is readable and attractive in both light and dark themes

**Dependencies:** Task 02

---

### Task 15 -- Extend DialogModel with Input Dialog Support

**Description:** Extend DialogModel to support input dialog configuration: initial value, placeholder, validation function, input type.

**Implementation Details:** Add `InputDialogConfig` struct to DialogModel.h: `{ string label; string placeholder; string initial_value; int max_length; InputType type; // Text, Password, Number, Email }`. Add `set_input_config(InputDialogConfig)` and `input_config()` to DialogModel. Add `validate_input(const string& value) -> string` method that delegates to a stored validation function. The model validates without UI dependency, enabling unit testing of validation logic.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp` (modify)

**Acceptance Criteria:**
- InputDialogConfig stores all configuration fields
- validate_input() delegates to the stored validation function
- Model rejects input exceeding max_length
- Password type is supported
- Unit tests validate all input types

**Dependencies:** None

---

### Task 16 -- Extend DialogModel with Wizard State

**Description:** Extend DialogModel to track wizard state: current step, completed steps, per-step validation status.

**Implementation Details:** Add `WizardState` struct: `{ int current_step; int total_steps; vector<bool> step_completed; vector<bool> step_valid; }`. Add methods: `set_wizard_state(WizardState)`, `advance_step() -> bool` (returns false if current step is invalid), `go_back()`, `is_first_step()`, `is_last_step()`, `can_finish()`. The model enforces that you cannot advance past an invalid step or finish when any required step is incomplete.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp` (modify)

**Acceptance Criteria:**
- Wizard state tracks current step and completion
- advance_step() blocks on invalid current step
- can_finish() requires all steps completed or valid
- go_back() works from any step except first
- Unit tests verify all wizard navigation paths

**Dependencies:** None

---

### Task 17 -- Replace wxMessageBox Calls with Themed Dialogs

**Description:** Find and replace all wxMessageBox calls throughout the codebase with the appropriate themed dialog (MessageDialog, ConfirmDialog, Save Changes dialog).

**Implementation Details:** Search the codebase for all `wxMessageBox`, `wxMessageDialog`, and `wxDialog::ShowModal()` calls. Replace each with the equivalent themed dialog call. Map `wxOK` to MessageDialog, `wxYES_NO` to ConfirmDialog with two buttons, `wxYES_NO | wxCANCEL` to ConfirmDialog with three buttons. Preserve all existing behavior (return values, button labels, default buttons). Log all replacements for verification.

**Files Affected:**
- Multiple .cpp files throughout `/Users/ryanrentfro/code/markamp/src/ui/` (modify)
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` (modify)

**Acceptance Criteria:**
- Zero wxMessageBox calls remain in the codebase
- All dialogs render with themed appearance
- All existing dialog behaviors are preserved
- No regressions in user-facing dialog workflows
- Compilation succeeds with no missing includes

**Dependencies:** Tasks 04, 12

---

### Task 18 -- Migrate FirstRunWizard to WizardDialog

**Description:** Refactor the existing FirstRunWizard to use the new WizardDialog base class instead of directly extending wxDialog with manual navigation.

**Implementation Details:** Rewrite FirstRunWizard to extend WizardDialog. Migrate the 4 pages (Welcome, Theme Selection, Key Profile, Extension Recommendations) to WizardDialog steps. Use `add_step()` to register each page. The step indicator replaces the manual Back/Next/Finish button management. Validation: Theme page requires a theme selection, Profile page requires a profile selection. Extension page is optional (no validation). Preserve the `ShouldShow()` static method and the `first_run_completed` config check.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp` (modify)

**Acceptance Criteria:**
- FirstRunWizard renders using WizardDialog with step indicator
- All 4 pages work identically to before
- Navigation buttons are managed by WizardDialog base
- Step validation blocks advancement when selection is missing
- Visual appearance matches other themed dialogs

**Dependencies:** Task 07

---

### Task 19 -- Implement Dialog Entry and Exit Animations

**Description:** Add entrance and exit animations to dialogs: scale from 95% to 100% with fade-in on open, scale to 95% with fade-out on close.

**Implementation Details:** ThemedDialog overrides ShowModal() to start an entrance animation: the dialog begins at 95% scale and 0% opacity, then animates to 100% scale and 100% opacity over 200ms with ease-out-cubic. On EndModal(), the dialog animates to 95% scale and 0% opacity over 150ms before actually closing. The scale is achieved by rendering the dialog content to a wxBitmap and drawing it scaled (avoiding actual window resize which would cause layout thrashing). The backdrop fade-in/fade-out (Task 01) synchronizes with the dialog animation.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (modify)

**Acceptance Criteria:**
- Dialog appears with scale+fade entrance animation
- Dialog disappears with scale+fade exit animation
- Animation is 200ms entrance, 150ms exit
- No layout thrashing during animation
- Animation can be disabled via settings (for accessibility)

**Dependencies:** Task 02, Phase 41 Task 19 (AnimationDriver)

---

### Task 20 -- Implement Dialog Tab Navigation

**Description:** Implement full Tab navigation within dialogs: Tab cycles through all interactive controls, Shift+Tab cycles backward, focus is trapped within the dialog (does not leak to parent window).

**Implementation Details:** ThemedDialog implements focus trapping by overriding `ProcessEvent()` to intercept Tab key events. Maintain an ordered list of focusable controls within the dialog. Tab moves focus forward, Shift+Tab moves backward. When focus reaches the last control, Tab wraps to the first control. When focus is on the first control, Shift+Tab wraps to the last control. Focus is never allowed to leave the dialog while it is modal. The close X button is included in the Tab order (between title and content).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (modify)

**Acceptance Criteria:**
- Tab cycles through all controls in the dialog
- Shift+Tab cycles backward
- Focus wraps from last to first and vice versa
- Focus does not leak to parent window
- Close button is included in Tab order

**Dependencies:** Task 02

---

### Task 21 -- Implement Undo-Instead-of-Confirm Pattern

**Description:** For low-severity actions (delete file, remove tag), implement the "undo toast instead of confirmation dialog" pattern where the action executes immediately and shows an undo notification.

**Implementation Details:** The existing `DialogModel::prefer_undo(severity)` returns true for Info severity. When `prefer_undo` returns true, the action proceeds immediately, the item is soft-deleted (marked for permanent deletion after 10 seconds), and an undo notification toast appears with an "Undo" action button. Clicking Undo restores the item. After 10 seconds without undo, the deletion is finalized. This pattern is used for: file deletion (moved to trash), tag removal, bookmark removal, and notification dismissal.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` (modify -- add undo path)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp` (modify -- add undo toasts)

**Acceptance Criteria:**
- Low-severity actions execute immediately without dialog
- Undo toast appears for 10 seconds
- Clicking Undo restores the deleted item
- After 10 seconds, deletion is permanent
- High-severity actions still show confirmation dialog

**Dependencies:** Task 04, Phase 43 Task 09 (NotificationManager routing)

---

### Task 22 -- Implement Dialog Result Callbacks

**Description:** Add an async callback-based API for dialogs so callers do not need to block on ShowModal(). This is essential for non-blocking workflows.

**Implementation Details:** Add `ShowAsync(std::function<void(int result)> callback)` to ThemedDialog. This calls `ShowModal()` on the main thread and invokes the callback with the result. For convenience, add typed callback APIs to specific dialogs: `ConfirmDialog::ShowAsync(callback<ConfirmResult>)`, `InputDialog::ShowAsync(callback<optional<string>>)`, `ProgressDialog::ShowAsync(callback<bool cancelled>)`. The async API ensures the callback runs on the main thread. For callers that prefer synchronous, `ShowModal()` still works.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.h` (modify)

**Acceptance Criteria:**
- ShowAsync fires callback with correct result
- Callback runs on main thread
- Synchronous ShowModal still works
- Multiple async dialogs can be queued
- No race conditions between dialog close and callback

**Dependencies:** Task 02

---

### Task 23 -- Add Confirmation Dialog Convenience API

**Description:** Create a simple static API for common dialog patterns so callers can show a dialog in one line of code.

**Implementation Details:** Add static factory methods: `ConfirmDialog::Ask(parent, title, message) -> bool` (Yes/No, returns true for Yes), `ConfirmDialog::AskSave(parent, filename) -> SaveChangesResult`, `MessageDialog::Info(parent, title, message)`, `MessageDialog::Warning(parent, title, message)`, `MessageDialog::Error(parent, title, message)`, `InputDialog::GetText(parent, label, initial_value) -> optional<string>`. Each factory creates the appropriate dialog, shows it modally, and returns the result. These one-liners replace the verbose dialog setup code scattered across the codebase.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InputDialog.h` (modify)

**Acceptance Criteria:**
- One-line API shows correct dialog and returns result
- All factory methods work for their intended use case
- Factory methods use correct default buttons and severity
- Callers can replace multi-line dialog code with one-liners
- Return types are clear and unambiguous

**Dependencies:** Tasks 04, 05, 12

---

### Task 24 -- Add Dialog Accessibility

**Description:** Ensure all dialogs are accessible: screen reader announcements, focus management, role annotations.

**Implementation Details:** ThemedDialog has role `wxACC_ROLE_DIALOG` with name from title. On open, announce the dialog title and description via `wxAccessible::NotifyEvent(wxACC_EVENT_DIALOG_START)`. On close, announce `wxACC_EVENT_DIALOG_END`. All buttons and inputs within the dialog have correct roles and labels. Focus is set to the first interactive control (not the title bar) on open. The close X button has accessible name "Close dialog". Confirmation dialogs announce the severity (e.g., "Warning: Do you want to delete this file?").

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ConfirmDialog.cpp` (modify)

**Acceptance Criteria:**
- VoiceOver announces dialog title on open
- Focus lands on first interactive control
- All controls have correct accessible roles
- Escape closes the dialog (accessible keyboard shortcut)
- Screen reader reads severity for confirmation dialogs

**Dependencies:** Task 20

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new dialog files to CMakeLists.txt and create comprehensive unit tests for DialogModel extensions, dialog stacking, and UnsavedChangesGuard.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_dialog_system.cpp` with sections: (1) DialogModel remembers choices correctly, (2) DialogModel ordered_buttons follows platform convention, (3) InputDialogConfig validates inputs, (4) WizardState navigation and validation, (5) prefer_undo returns true for Info severity, (6) DialogStackManager tracks multiple dialogs, (7) DialogStackManager increases backdrop opacity, (8) UnsavedChangesGuard detects unsaved files, (9) Convenience APIs return correct results, (10) Dialog responsive sizing respects 90% constraint.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_dialog_system.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- DialogModel unit tests work without GUI
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- All wxMessageBox calls replaced with themed dialogs
- Dialog backdrop renders with fade animation
- Multi-dialog stacking works correctly
- FirstRunWizard migrated to WizardDialog
- Save Changes guard works for tab close and app close
- Keyboard navigation (Tab, Enter, Escape) works in all dialogs
- All dialogs are accessible via screen reader
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
