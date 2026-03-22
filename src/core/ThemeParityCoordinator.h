#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 18: Theme parity coordination service.
///
/// Provides token coverage auditing, theme calibration checks,
/// light/high-contrast parity validation, and token usage reporting.
class ThemeParityCoordinator
{
public:
    /// Theme variant for parity checking.
    enum class ThemeVariant : uint8_t
    {
        kDark,
        kLight,
        kHighContrastDark,
        kHighContrastLight,
    };

    /// Token coverage audit result.
    struct TokenCoverageResult
    {
        int total_tokens{0};
        int themed_tokens{0};           /// Tokens with proper theme-aware values
        int hardcoded_tokens{0};        /// Tokens with hardcoded values
        float coverage_percent{0.0F};
    };

    /// Theme calibration check result.
    struct CalibrationResult
    {
        std::string surface_name;
        bool passes_contrast{false};
        bool passes_readability{false};
        bool passes_hierarchy{false};
        float contrast_ratio{0.0F};
    };

    /// Theme parity check summary.
    struct ParityCheckSummary
    {
        int surfaces_checked{0};
        int surfaces_passing{0};
        int contrast_failures{0};
        int readability_failures{0};
        int hierarchy_failures{0};
        float parity_score{0.0F};
    };

    /// Theme preview configuration.
    struct ThemePreviewConfig
    {
        int preview_width{240};
        int preview_height{160};
        int color_swatch_size{16};
        int swatch_gap{4};
        int sample_code_lines{8};
        CornerRadiusToken preview_corner{CornerRadiusToken::kMd};
        BorderWeightToken preview_border{BorderWeightToken::kThin};
        TypeScaleToken sample_font{TypeScaleToken::kCaption};
    };

    ThemeParityCoordinator() = default;

    [[nodiscard]] auto theme_preview_config() const -> ThemePreviewConfig
    {
        return preview_;
    }

    /// Run a mock token coverage audit.
    [[nodiscard]] auto audit_token_coverage() const -> TokenCoverageResult;

    /// Run a mock parity check across all surfaces.
    [[nodiscard]] auto check_parity(ThemeVariant variant) const -> ParityCheckSummary;

private:
    ThemePreviewConfig preview_;
};

} // namespace markamp::core
