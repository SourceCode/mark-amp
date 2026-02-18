/// @file AssetLinkIntegrity.h
/// @brief V9 Phase 37 — Broken asset link detection and repair.
#pragma once

#include "AssetTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A broken asset link found in a document.
struct BrokenAssetLink
{
    std::string document_id;   // Document containing the broken link
    int line_number{0};        // Line number in the document
    std::string link_text;     // The original link text (e.g., "![alt](path)")
    std::string expected_path; // The referenced path
    std::string suggestion;    // Suggested replacement path
    double confidence{0.0};    // Confidence of suggestion (0.0 - 1.0)
};

/// Summary report of link integrity scan.
struct LinkIntegrityReport
{
    int total_links{0};
    int broken{0};
    int repaired{0};
    int suggestions{0};
    std::vector<BrokenAssetLink> broken_links;
};

/// Scans documents for broken asset links and suggests repairs.
class AssetLinkIntegrity
{
public:
    AssetLinkIntegrity() = default;

    /// Scan a document's content for broken asset links.
    [[nodiscard]] auto scan_document(const std::string& document_id,
                                     const std::string& content,
                                     const std::vector<AssetInfo>& available_assets) const
        -> std::vector<BrokenAssetLink>;

    /// Scan multiple documents for broken links.
    [[nodiscard]] auto scan_all(const std::vector<std::pair<std::string, std::string>>& documents,
                                const std::vector<AssetInfo>& available_assets) const
        -> LinkIntegrityReport;

    /// Suggest a repair for a broken link.
    [[nodiscard]] auto suggest_repair(const BrokenAssetLink& broken_link,
                                      const std::vector<AssetInfo>& available_assets) const
        -> std::string;

    /// Auto-repair broken links where confidence > threshold.
    [[nodiscard]] auto auto_repair(std::vector<BrokenAssetLink>& broken_links,
                                   const std::vector<AssetInfo>& available_assets,
                                   double confidence_threshold = 0.8) const -> int;

    /// Extract all image/asset links from markdown content.
    [[nodiscard]] static auto extract_asset_links(const std::string& content)
        -> std::vector<std::pair<int, std::string>>; // line_number, path

    /// Compute fuzzy similarity between two filenames (0.0 - 1.0).
    [[nodiscard]] static auto filename_similarity(const std::string& name_a,
                                                  const std::string& name_b) -> double;

private:
    /// Check if an asset path exists in available assets.
    [[nodiscard]] static auto find_asset_by_path(const std::string& path,
                                                 const std::vector<AssetInfo>& assets)
        -> const AssetInfo*;
};

} // namespace markamp::core
