# Phase 05: Application Architecture and Module Design

## Objective

Define and implement the core application architecture: module boundaries, interfaces, the event bus for inter-component communication, and the service locator or dependency injection pattern that will wire the application together. This phase establishes the contracts that all subsequent phases will implement against.

## Prerequisites

- Phase 04 (wxWidgets Integration and Basic Window)

## Deliverables

1. Event bus system for decoupled inter-component communication
2. Service registry / dependency injection container
3. Core interface definitions (abstract base classes / concepts) for all major subsystems
4. Application state management model
5. Command pattern implementation for undo/redo capability
6. Configuration management system

## Tasks

### Task 5.1: Design and implement the Event Bus

Create `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` and `EventBus.cpp`:

The event bus provides publish-subscribe communication between components without direct coupling.

**Design requirements:**
- Type-safe event dispatching using C++26 features
- Support for synchronous and asynchronous (queued) event delivery
- Event listener registration with automatic cleanup (RAII subscription tokens)
- Thread-safe for multi-threaded scenarios

**Event base class:**
```cpp
namespace markamp::core {

// Base event type
struct Event
{
    virtual ~Event() = default;
    [[nodiscard]] virtual auto type_name() const -> std::string_view = 0;
};

// Subscription token -- unsubscribes on destruction
class Subscription
{
public:
    Subscription() = default;
    explicit Subscription(std::function<void()> unsubscribe_fn);
    ~Subscription();
    Subscription(Subscription&&) noexcept;
    Subscription& operator=(Subscription&&) noexcept;
    // Non-copyable
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    void cancel();
private:
    std::function<void()> unsubscribe_fn_;
};

class EventBus
{
public:
    // Subscribe to events of type T
    template<typename T>
    requires std::derived_from<T, Event>
    [[nodiscard]] auto subscribe(std::function<void(const T&)> handler) -> Subscription;

    // Publish an event synchronously
    template<typename T>
    requires std::derived_from<T, Event>
    void publish(const T& event);

    // Queue an event for later delivery (on the main thread)
    template<typename T>
    requires std::derived_from<T, Event>
    void queue(T event);

    // Process all queued events (call from main loop)
    void process_queued();

private:
    // Implementation using type-erased handler lists
    // ...
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Events can be published and received by subscribers
- Subscriptions automatically unregister on destruction
- Queued events are delivered when `process_queued()` is called
- At least 5 unit tests covering subscribe, publish, unsubscribe, queue, and type safety

### Task 5.2: Define core event types

Create `/Users/ryanrentfro/code/markamp/src/core/Events.h` with all application events:

```cpp
namespace markamp::core::events {

// Theme events
struct ThemeChangedEvent : Event {
    std::string theme_id;
    // ...
};

// File events
struct FileOpenedEvent : Event {
    std::string file_path;
    std::string content;
};

struct FileContentChangedEvent : Event {
    std::string file_id;
    std::string new_content;
};

struct FileSavedEvent : Event {
    std::string file_path;
};

struct ActiveFileChangedEvent : Event {
    std::string file_id;
};

// View events
struct ViewModeChangedEvent : Event {
    // ViewMode enum value
};

struct SidebarToggleEvent : Event {
    bool visible;
};

// Editor events
struct CursorPositionChangedEvent : Event {
    int line;
    int column;
};

struct EditorContentChangedEvent : Event {
    std::string content;
};

// Application events
struct AppReadyEvent : Event {};
struct AppShutdownEvent : Event {};

} // namespace markamp::core::events
```

**Acceptance criteria:**
- All event types are defined
- Each event type has a unique `type_name()`
- Events are default-constructible and movable

### Task 5.3: Implement the Service Registry

Create `/Users/ryanrentfro/code/markamp/src/core/ServiceRegistry.h` and `ServiceRegistry.cpp`:

A lightweight service locator that allows components to be wired together without hard dependencies.

```cpp
namespace markamp::core {

class ServiceRegistry
{
public:
    // Register a service implementation
    template<typename Interface, typename Implementation>
    requires std::derived_from<Implementation, Interface>
    void register_service(std::shared_ptr<Implementation> impl);

    // Retrieve a service
    template<typename Interface>
    [[nodiscard]] auto get() const -> std::shared_ptr<Interface>;

    // Check if a service is registered
    template<typename Interface>
    [[nodiscard]] auto has() const -> bool;

    // Singleton access (the registry itself is application-scoped)
    static auto instance() -> ServiceRegistry&;

private:
    // Type-erased storage using std::any or similar
    std::unordered_map<std::type_index, std::any> services_;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Services can be registered and retrieved by interface type
- Attempting to get an unregistered service returns nullptr or throws a descriptive error
- Thread-safe for read access after initial registration
- Unit tests cover registration, retrieval, and missing service scenarios

### Task 5.4: Define core interfaces (abstract base classes)

Create interface headers for each major subsystem. These define the contracts that later phases will implement.

**`/Users/ryanrentfro/code/markamp/src/core/IThemeEngine.h`:**
```cpp
namespace markamp::core {
class IThemeEngine {
public:
    virtual ~IThemeEngine() = default;
    virtual auto current_theme() const -> const Theme& = 0;
    virtual void apply_theme(const std::string& theme_id) = 0;
    virtual auto available_themes() const -> std::vector<ThemeInfo> = 0;
    virtual void import_theme(const std::filesystem::path& path) = 0;
    virtual void export_theme(const std::string& theme_id, const std::filesystem::path& path) = 0;
};
}
```

**`/Users/ryanrentfro/code/markamp/src/core/IFileSystem.h`:**
```cpp
namespace markamp::core {
class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual auto read_file(const std::filesystem::path& path) -> std::expected<std::string, std::string> = 0;
    virtual auto write_file(const std::filesystem::path& path, std::string_view content) -> std::expected<void, std::string> = 0;
    virtual auto list_directory(const std::filesystem::path& path) -> std::expected<std::vector<FileNode>, std::string> = 0;
    virtual auto watch_file(const std::filesystem::path& path, std::function<void()> callback) -> Subscription = 0;
};
}
```

**`/Users/ryanrentfro/code/markamp/src/core/IMarkdownParser.h`:**
```cpp
namespace markamp::core {
class IMarkdownParser {
public:
    virtual ~IMarkdownParser() = default;
    virtual auto parse(std::string_view markdown) -> std::expected<MarkdownDocument, std::string> = 0;
    virtual auto render_html(const MarkdownDocument& doc) -> std::string = 0;
};
}
```

**`/Users/ryanrentfro/code/markamp/src/core/IMermaidRenderer.h`:**
```cpp
namespace markamp::core {
class IMermaidRenderer {
public:
    virtual ~IMermaidRenderer() = default;
    virtual auto render(std::string_view mermaid_source) -> std::expected<std::string, std::string> = 0;
    virtual auto is_available() const -> bool = 0;
};
}
```

**Acceptance criteria:**
- All interface headers compile without errors
- Interfaces use C++26 features (std::expected, concepts where applicable)
- Each interface has clear documentation comments

### Task 5.5: Implement Application State Model

Create `/Users/ryanrentfro/code/markamp/src/core/AppState.h` and `AppState.cpp`:

Centralized application state that all components can observe via the event bus.

```cpp
namespace markamp::core {

enum class ViewMode { Editor, Preview, Split };

struct AppState
{
    // Current state
    std::string active_file_id;
    std::string active_file_content;
    ViewMode view_mode{ViewMode::Split};
    bool sidebar_visible{true};
    std::string current_theme_id{"midnight-neon"};
    int cursor_line{1};
    int cursor_column{1};
    std::string encoding{"UTF-8"};

    // File tree
    std::vector<FileNode> file_tree;

    // Derived state
    [[nodiscard]] auto active_file_name() const -> std::string;
    [[nodiscard]] auto is_modified() const -> bool;
};

class AppStateManager
{
public:
    explicit AppStateManager(EventBus& event_bus);

    [[nodiscard]] auto state() const -> const AppState&;

    // State mutations (these publish events)
    void set_active_file(const std::string& file_id);
    void set_view_mode(ViewMode mode);
    void set_sidebar_visible(bool visible);
    void set_cursor_position(int line, int column);
    void update_content(const std::string& content);

private:
    AppState state_;
    EventBus& event_bus_;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- State mutations publish corresponding events
- State is observable through the event bus
- State changes are atomic (no partial updates visible)
- Unit tests verify state transitions and event emissions

### Task 5.6: Implement Command pattern for undo/redo

Create `/Users/ryanrentfro/code/markamp/src/core/Command.h` and `CommandHistory.h/.cpp`:

```cpp
namespace markamp::core {

class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    [[nodiscard]] virtual auto description() const -> std::string = 0;
    [[nodiscard]] virtual auto is_mergeable_with(const Command& other) const -> bool;
    virtual void merge_with(Command& other);
};

class CommandHistory
{
public:
    void execute(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    [[nodiscard]] auto undo_description() const -> std::string;
    [[nodiscard]] auto redo_description() const -> std::string;
    void clear();

private:
    std::vector<std::unique_ptr<Command>> history_;
    size_t current_index_{0};
    static constexpr size_t kMaxHistorySize = 1000;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Commands can be executed, undone, and redone
- History has a configurable maximum size
- Executing a new command after undo clears the redo stack
- Mergeable commands (e.g., consecutive character typing) can be combined
- Unit tests cover all operations including edge cases

### Task 5.7: Implement Configuration Manager

Create `/Users/ryanrentfro/code/markamp/src/core/Config.h` and `Config.cpp`:

Manages persistent application settings stored as JSON.

```cpp
namespace markamp::core {

class Config
{
public:
    // Load/save from platform-appropriate config directory
    auto load() -> std::expected<void, std::string>;
    auto save() const -> std::expected<void, std::string>;

    // Typed getters with defaults
    [[nodiscard]] auto get_string(std::string_view key, std::string_view default_val = "") const -> std::string;
    [[nodiscard]] auto get_int(std::string_view key, int default_val = 0) const -> int;
    [[nodiscard]] auto get_bool(std::string_view key, bool default_val = false) const -> bool;
    [[nodiscard]] auto get_double(std::string_view key, double default_val = 0.0) const -> double;

    // Setters
    void set(std::string_view key, std::string_view value);
    void set(std::string_view key, int value);
    void set(std::string_view key, bool value);
    void set(std::string_view key, double value);

    // Platform config directory
    [[nodiscard]] static auto config_directory() -> std::filesystem::path;
    [[nodiscard]] static auto config_file_path() -> std::filesystem::path;

private:
    nlohmann::json data_;
};

} // namespace markamp::core
```

Config directory locations:
- macOS: `~/Library/Application Support/MarkAmp/`
- Windows: `%APPDATA%/MarkAmp/`
- Linux: `~/.config/markamp/`

**Acceptance criteria:**
- Config loads from and saves to the correct platform directory
- Missing config file creates defaults
- Corrupt config file is handled gracefully (reset to defaults with warning)
- Unit tests verify get/set/save/load cycle

### Task 5.8: Wire the architecture together in MarkAmpApp

Update `MarkAmpApp::OnInit()` to:

1. Create the `EventBus` instance
2. Create the `Config` and load settings
3. Create the `AppStateManager`
4. Create the `CommandHistory`
5. Register all services in the `ServiceRegistry`
6. Pass necessary references to `MainFrame`
7. Log the initialization sequence

**Acceptance criteria:**
- Application starts with all core services initialized
- Services are accessible via the registry
- Shutdown cleans up in reverse order
- No memory leaks (verify with ASAN)

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/EventBus.h` | Created |
| `src/core/EventBus.cpp` | Created |
| `src/core/Events.h` | Created |
| `src/core/ServiceRegistry.h` | Created |
| `src/core/ServiceRegistry.cpp` | Created |
| `src/core/IThemeEngine.h` | Created |
| `src/core/IFileSystem.h` | Created |
| `src/core/IMarkdownParser.h` | Created |
| `src/core/IMermaidRenderer.h` | Created |
| `src/core/AppState.h` | Created |
| `src/core/AppState.cpp` | Created |
| `src/core/Command.h` | Created |
| `src/core/CommandHistory.h` | Created |
| `src/core/CommandHistory.cpp` | Created |
| `src/core/Config.h` | Created |
| `src/core/Config.cpp` | Created |
| `src/app/MarkAmpApp.cpp` | Modified |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_event_bus.cpp` | Created |
| `tests/unit/test_service_registry.cpp` | Created |
| `tests/unit/test_app_state.cpp` | Created |
| `tests/unit/test_command_history.cpp` | Created |
| `tests/unit/test_config.cpp` | Created |

## Dependencies

- nlohmann/json (for Config serialization)
- std::expected (C++23/26)
- std::type_index, std::any (for ServiceRegistry)
- Platform APIs for config directory detection (or wxStandardPaths)

## Estimated Complexity

**High** -- This phase defines the architectural backbone. The event bus must be type-safe and efficient. The service registry must be thread-safe. The command pattern must handle edge cases. All of these are foundational -- bugs here ripple through every subsequent phase.

## CRITICAL: App Sandbox Compatibility Note

Since MarkAmp will be distributed through the **Mac App Store** and **Microsoft Store**, the architecture MUST be designed for sandboxed execution from the start:

1. **File access**: ALL file operations must go through a `FileAccessService` abstraction that:
   - On macOS: uses security-scoped bookmarks for persisted file access
   - On Windows: uses broker-based file access for MSIX apps
   - Provides `NSOpenPanel`/`NSSavePanel` integration for user-initiated file selection
   - Tracks "recent files" using security-scoped bookmarks (macOS) or future access list (Windows)

2. **No process spawning**: The Mermaid rendering MUST use an embedded JS engine (QuickJS), NOT an external CLI tool, because App Sandbox prohibits spawning child processes.

3. **No network access**: The architecture should not include any networking code. All resources (fonts, themes, grammars) must be bundled.

4. **Config storage**: Use `wxStandardPaths::GetUserDataDir()` which maps to the app container on sandboxed platforms.

5. **Clipboard access**: Allowed but should use wxWidgets clipboard API for cross-platform safety.

These constraints are NOT optional -- violating them will result in App Store rejection. Design for them from Phase 05 onward.
