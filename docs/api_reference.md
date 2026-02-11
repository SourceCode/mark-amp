# MarkAmp API Reference

Technical reference for MarkAmp's core interfaces, event types, configuration, and theme format.

---

## Core Interfaces

### IThemeEngine (`core/ThemeEngine.h`)

```cpp
class IThemeEngine {
    virtual auto current_theme() const -> const Theme& = 0;
    virtual void apply_theme(const std::string& theme_id) = 0;
    virtual auto available_themes() const -> std::vector<ThemeInfo> = 0;
    virtual void import_theme(const std::filesystem::path& path) = 0;
    virtual void export_theme(const std::string& theme_id,
                              const std::filesystem::path& path) = 0;
};
```

### AppStateManager (`core/AppState.h`)

```cpp
class AppStateManager {
    auto state() const -> const AppState&;
    void set_active_file(const std::string& file_id);
    void set_view_mode(events::ViewMode mode);
    void set_sidebar_visible(bool visible);
    void set_cursor_position(int line, int column);
    void update_content(const std::string& content);
};
```

### MarkdownParser (`core/MarkdownParser.h`)

```cpp
class MarkdownParser {
    auto parse(std::string_view markdown)
        -> std::expected<MarkdownDocument, std::string>;
};
```

### HtmlRenderer (`rendering/HtmlRenderer.h`)

```cpp
class HtmlRenderer {
    auto render(const MarkdownDocument& doc) -> std::string;
    auto render_with_footnotes(const MarkdownDocument& doc,
                               const std::string& footnote_section)
        -> std::string;
    void set_mermaid_renderer(IMermaidRenderer* renderer);
    void set_base_path(const std::filesystem::path& base_path);
};
```

### ThemeValidator (`core/ThemeValidator.h`)

```cpp
struct ValidationResult {
    bool is_valid{false};
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

class ThemeValidator {
    auto validate_json(const nlohmann::json& json) const -> ValidationResult;
    auto validate_theme(const Theme& theme) const -> ValidationResult;
    static auto contains_null_bytes(const std::string& str) -> bool;
    static auto contains_control_chars(const std::string& str) -> bool;
};
```

### EventBus (`core/EventBus.h`)

```cpp
class EventBus {
    template<typename T>
    auto subscribe(std::function<void(const T&)> handler) -> Subscription;

    template<typename T>
    void publish(const T& event);
};
```

`Subscription` is RAII — the handler is unsubscribed when the `Subscription` object is destroyed.

---

## Event Types (`core/Events.h`)

| Event                      | Fields                               | Published when      |
| -------------------------- | ------------------------------------ | ------------------- |
| `ThemeChangedEvent`        | `theme_id: string`                   | Theme applied       |
| `ViewModeChangedEvent`     | `mode: ViewMode`                     | View mode toggled   |
| `ActiveFileChangedEvent`   | `file_id: string`, `content: string` | File selected       |
| `ContentChangedEvent`      | `content: string`                    | Editor text changed |
| `CursorChangedEvent`       | `line: int`, `column: int`           | Cursor moved        |
| `SidebarToggleEvent`       | `visible: bool`                      | Sidebar show/hide   |
| `EditorScrollChangedEvent` | `scroll_fraction: double`            | Editor scrolled     |

### ViewMode Enum

```cpp
enum class ViewMode { Editor, Preview, Split };
```

---

## Configuration (`core/Config.h`)

### Typed Getters

```cpp
auto get_string(string_view key, string_view default_val = "") -> string;
auto get_int(string_view key, int default_val = 0) -> int;
auto get_bool(string_view key, bool default_val = false) -> bool;
auto get_double(string_view key, double default_val = 0.0) -> double;
```

### Typed Setters

```cpp
void set(string_view key, string_view value);
void set(string_view key, int value);
void set(string_view key, bool value);
void set(string_view key, double value);
```

### Default Configuration Keys

| Key               | Type   | Default           | Description             |
| ----------------- | ------ | ----------------- | ----------------------- |
| `theme`           | string | `"midnight-neon"` | Active theme ID         |
| `view_mode`       | string | `"split"`         | Default view mode       |
| `sidebar_visible` | bool   | `true`            | Sidebar visibility      |
| `sidebar_width`   | int    | `250`             | Sidebar width in pixels |
| `font_size`       | int    | `14`              | Editor font size        |
| `word_wrap`       | bool   | `true`            | Word wrap enabled       |
| `line_numbers`    | bool   | `true`            | Show line numbers       |
| `auto_save`       | bool   | `false`           | Auto-save on focus loss |

---

## Theme JSON Format

### Structure

```json
{
  "id": "theme-identifier",
  "name": "Human-Readable Name",
  "author": "Author Name",
  "description": "Theme description",
  "colors": {
    "--bg-app": "#1a1a2e",
    "--bg-panel": "#1f1f3a",
    "--bg-header": "#16162b",
    "--bg-input": "#12122a",
    "--text-main": "#e0e0e0",
    "--text-muted": "#808090",
    "--accent-primary": "#00d4ff",
    "--accent-secondary": "#ff6b9d",
    "--border-light": "#3a3a5a",
    "--border-dark": "#0a0a1a"
  }
}
```

### Required Color Keys

These 10 color keys are **required** in every theme:

| Key                  | Usage                           |
| -------------------- | ------------------------------- |
| `--bg-app`           | Main application background     |
| `--bg-panel`         | Panel/sidebar backgrounds       |
| `--bg-header`        | Title bar and toolbar           |
| `--bg-input`         | Input fields and editor         |
| `--text-main`        | Primary text color              |
| `--text-muted`       | Secondary/dimmed text           |
| `--accent-primary`   | Primary accent (links, buttons) |
| `--accent-secondary` | Secondary accent (highlights)   |
| `--border-light`     | Light border color              |
| `--border-dark`      | Dark border color               |

### Color Format

Colors must be valid CSS hex values: `#RGB`, `#RRGGBB`, or `#RRGGBBAA`.

### Validation Rules

| Rule                         | Level   | Description                         |
| ---------------------------- | ------- | ----------------------------------- |
| All 10 required keys present | Error   | Missing keys reject the theme       |
| All color values parseable   | Error   | Invalid hex values reject the theme |
| `id` ≤ 64 characters         | Error   | Theme ID length limit               |
| `name` ≤ 100 characters      | Error   | Theme name length limit             |
| No null bytes in strings     | Error   | Security check                      |
| No control characters        | Error   | Security check                      |
| Text/bg contrast ≥ 4.5:1     | Warning | WCAG AA accessibility               |

---

## FileNode (`core/FileNode.h`)

```cpp
enum class FileNodeType { File, Folder };

struct FileNode {
    std::string id;
    std::string name;
    FileNodeType type{FileNodeType::File};
    std::optional<std::string> content;
    std::vector<FileNode> children;
    bool is_open{false};

    auto is_folder() const -> bool;
    auto is_file() const -> bool;
    auto find_by_id(const std::string& target_id) -> FileNode*;
    auto file_count() const -> std::size_t;
    auto folder_count() const -> std::size_t;
    auto extension() const -> std::string;
};
```
