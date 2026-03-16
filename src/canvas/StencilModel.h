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

    // ── Round 6 Batch 9 (#81-86) ────────────────────────────────

    /// (#81) Whether a name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#82) Whether a default fill is set.
    [[nodiscard]] auto has_fill() const noexcept -> bool
    {
        return !default_fill.empty();
    }

    /// (#83) Whether a default stroke is set.
    [[nodiscard]] auto has_stroke() const noexcept -> bool
    {
        return !default_stroke.empty();
    }

    /// (#84) Whether category is flowchart.
    [[nodiscard]] auto is_flowchart() const noexcept -> bool
    {
        return category == StencilCategory::kFlowchart;
    }

    /// (#85) Whether category is architecture.
    [[nodiscard]] auto is_architecture() const noexcept -> bool
    {
        return category == StencilCategory::kArchitecture;
    }

    /// (#86) Whether category is UI wireframe.
    [[nodiscard]] auto is_wireframe() const noexcept -> bool
    {
        return category == StencilCategory::kUiWireframe;
    }
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

    // ── Round 6 Batch 9 (#87-90) ────────────────────────────────

    /// (#87) Whether favorites exist.
    [[nodiscard]] auto has_favorites() const noexcept -> bool
    {
        return !favorites_.empty();
    }

    /// (#88) Whether recents exist.
    [[nodiscard]] auto has_recents() const noexcept -> bool
    {
        return !recents_.empty();
    }

    /// (#89) Number of favorites.
    [[nodiscard]] auto favorite_count() const noexcept -> size_t
    {
        return favorites_.size();
    }

    /// (#90) Number of recents.
    [[nodiscard]] auto recent_count() const noexcept -> size_t
    {
        return recents_.size();
    }
};

} // namespace markamp::canvas
