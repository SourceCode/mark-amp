/// @file CellOutputRenderer.h
/// @brief V4 Phase 31 – Inline Output Rendering and MIME Bundles (pure-logic engine).
/// Renders execution results from MimeBundle into typed CellOutput objects.
/// No wxWidgets dependencies – testable without a UI toolkit.

#pragma once

#include "core/KernelManager.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{

// ============================================================================
// Output type classification
// ============================================================================

enum class OutputType : uint8_t
{
    kPlainText,
    kHtml,
    kImage,
    kSvg,
    kJson,
    kError,
    kStream
};

// ============================================================================
// CellOutput – a single rendered output item
// ============================================================================

struct CellOutput
{
    std::string cell_id;
    OutputType type{OutputType::kPlainText};
    std::string content;
    std::string mime_type;
    int execution_count{0};
    bool is_error{false};
    std::vector<std::string> traceback;
    std::string stream_name; // "stdout" or "stderr"
};

// ============================================================================
// CellOutputState – all outputs for one cell
// ============================================================================

struct CellOutputState
{
    std::string cell_id;
    std::vector<CellOutput> outputs;
    bool collapsed{false};
    bool cleared{false};
    int execution_count{0};
};

// ============================================================================
// CellOutputRenderer – pure-logic MIME rendering engine
// ============================================================================

class CellOutputRenderer
{
public:
    explicit CellOutputRenderer(markamp::core::EventBus& event_bus);

    // --- Rendering ---

    /// Render a MimeBundle as the best available format.
    [[nodiscard]] auto render_output(const markamp::core::MimeBundle& bundle) const -> CellOutput;

    /// Render an error with formatted traceback.
    [[nodiscard]] auto render_error(const std::string& error_name,
                                    const std::string& error_value,
                                    const std::vector<std::string>& traceback) const -> CellOutput;

    /// Render a stream output (stdout/stderr).
    [[nodiscard]] auto render_stream(const std::string& stream_name, const std::string& text) const
        -> CellOutput;

    /// Format ANSI escape codes: strip sequences, return plain text.
    [[nodiscard]] static auto ansi_to_styled(const std::string& text) -> std::string;

    // --- Output state management ---

    /// Add an output to a cell's state.
    auto add_output(const std::string& cell_id, const CellOutput& output) -> void;

    /// Clear all outputs for a cell.
    auto clear_outputs(const std::string& cell_id) -> void;

    /// Toggle collapsed state for a cell's output area.
    auto toggle_collapsed(const std::string& cell_id) -> void;

    /// Get the output state for a cell.
    [[nodiscard]] auto get_state(const std::string& cell_id) const -> const CellOutputState*;

    /// Get all cell output states.
    [[nodiscard]] auto all_states() const
        -> const std::unordered_map<std::string, CellOutputState>&;

private:
    markamp::core::EventBus& event_bus_;
    std::unordered_map<std::string, CellOutputState> states_;

    /// Select the highest-priority MIME type from a bundle.
    [[nodiscard]] static auto select_best_mime(const markamp::core::MimeBundle& bundle)
        -> std::string;

    /// Map a MIME type string to an OutputType enum.
    [[nodiscard]] static auto mime_to_output_type(const std::string& mime_type) -> OutputType;
};

} // namespace markamp::ui
