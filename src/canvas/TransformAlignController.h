#pragma once

/**
 * @file TransformAlignController.h
 * @brief Phase 48 Task 3-4: Alignment, distribution, and transform pivot.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Alignment command.
enum class AlignCommand : uint8_t
{
    kLeft,
    kRight,
    kTop,
    kBottom,
    kCenterH,
    kCenterV,
};

/// Distribution command.
enum class DistributeCommand : uint8_t
{
    kHorizontal,
    kVertical,
};

/// An item with position/size for alignment.
struct AlignItem
{
    std::string object_id;
    double pos_x{0.0};
    double pos_y{0.0};
    double width{0.0};
    double height{0.0};
};

/**
 * @brief Computes alignment and distribution transforms.
 */
class TransformAlignController
{
public:
    TransformAlignController() = default;

    /// Get alignment command as string.
    [[nodiscard]] static auto command_name(AlignCommand cmd) -> std::string;

    // ── Alignment ──────────────────────────────────────────────────

    /// Compute aligned positions for items. Returns new positions.
    [[nodiscard]] static auto align(const std::vector<AlignItem>& items, AlignCommand command)
        -> std::vector<AlignItem>;

    // ── Distribution ───────────────────────────────────────────────

    /// Compute distributed positions for items. Returns new positions.
    [[nodiscard]] static auto distribute(const std::vector<AlignItem>& items,
                                         DistributeCommand command) -> std::vector<AlignItem>;

    // ── Pivot ──────────────────────────────────────────────────────

    /// Set custom transform pivot.
    void set_pivot(double pos_x, double pos_y);

    /// Get pivot X.
    [[nodiscard]] auto pivot_x() const -> double;

    /// Get pivot Y.
    [[nodiscard]] auto pivot_y() const -> double;

    /// Reset pivot to default (center).
    void reset_pivot();

private:
    double pivot_x_{0.0};
    double pivot_y_{0.0};
};

} // namespace markamp::canvas
