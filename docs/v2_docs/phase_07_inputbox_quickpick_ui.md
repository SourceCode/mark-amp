# Phase 07: InputBox & QuickPick UI Integration

**Priority:** High
**Estimated Scope:** ~6 files affected
**Dependencies:** Phase 02

## Objective

Wire InputBoxService and QuickPickService to actual UI dialogs so that extensions can prompt users for input and selections at runtime, not just in tests.

## Background/Context

Both services currently store callbacks and set `visible_ = true`, but never trigger any UI. They have test helpers (`test_accept()`, `test_cancel()`) for unit testing, but no real wxWidgets integration. The `// TODO(extensions): Trigger UI to show...` comments in both `.cpp` files confirm this is intentionally incomplete.

The existing CommandPalette already implements a filterable list with keyboard navigation -- QuickPick should reuse this pattern. InputBox can use a simple wxTextEntryDialog or a custom themed dialog.

## Detailed Tasks

### Task 1: Implement InputBoxService UI trigger

**File:** `/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp`

Replace the TODO in `show()` with a mechanism to notify the UI layer. Use EventBus to publish a request event:

Add to Events.h:
```cpp
MARKAMP_DECLARE_EVENT_BEGIN(ShowInputBoxRequestEvent)
    std::string title;
    std::string prompt;
    std::string value;
    std::string placeholder;
    bool password{false};
MARKAMP_DECLARE_EVENT_END();

MARKAMP_DECLARE_EVENT_BEGIN(InputBoxResultEvent)
    std::optional<std::string> result; // nullopt = cancelled
MARKAMP_DECLARE_EVENT_END();
```

Or alternatively, have LayoutManager/MainFrame hold a reference to InputBoxService and poll/subscribe.

### Task 2: Create InputBox dialog in MainFrame or LayoutManager

**File:** `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` or new file

When `ShowInputBoxRequestEvent` fires, show a themed dialog:
```cpp
wxTextEntryDialog dialog(this, prompt, title, value);
if (dialog.ShowModal() == wxID_OK) {
    input_box_service_->test_accept(dialog.GetValue().ToStdString());
} else {
    input_box_service_->test_cancel();
}
```

For a themed experience, create a custom dialog that uses ThemeEngine colors.

### Task 3: Implement QuickPickService UI trigger

**File:** `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`

Same pattern: publish `ShowQuickPickRequestEvent` via EventBus.

### Task 4: Create QuickPick dialog reusing CommandPalette pattern

**File:** New file or extend CommandPalette

The CommandPalette already has a filterable list with keyboard navigation. Create a variant mode or a separate QuickPickDialog that:
- Shows a list of QuickPickItem entries
- Supports filtering by label/description
- Returns selected item via callback
- Supports multi-select mode with checkboxes

### Task 5: Wire callbacks

Ensure that when the user makes a selection or cancels, the stored callback in the service is invoked correctly.

### Task 6: Update NotificationServiceAPI action buttons

**File:** `/Users/ryanrentfro/code/markamp/src/core/NotificationServiceAPI.cpp`

Line 45 has `// TODO(extensions): Wire action button callback via EventBus response channel`. Wire the `show_with_actions()` method to include action buttons in the notification toast, and route button clicks back to the callback.

## Acceptance Criteria

1. `input_box_service_->show()` displays a real dialog to the user
2. User input is returned to the extension via callback
3. `quick_pick_service_->show()` displays a filterable pick list
4. QuickPick supports both single-select and multi-select
5. NotificationServiceAPI action buttons trigger callbacks
6. All existing tests still pass (test helpers remain functional)

## Testing Requirements

- Manual testing: extension calls show() -> dialog appears -> result returned
- Add automated test using a mock parent window
- Existing test suite passes
