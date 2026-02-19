#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Level-of-detail mode.
enum class LodLevel : uint8_t
{
    kFull,
    kSimplified,
    kBoundingBox,
};

/// Render telemetry snapshot.
struct RenderTelemetry
{
    double frame_time_ms{0.0};
    int draw_calls{0};
    int visible_objects{0};
    int culled_objects{0};
};

/// Testable model for Large Board Rendering (Phase 69).
///
/// Encapsulates:
/// - Spatial culling viewport bounds
/// - LOD policy by zoom level
/// - Tile cache stats (hits, misses, evictions)
/// - Render telemetry counters
class LargeBoardModel
{
public:
    // ── Culling ─────────────────────────────────────────────────────

    void set_total_objects(int count);
    [[nodiscard]] auto total_objects() const -> int;

    void set_visible_count(int count);
    [[nodiscard]] auto visible_count() const -> int;
    [[nodiscard]] auto culled_count() const -> int;

    // ── LOD ─────────────────────────────────────────────────────────

    [[nodiscard]] auto lod_for_zoom(double zoom) const -> LodLevel;

    // ── Tile cache ──────────────────────────────────────────────────

    void record_cache_hit();
    void record_cache_miss();
    void record_cache_eviction();
    [[nodiscard]] auto cache_hits() const -> int;
    [[nodiscard]] auto cache_misses() const -> int;
    [[nodiscard]] auto cache_evictions() const -> int;
    [[nodiscard]] auto cache_hit_rate() const -> double;

    // ── Telemetry ───────────────────────────────────────────────────

    void set_telemetry(RenderTelemetry telemetry);
    [[nodiscard]] auto telemetry() const -> const RenderTelemetry&;

private:
    int total_objects_{0};
    int visible_count_{0};
    int cache_hits_{0};
    int cache_misses_{0};
    int cache_evictions_{0};
    RenderTelemetry telemetry_;
};

} // namespace markamp::canvas
