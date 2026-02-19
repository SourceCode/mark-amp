#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Stencil category.
enum class StencilCategory : uint8_t
{
    kFlowchart,
    kArchitecture,
    kUiWireframe,
    kGeneral,
};

/// A stencil definition.
struct StencilDef
{
    std::string stencil_id;
    std::string name;
    StencilCategory category{StencilCategory::kGeneral};
    std::string default_fill{"#FFFFFF"};
    std::string default_stroke{"#333333"};
};

/// Testable model for Diagram Library & Stencil System (Phase 57).
///
/// Encapsulates:
/// - Stencil catalog with categories
/// - Search/filter
/// - Favorites and recents
/// - Insert-time style override option
class StencilModel
{
public:
    // ── Catalog ─────────────────────────────────────────────────────

    void set_catalog(std::vector<StencilDef> stencils);
    [[nodiscard]] auto catalog() const -> const std::vector<StencilDef>&;
    [[nodiscard]] auto catalog_size() const -> int;

    // ── Search ──────────────────────────────────────────────────────

    [[nodiscard]] auto search(const std::string& query) const -> std::vector<StencilDef>;
    [[nodiscard]] auto by_category(StencilCategory category) const -> std::vector<StencilDef>;

    // ── Favorites ───────────────────────────────────────────────────

    void add_favorite(const std::string& stencil_id);
    void remove_favorite(const std::string& stencil_id);
    [[nodiscard]] auto favorites() const -> const std::vector<std::string>&;
    [[nodiscard]] auto is_favorite(const std::string& stencil_id) const -> bool;

    // ── Recents ─────────────────────────────────────────────────────

    void push_recent(const std::string& stencil_id);
    [[nodiscard]] auto recents() const -> const std::vector<std::string>&;

    // ── Insert override ─────────────────────────────────────────────

    void set_override_fill(const std::string& color);
    void set_override_stroke(const std::string& color);
    void clear_overrides();
    [[nodiscard]] auto override_fill() const -> const std::string&;
    [[nodiscard]] auto override_stroke() const -> const std::string&;
    [[nodiscard]] auto has_overrides() const -> bool;

private:
    std::vector<StencilDef> catalog_;
    std::vector<std::string> favorites_;
    std::vector<std::string> recents_;
    static constexpr int kMaxRecent = 8;
    std::string override_fill_;
    std::string override_stroke_;
};

} // namespace markamp::canvas
