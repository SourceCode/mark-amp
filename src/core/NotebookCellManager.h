/// @file NotebookCellManager.h
/// @brief V4 Phase 34 – Cell Metadata, Execution Tracking, and Variable Inspector.
/// Notebook cell management: CRUD, tags, execution tracking, serialization.

#pragma once

#include "core/KernelManager.h"

#include <chrono>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Cell type enumeration
// ============================================================================

enum class CellType : uint8_t
{
    kCode,
    kMarkdown,
    kRaw
};

// ============================================================================
// CellMetadata – per-cell metadata
// ============================================================================

struct CellMetadata
{
    std::string cell_id;
    CellType type{CellType::kCode};
    std::string language;         // "python", "r", etc.
    int execution_count{0};       // [1], [2], etc.
    bool collapsed_input{false};  // Hide code, show output only
    bool collapsed_output{false}; // Hide output
    bool editable{true};
    bool deletable{true};
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> custom;

    // Execution timing.
    std::optional<std::chrono::system_clock::time_point> started_at;
    std::optional<std::chrono::system_clock::time_point> completed_at;
    double execution_time_ms{0.0};

    /// Return "[N]" or "[ ]" label.
    [[nodiscard]] auto execution_label() const -> std::string;

    /// Check if cell is currently running.
    [[nodiscard]] auto is_running() const -> bool;
};

// ============================================================================
// NotebookCell – cell with source and output references
// ============================================================================

struct NotebookCell
{
    CellMetadata metadata;
    std::string source;
    std::vector<std::string> output_ids;
};

// ============================================================================
// VariableInfo – kernel namespace variable
// ============================================================================

struct VariableInfo
{
    std::string name;
    std::string type_name; // "int", "pandas.DataFrame", "list"
    std::string value_repr;
    int64_t size_bytes{0};
    std::string shape; // For arrays: "(100, 5)"
};

// ============================================================================
// NotebookCellManager
// ============================================================================

class NotebookCellManager
{
public:
    explicit NotebookCellManager(EventBus& event_bus);

    // --- Cell CRUD ---
    auto add_cell(CellType type, int position = -1) -> std::string;
    auto remove_cell(const std::string& cell_id) -> void;
    auto move_cell(const std::string& cell_id, int new_position) -> void;
    auto set_cell_source(const std::string& cell_id, const std::string& source) -> void;

    // --- Cell Queries ---
    [[nodiscard]] auto get_cell(const std::string& cell_id) const -> std::optional<NotebookCell>;
    [[nodiscard]] auto cells() const -> const std::vector<NotebookCell>&;
    [[nodiscard]] auto cell_count() const -> int;
    [[nodiscard]] auto cell_at(int position) const -> const NotebookCell&;

    // --- Cell Metadata ---
    auto set_collapsed_input(const std::string& cell_id, bool collapsed) -> void;
    auto set_collapsed_output(const std::string& cell_id, bool collapsed) -> void;
    auto add_cell_tag(const std::string& cell_id, const std::string& tag) -> void;
    auto record_execution(const std::string& cell_id, int count, double elapsed_ms) -> void;

    // --- Execution Tracking ---
    [[nodiscard]] auto next_execution_count() -> int;
    [[nodiscard]] auto execution_order() const -> const std::vector<std::string>&;

    // --- Serialization ---
    [[nodiscard]] auto serialize_to_json() const -> std::string;
    auto deserialize_from_json(const std::string& json) -> void;

private:
    EventBus& event_bus_;
    std::vector<NotebookCell> cells_;
    int execution_counter_{0};
    std::vector<std::string> execution_order_;
    int next_cell_id_{1};

    /// Generate a unique cell ID.
    [[nodiscard]] auto generate_cell_id() -> std::string;

    /// Find a cell by ID, returning iterator.
    [[nodiscard]] auto find_cell(const std::string& cell_id) -> std::vector<NotebookCell>::iterator;
    [[nodiscard]] auto find_cell(const std::string& cell_id) const
        -> std::vector<NotebookCell>::const_iterator;
};

// ============================================================================
// VariableInspector
// ============================================================================

class VariableInspector
{
public:
    VariableInspector(EventBus& event_bus, KernelManager& kernel_manager);

    /// Refresh the variable list from the kernel (stubbed).
    [[nodiscard]] auto refresh(const std::string& kernel_id)
        -> std::expected<std::vector<VariableInfo>, std::string>;

    /// Get the last refreshed variable list.
    [[nodiscard]] auto variables() const -> const std::vector<VariableInfo>&;

    /// Get detailed inspection of a specific variable (stubbed).
    [[nodiscard]] auto inspect_variable(const std::string& kernel_id,
                                        const std::string& var_name) const
        -> std::expected<MimeBundle, std::string>;

    /// Delete a variable from the kernel namespace (stubbed).
    [[nodiscard]] auto delete_variable(const std::string& kernel_id, const std::string& var_name)
        -> std::expected<void, std::string>;

    /// Add a variable manually (for testing).
    auto add_variable(const VariableInfo& info) -> void;

    /// Clear all variables.
    auto clear() -> void;

private:
    EventBus& event_bus_;
    KernelManager& kernel_manager_;
    std::vector<VariableInfo> variables_;
};

} // namespace markamp::core
