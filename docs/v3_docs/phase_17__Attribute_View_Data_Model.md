# Phase 17 -- Attribute View Core Data Model

**Priority:** High (foundation for Phases 18-23)
**Estimated Scope:** ~8 new files, ~3 modified files
**Dependencies:** None (standalone data layer)

## Objective

Implement the Attribute View (AV) data model -- SiYuan's database-like feature that attaches structured data tables to Markdown blocks. An Attribute View is a typed schema (columns/keys) with rows (values) and multiple view layouts (Table, Gallery, Kanban). This is analogous to Notion databases or Airtable bases embedded directly in a knowledge management system.

The data model provides:
- A schema definition with 17 typed column types (KeyTypes)
- Row storage with typed variant values per column
- Multiple named views (Table, Gallery, Kanban) over the same underlying data
- JSON-based persistence to `storage/av/{id}.json` files
- Thread-safe in-memory cache for active attribute views

This phase establishes only the data structures, serialization, and storage layer. Rendering (Phases 19-22) and advanced column logic (Phase 18) are separate.

## Prerequisites

- None. This phase introduces a new subsystem with no dependencies on prior v3 phases.
- Requires existing: `EventBus`, `Config`, `PluginContext` (for future service registration).

## SiYuan Source Reference

| SiYuan File | Purpose | Key Structures |
|---|---|---|
| `kernel/av/av.go` | Root AV struct, View struct, ViewType enum | `AttributeView`, `View`, `ViewType`, `KeyValues` |
| `kernel/av/key.go` | Column schema definition | `Key`, `KeyType`, `SelectOption` |
| `kernel/av/value.go` | Typed value union | `Value`, `ValueBlock`, `ValueText`, `ValueNumber`, `ValueDate`, `ValueSelect`, `ValueMSelect`, `ValueURL`, `ValueEmail`, `ValuePhone`, `ValueAsset`, `ValueTemplate`, `ValueCreated`, `ValueUpdated`, `ValueCheckbox`, `ValueRelation`, `ValueRollup` |
| `kernel/av/table.go` | Table view layout config | `TableView`, `TableColumn` |
| `kernel/av/gallery.go` | Gallery view layout config | `GalleryView` |
| `kernel/av/kanban.go` | Kanban view layout config | `KanbanView` |
| `kernel/model/attribute_view.go` | CRUD operations, JSON I/O | `LoadAttributeView`, `SaveAttributeView` |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/core/av/AVTypes.h` | `markamp::core::av` | All enums, structs, type aliases for the AV data model |
| `src/core/av/AVTypes.cpp` | `markamp::core::av` | Non-trivial method implementations (serialization helpers) |
| `src/core/av/AVValue.h` | `markamp::core::av` | `AVValue` variant type with per-type accessor methods |
| `src/core/av/AVValue.cpp` | `markamp::core::av` | Value serialization, comparison, formatting stubs |
| `src/core/av/AttributeView.h` | `markamp::core::av` | `AttributeView` aggregate root struct |
| `src/core/av/AttributeView.cpp` | `markamp::core::av` | View manipulation, key/row CRUD |
| `src/core/av/AttributeViewStore.h` | `markamp::core::av` | Storage layer: load, save, create, delete, list |
| `src/core/av/AttributeViewStore.cpp` | `markamp::core::av` | JSON file I/O with `nlohmann::json` or manual serialization |
| `tests/unit/test_av_data_model.cpp` | (anonymous) | Catch2 tests for this phase |

### Files to Modify

| File | Change |
|---|---|
| `src/CMakeLists.txt` | Add `core/av/AVTypes.cpp`, `core/av/AVValue.cpp`, `core/av/AttributeView.cpp`, `core/av/AttributeViewStore.cpp` to `markamp` target sources |
| `tests/CMakeLists.txt` | Add `test_av_data_model` test target |
| `src/core/Events.h` | Add AV-related events (see Events section below) |
| `src/core/PluginContext.h` | Forward-declare `AttributeViewStore`, add pointer field (deferred to wiring phase) |

## Data Structures to Implement

### File: `src/core/av/AVTypes.h`

```cpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Column Type Enumeration (17 types, matches SiYuan kernel/av/key.go KeyType)
// ============================================================================

enum class AVKeyType
{
    Block,        // Reference to a content block (primary key for rows)
    Text,         // Plain text string
    Number,       // Floating-point number with configurable format
    Date,         // Unix timestamp(s) with optional end date
    Select,       // Single-select from predefined options
    MSelect,      // Multi-select from predefined options
    URL,          // URL string with optional smart display
    Email,        // Email address string
    Phone,        // Phone number string
    MAsset,       // Multiple file/image assets
    Template,     // Computed template column (read-only, rendered from template expression)
    Created,      // Auto-populated creation timestamp (read-only)
    Updated,      // Auto-populated last-modified timestamp (read-only)
    Checkbox,     // Boolean toggle
    Relation,     // Link(s) to rows in another attribute view
    Rollup,       // Aggregation over related rows via a relation column
    LineNumber    // Auto-incremented row number (read-only, virtual)
};

/// Convert AVKeyType to its canonical string name for serialization.
[[nodiscard]] auto key_type_to_string(AVKeyType type) -> std::string;

/// Parse a string into AVKeyType. Returns AVKeyType::Text on unrecognized input.
[[nodiscard]] auto string_to_key_type(const std::string& str) -> AVKeyType;

// ============================================================================
// View Type Enumeration
// ============================================================================

enum class AVViewType
{
    Table,
    Gallery,
    Kanban
};

[[nodiscard]] auto view_type_to_string(AVViewType type) -> std::string;
[[nodiscard]] auto string_to_view_type(const std::string& str) -> AVViewType;

// ============================================================================
// Number Format (matches SiYuan kernel/av/value.go NumberFormat)
// ============================================================================

enum class AVNumberFormat
{
    None,       // Raw number, no formatting
    Commas,     // 1,234,567.89
    Percent,    // 0.75 -> 75%
    USDollar,   // $1,234.56
    Euro,       // 1.234,56 EUR
    Pound,      // GBP 1,234.56
    Yen,        // JPY 1,235
    Yuan,       // CNY 1,234.56
    Ruble,      // RUB 1 234,56
    Rupee,      // INR 1,234.56
    Won,        // KRW 1,235
    Custom      // User-defined format string
};

[[nodiscard]] auto number_format_to_string(AVNumberFormat fmt) -> std::string;
[[nodiscard]] auto string_to_number_format(const std::string& str) -> AVNumberFormat;

// ============================================================================
// Select Option
// ============================================================================

struct AVSelectOption
{
    std::string name;
    std::string color;  // CSS color string, e.g. "#FF6B6B" or "red"

    [[nodiscard]] auto operator==(const AVSelectOption& other) const -> bool = default;
};

// ============================================================================
// Relation Configuration (stored on the Key, not the Value)
// ============================================================================

struct AVRelationConfig
{
    std::string dest_av_id;       // ID of the target attribute view
    bool is_two_way{false};       // If true, target AV has a back-relation column
    std::string back_key_id;      // Key ID of the back-relation column in target AV
};

// ============================================================================
// Rollup Configuration (stored on the Key, not the Value)
// ============================================================================

enum class AVRollupCalc
{
    None,              // Show all values (no aggregation)
    CountAll,
    CountValues,
    CountUniqueValues,
    CountEmpty,
    CountNotEmpty,
    PercentEmpty,
    PercentNotEmpty,
    Sum,
    Average,
    Median,
    Min,
    Max,
    Range,
    DateEarliest,
    DateLatest,
    Checked,
    Unchecked,
    PercentChecked,
    PercentUnchecked
};

[[nodiscard]] auto rollup_calc_to_string(AVRollupCalc calc) -> std::string;
[[nodiscard]] auto string_to_rollup_calc(const std::string& str) -> AVRollupCalc;

struct AVRollupConfig
{
    std::string relation_key_id;  // Key ID of the relation column to traverse
    std::string target_key_id;    // Key ID in the related AV to aggregate
    AVRollupCalc calc{AVRollupCalc::None};
};

// ============================================================================
// Column Schema (Key)
// Matches SiYuan kernel/av/key.go Key struct
// ============================================================================

struct AVKey
{
    std::string id;                          // Unique key ID (UUID)
    std::string name;                        // Display name
    AVKeyType type{AVKeyType::Text};         // Column data type
    std::string icon;                        // Optional emoji or icon identifier
    std::string desc;                        // Optional description/tooltip

    // Type-specific configuration
    AVNumberFormat number_format{AVNumberFormat::None};
    std::string custom_number_format;        // Only used when number_format == Custom
    std::vector<AVSelectOption> options;     // Select/MSelect options
    std::string template_expr;               // Template expression (for Template type)

    // Relation/Rollup configuration
    AVRelationConfig relation_config;
    AVRollupConfig rollup_config;

    [[nodiscard]] auto operator==(const AVKey& other) const -> bool
    {
        return id == other.id;
    }
};

// ============================================================================
// View Column Configuration
// Per-column display settings within a specific view
// ============================================================================

struct AVViewColumn
{
    std::string key_id;           // References AVKey::id
    bool hidden{false};           // Whether this column is hidden in this view
    int width{200};               // Column width in pixels (table view)
    bool pinned{false};           // Whether this column is pinned/frozen
    bool wrap{false};             // Whether cell content wraps

    [[nodiscard]] auto operator==(const AVViewColumn& other) const -> bool = default;
};

// ============================================================================
// Table View Layout
// ============================================================================

struct AVTableView
{
    std::vector<AVViewColumn> columns;
    int row_height{32};           // Default row height in pixels
    bool show_row_numbers{false};
    bool stripe_rows{true};       // Alternating row colors
};

// ============================================================================
// Gallery View Layout
// ============================================================================

enum class AVCardSize
{
    Small,    // 4 cards per row
    Medium,   // 3 cards per row
    Large     // 2 cards per row
};

[[nodiscard]] auto card_size_to_string(AVCardSize size) -> std::string;
[[nodiscard]] auto string_to_card_size(const std::string& str) -> AVCardSize;

enum class AVCoverAspectRatio
{
    Ratio16x9,
    Ratio4x3,
    Ratio1x1,
    Auto
};

struct AVGalleryView
{
    std::vector<AVViewColumn> columns;   // Which fields to show on cards
    int page_size{50};
    AVCardSize card_size{AVCardSize::Medium};
    std::string cover_key_id;            // Key ID of MAsset column used as cover image
    std::string cover_field;             // Alternative: field name for cover
    bool hide_empty_values{true};
    AVCoverAspectRatio cover_ratio{AVCoverAspectRatio::Ratio16x9};
};

// ============================================================================
// Kanban View Layout
// ============================================================================

struct AVKanbanView
{
    std::vector<AVViewColumn> columns;    // Which fields to show on cards
    std::string group_by_key_id;          // Key ID of Select/MSelect column for lanes
    bool hide_empty_groups{false};
    int page_size{50};
};

// ============================================================================
// View (named layout over the same data)
// ============================================================================

struct AVView
{
    std::string id;                       // Unique view ID (UUID)
    std::string name;                     // Display name (e.g. "Main Table", "Card Gallery")
    std::string icon;                     // Optional icon
    std::string desc;                     // Optional description
    AVViewType type{AVViewType::Table};
    int page_size{50};

    // Exactly one of these is populated based on type
    AVTableView table;
    AVGalleryView gallery;
    AVKanbanView kanban;
};

// ============================================================================
// Specification Version
// ============================================================================

inline constexpr int kAVSpecVersion = 1;

} // namespace markamp::core::av
```

### File: `src/core/av/AVValue.h`

```cpp
#pragma once

#include "AVTypes.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Per-type value structs (matches SiYuan kernel/av/value.go Value* structs)
// ============================================================================

struct AVValueBlock
{
    std::string id;               // Block ID
    std::string content;          // Block content text (denormalized for display)
    bool is_detached{false};      // True if the block no longer exists

    [[nodiscard]] auto operator==(const AVValueBlock& other) const -> bool = default;
};

struct AVValueText
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValueText& other) const -> bool = default;
};

struct AVValueNumber
{
    double content{0.0};
    bool is_not_empty{false};     // Distinguishes 0 from "no value set"
    AVNumberFormat format{AVNumberFormat::None};
    std::string formatted_content; // Cached formatted string

    [[nodiscard]] auto operator==(const AVValueNumber& other) const -> bool = default;
};

struct AVValueDate
{
    int64_t content{0};            // Unix timestamp (seconds) for start date
    int64_t content2{0};           // Unix timestamp for end date (if has_end_date)
    bool is_not_empty{false};
    bool is_not_time{false};       // True if only date, no time component
    bool has_end_date{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueDate& other) const -> bool = default;
};

struct AVValueSelect
{
    AVSelectOption option;         // Single selected option

    [[nodiscard]] auto operator==(const AVValueSelect& other) const -> bool = default;
};

struct AVValueMSelect
{
    std::vector<AVSelectOption> options;  // Multiple selected options

    [[nodiscard]] auto operator==(const AVValueMSelect& other) const -> bool = default;
};

struct AVValueURL
{
    std::string content;           // Raw URL
    std::string smart_url;         // Display-friendly URL (e.g. domain only)

    [[nodiscard]] auto operator==(const AVValueURL& other) const -> bool = default;
};

struct AVValueEmail
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValueEmail& other) const -> bool = default;
};

struct AVValuePhone
{
    std::string content;

    [[nodiscard]] auto operator==(const AVValuePhone& other) const -> bool = default;
};

struct AVValueAssetEntry
{
    std::string name;              // Display name
    std::string content;           // File path or URL
    std::string type;              // "file" or "image"

    [[nodiscard]] auto operator==(const AVValueAssetEntry& other) const -> bool = default;
};

struct AVValueMAsset
{
    std::vector<AVValueAssetEntry> assets;

    [[nodiscard]] auto operator==(const AVValueMAsset& other) const -> bool = default;
};

struct AVValueTemplate
{
    std::string content;           // Rendered template output (read-only)

    [[nodiscard]] auto operator==(const AVValueTemplate& other) const -> bool = default;
};

struct AVValueCreated
{
    int64_t content{0};            // Unix timestamp
    bool is_not_empty{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueCreated& other) const -> bool = default;
};

struct AVValueUpdated
{
    int64_t content{0};
    bool is_not_empty{false};
    std::string formatted_content;

    [[nodiscard]] auto operator==(const AVValueUpdated& other) const -> bool = default;
};

struct AVValueCheckbox
{
    bool checked{false};

    [[nodiscard]] auto operator==(const AVValueCheckbox& other) const -> bool = default;
};

struct AVRelationContent
{
    std::string block_id;
    std::string content;           // Denormalized block content for display

    [[nodiscard]] auto operator==(const AVRelationContent& other) const -> bool = default;
};

struct AVValueRelation
{
    std::vector<std::string> block_ids;
    std::vector<AVRelationContent> contents;

    [[nodiscard]] auto operator==(const AVValueRelation& other) const -> bool = default;
};

// Forward declaration for recursive type
struct AVValue;

struct AVValueRollup
{
    std::vector<AVValue> contents; // Aggregated values from related rows

    // Cannot use defaulted == due to recursive AVValue
    [[nodiscard]] auto operator==(const AVValueRollup& other) const -> bool;
};

// ============================================================================
// AVValue: Union of all typed values via std::variant
// ============================================================================

/// Monostate represents an empty/unset value.
using AVValueVariant = std::variant<
    std::monostate,     // Empty / no value
    AVValueBlock,
    AVValueText,
    AVValueNumber,
    AVValueDate,
    AVValueSelect,
    AVValueMSelect,
    AVValueURL,
    AVValueEmail,
    AVValuePhone,
    AVValueMAsset,
    AVValueTemplate,
    AVValueCreated,
    AVValueUpdated,
    AVValueCheckbox,
    AVValueRelation,
    AVValueRollup
>;

struct AVValue
{
    std::string id;                // Unique value ID (UUID)
    std::string key_id;            // References AVKey::id
    std::string block_id;          // Row identifier (block this value belongs to)
    AVKeyType type{AVKeyType::Text};
    AVValueVariant data;

    // ── Accessors ──
    // Each returns a pointer to the typed value, or nullptr if type mismatch.

    [[nodiscard]] auto as_block() -> AVValueBlock*;
    [[nodiscard]] auto as_block() const -> const AVValueBlock*;
    [[nodiscard]] auto as_text() -> AVValueText*;
    [[nodiscard]] auto as_text() const -> const AVValueText*;
    [[nodiscard]] auto as_number() -> AVValueNumber*;
    [[nodiscard]] auto as_number() const -> const AVValueNumber*;
    [[nodiscard]] auto as_date() -> AVValueDate*;
    [[nodiscard]] auto as_date() const -> const AVValueDate*;
    [[nodiscard]] auto as_select() -> AVValueSelect*;
    [[nodiscard]] auto as_select() const -> const AVValueSelect*;
    [[nodiscard]] auto as_mselect() -> AVValueMSelect*;
    [[nodiscard]] auto as_mselect() const -> const AVValueMSelect*;
    [[nodiscard]] auto as_url() -> AVValueURL*;
    [[nodiscard]] auto as_url() const -> const AVValueURL*;
    [[nodiscard]] auto as_email() -> AVValueEmail*;
    [[nodiscard]] auto as_email() const -> const AVValueEmail*;
    [[nodiscard]] auto as_phone() -> AVValuePhone*;
    [[nodiscard]] auto as_phone() const -> const AVValuePhone*;
    [[nodiscard]] auto as_masset() -> AVValueMAsset*;
    [[nodiscard]] auto as_masset() const -> const AVValueMAsset*;
    [[nodiscard]] auto as_template() -> AVValueTemplate*;
    [[nodiscard]] auto as_template() const -> const AVValueTemplate*;
    [[nodiscard]] auto as_created() -> AVValueCreated*;
    [[nodiscard]] auto as_created() const -> const AVValueCreated*;
    [[nodiscard]] auto as_updated() -> AVValueUpdated*;
    [[nodiscard]] auto as_updated() const -> const AVValueUpdated*;
    [[nodiscard]] auto as_checkbox() -> AVValueCheckbox*;
    [[nodiscard]] auto as_checkbox() const -> const AVValueCheckbox*;
    [[nodiscard]] auto as_relation() -> AVValueRelation*;
    [[nodiscard]] auto as_relation() const -> const AVValueRelation*;
    [[nodiscard]] auto as_rollup() -> AVValueRollup*;
    [[nodiscard]] auto as_rollup() const -> const AVValueRollup*;

    /// Returns true if the value holds std::monostate (no data set).
    [[nodiscard]] auto is_empty() const -> bool;

    /// Format the value as a human-readable string (stub -- full formatting in Phase 18).
    [[nodiscard]] auto to_display_string() const -> std::string;

    [[nodiscard]] auto operator==(const AVValue& other) const -> bool = default;
};

} // namespace markamp::core::av
```

### File: `src/core/av/AttributeView.h`

```cpp
#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// KeyValues: All values for a single column across all rows
// Matches SiYuan kernel/av/av.go KeyValues struct
// ============================================================================

struct AVKeyValues
{
    AVKey key;
    std::vector<AVValue> values;  // One value per row, indexed by row order
};

// ============================================================================
// AttributeView: Root aggregate
// Matches SiYuan kernel/av/av.go AttributeView struct
// ============================================================================

struct AttributeView
{
    std::string id;                       // Unique AV ID (UUID format)
    std::string name;                     // Display name
    int spec{kAVSpecVersion};             // Schema version for migration
    std::vector<AVKeyValues> key_values;  // Columns with their row values
    std::vector<AVView> views;            // Named view layouts

    // ── Key (column) operations ──

    /// Add a new column. Returns the assigned key ID.
    [[nodiscard]] auto add_key(AVKey key) -> std::string;

    /// Remove a column by key ID. Returns false if not found.
    [[nodiscard]] auto remove_key(const std::string& key_id) -> bool;

    /// Find a key by ID. Returns nullptr if not found.
    [[nodiscard]] auto find_key(const std::string& key_id) -> AVKey*;
    [[nodiscard]] auto find_key(const std::string& key_id) const -> const AVKey*;

    /// Get all key IDs in order.
    [[nodiscard]] auto key_ids() const -> std::vector<std::string>;

    /// Get the number of columns.
    [[nodiscard]] auto key_count() const -> size_t;

    // ── Row operations ──

    /// Add a new row (block). Inserts empty values for all columns.
    /// Returns the block_id assigned to the row.
    [[nodiscard]] auto add_row(const std::string& block_id) -> std::string;

    /// Remove a row by block_id. Returns false if not found.
    [[nodiscard]] auto remove_row(const std::string& block_id) -> bool;

    /// Get all unique block_ids (row identifiers) in order.
    [[nodiscard]] auto row_block_ids() const -> std::vector<std::string>;

    /// Get the number of rows.
    [[nodiscard]] auto row_count() const -> size_t;

    // ── Value operations ──

    /// Get a value by key_id and block_id. Returns nullptr if not found.
    [[nodiscard]] auto get_value(const std::string& key_id,
                                  const std::string& block_id) -> AVValue*;
    [[nodiscard]] auto get_value(const std::string& key_id,
                                  const std::string& block_id) const -> const AVValue*;

    /// Set a value. Creates the value entry if it does not exist.
    void set_value(const std::string& key_id,
                   const std::string& block_id,
                   AVValueVariant data);

    // ── View operations ──

    /// Add a new view. Returns the assigned view ID.
    [[nodiscard]] auto add_view(AVView view) -> std::string;

    /// Remove a view by ID. Returns false if not found.
    [[nodiscard]] auto remove_view(const std::string& view_id) -> bool;

    /// Find a view by ID.
    [[nodiscard]] auto find_view(const std::string& view_id) -> AVView*;
    [[nodiscard]] auto find_view(const std::string& view_id) const -> const AVView*;

    /// Get the first view, or create a default table view if none exist.
    [[nodiscard]] auto default_view() -> AVView&;
};

} // namespace markamp::core::av
```

### File: `src/core/av/AttributeViewStore.h`

```cpp
#pragma once

#include "AttributeView.h"

#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::av
{

/// Persistent storage for AttributeView objects.
/// Each AV is stored as a JSON file at `{storage_root}/av/{id}.json`.
/// Provides thread-safe caching for loaded AVs.
class AttributeViewStore
{
public:
    /// Construct with a storage root directory.
    /// The `av/` subdirectory is created automatically.
    explicit AttributeViewStore(std::filesystem::path storage_root);

    // ── CRUD operations ──

    /// Create a new empty AttributeView with a generated UUID.
    /// Persists immediately. Returns the created AV.
    [[nodiscard]] auto create(const std::string& name)
        -> std::expected<AttributeView, std::string>;

    /// Load an AV from disk (or cache). Returns error string on failure.
    [[nodiscard]] auto load(const std::string& av_id)
        -> std::expected<AttributeView, std::string>;

    /// Save an AV to disk and update cache.
    [[nodiscard]] auto save(const AttributeView& av)
        -> std::expected<void, std::string>;

    /// Delete an AV from disk and cache.
    [[nodiscard]] auto delete_av(const std::string& av_id)
        -> std::expected<void, std::string>;

    // ── Query operations ──

    /// List all AV IDs found in the storage directory.
    [[nodiscard]] auto get_all_ids() const -> std::vector<std::string>;

    /// Check if an AV exists on disk.
    [[nodiscard]] auto exists(const std::string& av_id) const -> bool;

    // ── Cache management ──

    /// Remove an AV from the in-memory cache (forces reload from disk on next access).
    void evict(const std::string& av_id);

    /// Clear the entire in-memory cache.
    void clear_cache();

    /// Get the file path for an AV ID.
    [[nodiscard]] auto file_path_for(const std::string& av_id) const
        -> std::filesystem::path;

private:
    std::filesystem::path storage_root_;
    std::filesystem::path av_directory_;  // storage_root_ / "av"

    mutable std::mutex mutex_;
    std::unordered_map<std::string, AttributeView> cache_;

    /// Serialize an AttributeView to a JSON string.
    [[nodiscard]] auto serialize(const AttributeView& av) const -> std::string;

    /// Deserialize a JSON string into an AttributeView.
    [[nodiscard]] auto deserialize(const std::string& json_str)
        -> std::expected<AttributeView, std::string>;

    /// Generate a UUID v4 string.
    [[nodiscard]] static auto generate_uuid() -> std::string;

    /// Ensure the av/ directory exists.
    [[nodiscard]] auto ensure_directory() -> std::expected<void, std::string>;
};

} // namespace markamp::core::av
```

## Key Functions to Implement

### AVTypes.cpp

1. **`key_type_to_string(AVKeyType)`** -- Switch on all 17 enum values, return lowercase string (e.g. `AVKeyType::MSelect` -> `"mSelect"`). Used for JSON serialization.
2. **`string_to_key_type(const std::string&)`** -- Reverse lookup with a static `std::unordered_map<std::string, AVKeyType>`. Return `AVKeyType::Text` for unrecognized strings.
3. **`view_type_to_string(AVViewType)`** / **`string_to_view_type(const std::string&)`** -- Same pattern for the 3 view types.
4. **`number_format_to_string(AVNumberFormat)`** / **`string_to_number_format(const std::string&)`** -- Same pattern for the 12 number formats.
5. **`rollup_calc_to_string(AVRollupCalc)`** / **`string_to_rollup_calc(const std::string&)`** -- Same pattern for the 20 rollup calculations.
6. **`card_size_to_string(AVCardSize)`** / **`string_to_card_size(const std::string&)`** -- Same pattern for 3 card sizes.

### AVValue.cpp

7. **`AVValue::as_<type>()` accessors (const and non-const overloads)** -- Use `std::get_if<AVValue*type*>(&data)` to safely extract the typed pointer. Return `nullptr` on type mismatch.
8. **`AVValue::is_empty()`** -- Return `std::holds_alternative<std::monostate>(data)`.
9. **`AVValue::to_display_string()`** -- Basic string representation: visit the variant, return content string for text types, `std::to_string` for numbers, "true"/"false" for checkbox, comma-joined names for selects. Full formatting deferred to Phase 18.
10. **`AVValueRollup::operator==()`** -- Element-wise comparison of the `contents` vector.

### AttributeView.cpp

11. **`AttributeView::add_key(AVKey)`** -- If `key.id` is empty, generate a UUID. Push a new `AVKeyValues{key, {}}` to `key_values`. For each existing row (determined from the Block column), insert a default empty `AVValue` in the new column's values vector.
12. **`AttributeView::remove_key(const std::string&)`** -- Find and erase the `AVKeyValues` entry. Also remove the column from all views' column lists.
13. **`AttributeView::find_key(const std::string&)`** -- Linear scan of `key_values` comparing `key.id`.
14. **`AttributeView::add_row(const std::string&)`** -- For each `AVKeyValues` entry, push an empty `AVValue{generate_uuid(), kv.key.id, block_id, kv.key.type, std::monostate{}}`.
15. **`AttributeView::remove_row(const std::string&)`** -- For each `AVKeyValues` entry, erase the value with matching `block_id`.
16. **`AttributeView::row_count()`** -- Return the size of the first `AVKeyValues`'s values vector (all columns have the same row count).
17. **`AttributeView::get_value(key_id, block_id)`** -- Find the `AVKeyValues` for `key_id`, then linear scan its values for matching `block_id`.
18. **`AttributeView::set_value(key_id, block_id, data)`** -- Find or create the value entry, set `data` field.
19. **`AttributeView::add_view(AVView)`** -- If `view.id` is empty, generate UUID. Push to `views`. If the view has no columns configured, auto-populate from all keys.
20. **`AttributeView::default_view()`** -- Return `views.front()` or create and insert a default table view.

### AttributeViewStore.cpp

21. **`AttributeViewStore::create(name)`** -- Generate UUID, construct `AttributeView{id, name, kAVSpecVersion, {}, {}}`, add a default Block key and a default table view, call `save()`, return the AV.
22. **`AttributeViewStore::load(av_id)`** -- Check cache first. If miss, read file at `av_directory_ / (av_id + ".json")` using `std::ifstream` with `std::error_code`. Deserialize. Store in cache. Return.
23. **`AttributeViewStore::save(av)`** -- Serialize to JSON string. Write to `av_directory_ / (av.id + ".json")` using `std::ofstream` with `std::error_code`. Update cache.
24. **`AttributeViewStore::delete_av(av_id)`** -- Remove file with `std::filesystem::remove()` using `std::error_code` overload. Evict from cache.
25. **`AttributeViewStore::get_all_ids()`** -- Iterate `av_directory_` with `std::filesystem::directory_iterator` using `std::error_code`. Collect `.json` filenames, strip extension.
26. **`AttributeViewStore::serialize(av)`** -- Build JSON manually or use nlohmann::json. Serialize all fields including nested structs. Use the `*_to_string()` helpers for enums.
27. **`AttributeViewStore::deserialize(json_str)`** -- Parse JSON. Reconstruct all structs. Use `string_to_*()` helpers for enums. Return error on malformed input.
28. **`AttributeViewStore::generate_uuid()`** -- Use `<random>` with `std::mt19937` and `std::uniform_int_distribution` to generate a UUID v4 string.

## Events to Add

Add to `src/core/Events.h` in a new section:

```cpp
// ============================================================================
// Attribute View events (Phase 17+)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVCreatedEvent)
std::string av_id;
std::string name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVDeletedEvent)
std::string av_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVSavedEvent)
std::string av_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKeyAddedEvent)
std::string av_id;
std::string key_id;
std::string key_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKeyRemovedEvent)
std::string av_id;
std::string key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowAddedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowRemovedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVValueChangedEvent)
std::string av_id;
std::string key_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVViewAddedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVViewRemovedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `av.storage_directory` | string | `"storage"` | Root directory for AV JSON files (relative to workspace) |
| `av.auto_save` | bool | `true` | Auto-save AVs after every edit |
| `av.auto_save_delay_ms` | int | `1000` | Debounce delay before auto-save triggers |
| `av.max_cache_entries` | int | `50` | Maximum number of AVs held in memory cache |
| `av.default_page_size` | int | `50` | Default number of rows per page in views |

## Test Cases

File: `tests/unit/test_av_data_model.cpp`

All tests use Catch2 with `[av][data_model]` tags.

1. **Create empty AttributeView** -- Construct an `AttributeView` with an ID and name. Verify `id`, `name`, `spec == kAVSpecVersion`, empty `key_values`, empty `views`.

2. **Add and retrieve a key** -- Call `add_key()` with a Text-type key. Verify `key_count() == 1`. Call `find_key()` and verify returned key matches. Verify `key_ids()` contains the new ID.

3. **Add and retrieve a row** -- Add a Block key first, then call `add_row("block-1")`. Verify `row_count() == 1`. Verify `row_block_ids()` contains `"block-1"`. Verify each column has a value entry for the new row.

4. **Set and get a typed value** -- Add a Number key. Add a row. Call `set_value(key_id, block_id, AVValueNumber{42.0, true, AVNumberFormat::None, ""})`. Call `get_value()`. Verify `as_number()->content == 42.0`.

5. **Save and load round-trip via AttributeViewStore** -- Create a store with a temp directory. Create an AV with 3 keys (Text, Number, Checkbox) and 5 rows with values. Save. Clear cache. Load. Verify all keys, rows, and values match the original.

6. **Multiple views on same data** -- Create an AV. Add a Table view and a Gallery view. Verify `views.size() == 2`. Verify each view has correct type. Modify view columns. Save and reload. Verify view configurations persist.

7. **Select options on key** -- Create an AV with a Select key having 3 options ("Todo"/"red", "In Progress"/"yellow", "Done"/"green"). Add a row and set its select value to "In Progress". Verify `as_select()->option.name == "In Progress"`. Save/load round-trip.

8. **All 17 key types can be instantiated** -- For each `AVKeyType` enum value, create a key with that type. Add a row. Set an appropriate default value for the type. Verify `get_value()` returns the correct typed variant. This test ensures no type is accidentally missing from the variant.

9. **Value serialization for all types** -- Create values of each of the 17 types. Serialize to JSON via `AttributeViewStore::serialize()`. Deserialize. Verify equality with originals using `operator==`. Covers the full variant round-trip.

10. **View configuration persistence** -- Create a TableView with custom column widths, hidden columns, and row striping disabled. Save/load. Verify all configuration fields survive the round-trip.

11. **Large AV with 1000 rows** -- Create an AV with 5 columns (Text, Number, Date, Checkbox, Select). Add 1000 rows with randomized values. Save to disk. Measure load time. Verify load time is under 500ms on debug builds. Verify all 1000 rows are present after load.

12. **Corrupt file handling** -- Write invalid JSON to an AV file path. Call `load()`. Verify it returns an error string (not a crash or exception). Write truncated JSON. Verify same. Write valid JSON with missing required fields. Verify graceful degradation or error.

## Acceptance Criteria

- [ ] `AVKeyType` enum has all 17 values with string conversion round-trips
- [ ] `AVViewType` enum has 3 values (Table, Gallery, Kanban)
- [ ] `AVValue` variant holds all 17 typed value structs plus `std::monostate`
- [ ] `AVValue` typed accessors (`as_text()`, `as_number()`, etc.) return correct pointers or nullptr
- [ ] `AttributeView` supports add/remove/find for keys, rows, and values
- [ ] `AttributeView` supports add/remove/find for views
- [ ] `AttributeViewStore` creates, loads, saves, and deletes AV JSON files
- [ ] `AttributeViewStore` uses `std::error_code` for all filesystem operations (no exceptions from fs)
- [ ] JSON serialization/deserialization round-trips all fields faithfully
- [ ] Corrupt or missing JSON files produce descriptive error strings, not crashes
- [ ] All 12 test cases pass
- [ ] No use of `catch(...)` anywhere in the implementation
- [ ] All query methods have `[[nodiscard]]`
- [ ] Code uses 4-space indentation and Allman brace style

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/core/av/AVTypes.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVTypes.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVValue.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVValue.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AttributeView.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AttributeView.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AttributeViewStore.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AttributeViewStore.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_data_model.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 4 new `.cpp` source files to `markamp` target
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_data_model` test target
- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 10 new AV event declarations
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 5 new `av.*` config keys
