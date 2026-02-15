#pragma once

/// @file OutlinePanelController.h
/// @brief V4 Phase 12 – Document Outline / Table-of-Contents panel.

#include "EventBus.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class VaultService;

// ============================================================================
// Outline Node
// ============================================================================

struct OutlineNode
{
    std::string text;
    int level{1};           // 1 = h1, 2 = h2, …
    int line_number{0};     // 0-based line in document
    bool is_expanded{true}; // UI state

    std::vector<OutlineNode> children;

    [[nodiscard]] auto child_count() const -> size_t;
    [[nodiscard]] auto total_count() const -> size_t;
};

// ============================================================================
// Outline Panel Controller
// ============================================================================

class OutlinePanelController
{
public:
    OutlinePanelController(EventBus& event_bus, VaultService& vault_service);

    /// Parse headings from markdown content and build an outline tree.
    [[nodiscard]] auto build_outline(const std::string& markdown) const -> std::vector<OutlineNode>;

    /// Get a flat list of all headings (depth-first order).
    [[nodiscard]] auto flatten(const std::vector<OutlineNode>& nodes) const
        -> std::vector<OutlineNode>;

    /// Find the outline node closest to a given line number.
    [[nodiscard]] auto find_active(const std::vector<OutlineNode>& nodes, int cursor_line) const
        -> std::optional<OutlineNode>;

    /// Update the outline for a given document.
    auto update_for_document(const std::string& document_id) -> void;

    /// Get current outline.
    [[nodiscard]] auto current_outline() const -> const std::vector<OutlineNode>&;

    /// Request scroll to a specific heading's line.
    auto scroll_to_heading(int line_number) -> void;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    std::vector<OutlineNode> current_outline_;
    std::string current_doc_id_;

    static auto insert_node(std::vector<OutlineNode>& root, const OutlineNode& node) -> void;
};

} // namespace markamp::core
