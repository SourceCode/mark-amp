#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 07: Syntax presentation coordination service.
///
/// Manages token hierarchy calibration, comment/string styling,
/// markdown presentation, semantic overlays, and degradation paths.
class SyntaxPresentationCoordinator
{
public:
    /// Token hierarchy priority levels for syntax coloring.
    enum class TokenPriority : uint8_t
    {
        kKeyword   = 0, /// Highest visual weight — structural keywords
        kType      = 1, /// Type names and annotations
        kFunction  = 2, /// Function/method names
        kVariable  = 3, /// Variables and parameters
        kOperator  = 4, /// Operators and punctuation
        kLiteral   = 5, /// Number and boolean literals
        kString    = 6, /// String literals
        kComment   = 7, /// Comments — lowest visual weight
    };

    /// Token visual weight configuration.
    struct TokenWeightConfig
    {
        TokenPriority priority{TokenPriority::kVariable};
        bool is_bold{false};
        bool is_italic{false};
        float opacity{1.0F};  /// Token text opacity (1.0 = full, <1.0 = muted)
    };

    /// Comment styling configuration.
    struct CommentStyle
    {
        bool is_italic{true};
        float opacity{0.65F};         /// Comment text opacity
        bool dim_line_comments{true};  /// Dim single-line comments more than block
        float dim_factor{0.85F};       /// Additional dim factor for line comments
    };

    /// Code fence / markdown presentation configuration.
    struct CodeFenceStyle
    {
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        BorderWeightToken border{BorderWeightToken::kThin};
        int padding_h{12};
        int padding_v{8};
        TypeScaleToken font{TypeScaleToken::kBody};
        float bg_opacity{0.5F}; /// Code block background opacity
    };

    /// Degradation path for large files or low-power modes.
    struct DegradationConfig
    {
        int large_file_threshold_lines{10000};   /// Lines above which degradation kicks in
        int very_large_threshold_lines{100000};
        bool disable_semantic_tokens_on_large{true};
        bool disable_bracket_matching_on_large{true};
        bool disable_minimap_on_very_large{true};
        bool reduce_highlight_scope_on_large{true};
    };

    SyntaxPresentationCoordinator() = default;

    /// Get visual weight config for a token priority.
    [[nodiscard]] auto token_weight(TokenPriority priority) const -> TokenWeightConfig;

    /// Comment styling config.
    [[nodiscard]] auto comment_style() const -> CommentStyle
    {
        return comment_style_;
    }

    /// Code fence presentation.
    [[nodiscard]] auto code_fence_style() const -> CodeFenceStyle
    {
        return code_fence_;
    }

    /// Degradation config.
    [[nodiscard]] auto degradation_config() const -> DegradationConfig
    {
        return degradation_;
    }

    /// Check if a file should use degraded rendering.
    [[nodiscard]] auto should_degrade(int line_count) const -> bool
    {
        return line_count >= degradation_.large_file_threshold_lines;
    }

    /// Check if a file is "very large" (maximum degradation).
    [[nodiscard]] auto is_very_large(int line_count) const -> bool
    {
        return line_count >= degradation_.very_large_threshold_lines;
    }

private:
    CommentStyle comment_style_;
    CodeFenceStyle code_fence_;
    DegradationConfig degradation_;
};

} // namespace markamp::core
