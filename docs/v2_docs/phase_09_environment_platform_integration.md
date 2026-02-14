# Phase 09: EnvironmentService Platform Integration

**Priority:** High
**Estimated Scope:** ~3 files affected
**Dependencies:** None

## Objective

Replace the stub implementations in EnvironmentService with real platform-integrated clipboard access and URI opening.

## Background/Context

`EnvironmentService` has two critical stubs:
1. `clipboard_write()` writes to a local `std::string` member, not the system clipboard
2. `clipboard_read()` reads from that local string -- completely disconnected from the OS
3. `open_external()` returns `true` without actually opening anything

These stubs have TODO comments acknowledging the gap. wxWidgets provides cross-platform clipboard access via `wxClipboard` and URL opening via `wxLaunchDefaultBrowser`.

## Detailed Tasks

### Task 1: Implement clipboard_read() using wxClipboard

**File:** `/Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp`

```cpp
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

auto EnvironmentService::clipboard_read() const -> std::string
{
    std::string result;
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            result = data.GetText().ToStdString();
        }
        wxTheClipboard->Close();
    }
    return result;
}
```

### Task 2: Implement clipboard_write() using wxClipboard

**File:** `/Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp`

```cpp
void EnvironmentService::clipboard_write(const std::string& text)
{
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(wxString(text)));
        wxTheClipboard->Close();
    }
}
```

### Task 3: Implement open_external() using wxLaunchDefaultBrowser

**File:** `/Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp`

```cpp
#include <wx/utils.h>

auto EnvironmentService::open_external(const std::string& uri) -> bool
{
    if (uri.empty()) return false;
    return wxLaunchDefaultBrowser(wxString(uri));
}
```

### Task 4: Implement machine_id() with persistent UUID

**File:** `/Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp`

Replace the hardcoded "markamp-dev-machine" with a generated UUID stored in the config directory:
```cpp
auto EnvironmentService::machine_id() const -> std::string
{
    if (!machine_id_.empty()) return machine_id_;

    // Try to read from persistent file
    auto config_dir = Config::config_directory();
    auto id_path = config_dir / "machine-id";

    std::error_code ec;
    if (std::filesystem::exists(id_path, ec)) {
        std::ifstream file(id_path);
        std::string id;
        std::getline(file, id);
        if (!id.empty()) return id;
    }

    // Generate and persist a new UUID
    // Use a simple hash of hostname + time for uniqueness
    auto uuid = generate_uuid();
    std::ofstream file(id_path);
    file << uuid;
    return uuid;
}
```

### Task 5: Update tests to account for platform integration

**File:** `/Users/ryanrentfro/code/markamp/tests/unit/test_p1_p4_services.cpp`

Keep existing tests using the override setters (set_machine_id, set_language, set_app_root) for deterministic testing. Add new tests that verify the wxClipboard integration when running with a wxApp context.

## Acceptance Criteria

1. clipboard_write() puts text on the system clipboard
2. clipboard_read() retrieves text from the system clipboard
3. open_external() opens URLs in the default browser
4. machine_id() generates and persists a unique identifier
5. Existing tests pass (they use override setters)

## Testing Requirements

- Existing test_p1_p4_services passes (uses override setters)
- Manual testing: verify clipboard round-trip works
- Manual testing: verify open_external opens a browser
