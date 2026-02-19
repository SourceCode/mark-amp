#pragma once

#include "NodeEditorTypes.h"
#include "NodeLayout.h"

#include <cstddef>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// LOD level for zoom-dependent rendering
// ---------------------------------------------------------------------------

enum class LodLevel
{
    kFull,       ///< Show everything: title, sockets, values, previews
    kSimplified, ///< Show title + sockets only
    kBlock,      ///< Show colored rectangle only
    kDot         ///< Show dot/point only (extreme zoom out)
};

// ---------------------------------------------------------------------------
// CullResult — output of viewport culling
// ---------------------------------------------------------------------------

struct CullResult
{
    std::vector<NodeId> visible_nodes;
    std::vector<LinkId> visible_links;
    LodLevel lod{LodLevel::kFull};

    /// Statistics
    std::size_t total_nodes{0};
    std::size_t total_links{0};
    std::size_t culled_nodes{0};
    std::size_t culled_links{0};

    [[nodiscard]] auto cull_ratio_nodes() const -> float
    {
        if (total_nodes == 0)
            return 0.0F;
        return static_cast<float>(culled_nodes) / static_cast<float>(total_nodes);
    }

    [[nodiscard]] auto cull_ratio_links() const -> float
    {
        if (total_links == 0)
            return 0.0F;
        return static_cast<float>(culled_links) / static_cast<float>(total_links);
    }
};

// ---------------------------------------------------------------------------
// ViewportCuller — determines visible elements for rendering
// ---------------------------------------------------------------------------

class ViewportCuller
{
public:
    /// Margin around viewport for pre-fetching (world units).
    explicit ViewportCuller(float margin = 50.0F);

    // --- Culling ----------------------------------------------------------

    [[nodiscard]] auto cull_nodes(const Rect& visible_rect,
                                  const std::vector<NodeLayoutResult>& layouts) const -> CullResult;

    /// Determine LOD level based on zoom factor.
    [[nodiscard]] static auto lod_for_zoom(float zoom) -> LodLevel;

    // --- Configuration ----------------------------------------------------

    void set_margin(float margin)
    {
        margin_ = margin;
    }
    [[nodiscard]] auto margin() const -> float
    {
        return margin_;
    }

    /// Zoom thresholds for LOD transitions.
    void set_lod_thresholds(float simplified, float block, float dot);

private:
    float margin_;
    float threshold_simplified_{0.4F};
    float threshold_block_{0.2F};
    float threshold_dot_{0.05F};
};

} // namespace markamp::node_editor
