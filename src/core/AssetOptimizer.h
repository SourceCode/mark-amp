/// @file AssetOptimizer.h
/// @brief V9 Phase 37 — Asset optimization and storage reporting.
#pragma once

#include "AssetTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Result of optimizing a single asset.
struct OptimizationResult
{
    std::string asset_id;
    int64_t original_size{0};
    int64_t optimized_size{0};
    double savings_percent{0.0};
    bool optimized{false};
    std::string message; // Description of what was done
};

/// Storage usage report.
struct StorageReport
{
    int64_t used_bytes{0};
    int64_t optimizable_bytes{0};
    double savings_estimate_percent{0.0};
    int32_t total_assets{0};
    int32_t optimizable_assets{0};
    int32_t orphan_assets{0};
    int64_t orphan_bytes{0};
};

/// Optimizes asset storage: metadata stripping, dimension capping,
/// thumbnail generation, and storage reporting.
class AssetOptimizer
{
public:
    AssetOptimizer() = default;

    /// Set the maximum image dimension (width or height).
    void set_max_dimension(int32_t max_dim);

    /// Get the current max dimension cap.
    [[nodiscard]] auto max_dimension() const -> int32_t;

    /// Optimize a single image asset (metadata strip, dimension cap).
    [[nodiscard]] auto optimize_image(const AssetInfo& asset) const -> OptimizationResult;

    /// Batch-optimize multiple assets.
    [[nodiscard]] auto batch_optimize(const std::vector<AssetInfo>& assets) const
        -> std::vector<OptimizationResult>;

    /// Estimate savings without actually optimizing.
    [[nodiscard]] auto estimate_savings(const AssetInfo& asset) const -> OptimizationResult;

    /// Generate a thumbnail for an image asset.
    [[nodiscard]] auto generate_thumbnail(const AssetInfo& asset, int32_t max_dim) const
        -> std::string;

    /// Generate a full storage usage report.
    [[nodiscard]] auto generate_report(const std::vector<AssetInfo>& assets) const -> StorageReport;

    /// Check if an asset is eligible for optimization.
    [[nodiscard]] static auto is_optimizable(const AssetInfo& asset) -> bool;

private:
    int32_t max_dimension_{4096};
};

} // namespace markamp::core
