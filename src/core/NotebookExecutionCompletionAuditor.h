/// @file NotebookExecutionCompletionAuditor.h
/// @brief V23 Phase 06 — Notebook, kernel, AI, and code execution completion auditor.
///
/// Audits that notebook kernel lifecycle, execution pipeline, AI integration,
/// and cell runtime behaviors have real implementations rather than simulated
/// or stubbed transports.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// NotebookCapabilityArea — areas of notebook/execution functionality
// ============================================================================

enum class NotebookCapabilityArea : uint8_t
{
    kKernelLifecycle,      ///< Spawn, stop, restart, interrupt
    kKernelTransport,      ///< ZeroMQ / message transport
    kCellExecution,        ///< Execute, output streaming, error handling
    kOutputRendering,      ///< Rich output MIME rendering
    kAICompletion,         ///< AI inline completion, chat, generation
    kAIDocumentGen,        ///< AI document and outline generation
    kVariableInspector,    ///< Variable state inspection
    kExecutionHistory,     ///< Execution history tracking
};

[[nodiscard]] constexpr auto notebook_capability_label(NotebookCapabilityArea area) -> const char*
{
    switch (area)
    {
    case NotebookCapabilityArea::kKernelLifecycle:   return "KernelLifecycle";
    case NotebookCapabilityArea::kKernelTransport:   return "KernelTransport";
    case NotebookCapabilityArea::kCellExecution:     return "CellExecution";
    case NotebookCapabilityArea::kOutputRendering:   return "OutputRendering";
    case NotebookCapabilityArea::kAICompletion:      return "AICompletion";
    case NotebookCapabilityArea::kAIDocumentGen:     return "AIDocumentGen";
    case NotebookCapabilityArea::kVariableInspector: return "VariableInspector";
    case NotebookCapabilityArea::kExecutionHistory:  return "ExecutionHistory";
    }
    return "Unknown";
}

// ============================================================================
// NotebookCompletionItem — evidence for a capability
// ============================================================================

struct NotebookCompletionItem
{
    NotebookCapabilityArea area{NotebookCapabilityArea::kKernelLifecycle};
    std::string feature_name;
    bool is_real_transport{false};     ///< Not simulated
    bool has_error_handling{false};
    bool has_process_supervision{false};
    std::string evidence_file;
    int evidence_line{0};

    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return is_real_transport && has_error_handling;
    }
};

// ============================================================================
// NotebookCompletionReport
// ============================================================================

struct NotebookCompletionReport
{
    std::size_t total{0};
    std::size_t complete{0};
    std::size_t incomplete{0};
    std::size_t kernel_gaps{0};
    std::size_t execution_gaps{0};
    std::size_t ai_gaps{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    {
        return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0;
    }
};

// ============================================================================
// NotebookExecutionCompletionAuditor
// ============================================================================

class NotebookExecutionCompletionAuditor
{
public:
    NotebookExecutionCompletionAuditor() = default;

    void add_item(NotebookCompletionItem item);
    void add_items(std::vector<NotebookCompletionItem> items);

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(NotebookCapabilityArea area) const
        -> std::vector<const NotebookCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const NotebookCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const NotebookCompletionItem*>;

    [[nodiscard]] auto report() const -> NotebookCompletionReport;

    void clear();

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<NotebookCompletionItem> items_;
};

} // namespace markamp::core
