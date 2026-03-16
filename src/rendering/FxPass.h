/// FxPass.h — Phase 46: Effect Pass Primitives
///
/// Defines the building blocks for the FX compositor pipeline.
/// Each pass represents a single visual effect that can be enabled,
/// parameterized, and quality-gated independently.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace markamp::rendering
{

/// Types of effect passes supported by the FX engine.
enum class FxPassType : uint8_t
{
    kBlur,         ///< Gaussian/box blur
    kGlow,         ///< Outer/inner glow
    kShadow,       ///< Drop shadow / soft shadow
    kDistortion,   ///< Wobbly/wave distortion
    kColorGrade,   ///< Color grading / tinting
    kComposite,    ///< Final compositing pass
    kStroke,       ///< Outline / stroke
    kBloom,        ///< Bloom / threshold glow
    kScanline,     ///< CRT scanline overlay
    kReflection,   ///< Surface reflection
    kChromaShift,  ///< Chromatic aberration
    kNoiseGrain,   ///< Film grain / noise
    kVignette,     ///< Radial edge darkening
    kFrostedGlass, ///< Frosted glass / vibrancy
};

/// Quality tiers that gate which passes are active.
enum class QualityTier : uint8_t
{
    kCinematic, ///< All effects enabled, highest quality
    kBalanced,  ///< Most effects, moderate GPU cost
    kEfficient, ///< Reduced effects, low GPU cost
    kMinimal,   ///< Near-zero effects, text clarity only
};

/// Identifies which surface an effect targets.
enum class FxSurfaceTarget : uint8_t
{
    kWindow,      ///< Window chrome / frame
    kToolbar,     ///< Toolbar area
    kTabBar,      ///< Tab bar area
    kStatusBar,   ///< Status bar area
    kEditorText,  ///< Editor text content
    kPreviewText, ///< Preview panel text
    kCanvasText,  ///< Canvas label text
    kOverlays,    ///< Tooltips, popups, overlays
    kAll,         ///< Applied to all surfaces
};

/// Configuration for a single effect pass.
struct FxPassConfig
{
    bool enabled{true};
    float intensity{1.0F};                         ///< 0.0–1.0 master intensity
    QualityTier min_tier{QualityTier::kCinematic}; ///< Minimum tier to activate
    FxSurfaceTarget target{FxSurfaceTarget::kAll};

    /// Arbitrary pass-specific parameters (e.g., "radius" -> 3.0).
    std::unordered_map<std::string, float> params;

    auto operator==(const FxPassConfig&) const -> bool = default;
};

/// A single compositing pass in the FX pipeline.
///
/// Passes are ordered and executed sequentially by the FxEngine.
/// Each pass can be enabled/disabled, quality-gated, and parameterized.
class FxPass
{
public:
    FxPass(std::string pass_name, FxPassType pass_type, FxPassConfig pass_config)
        : name_(std::move(pass_name))
        , type_(pass_type)
        , config_(std::move(pass_config))
    {
    }

    /// Execute this pass. Returns true if the pass produced output.
    /// Subclasses override for actual rendering; base returns enabled state.
    [[nodiscard]] auto execute() const -> bool
    {
        return config_.enabled;
    }

    // ── Accessors ──

    [[nodiscard]] auto name() const noexcept -> std::string_view
    {
        return name_;
    }
    [[nodiscard]] auto type() const noexcept -> FxPassType
    {
        return type_;
    }
    [[nodiscard]] auto config() const noexcept -> const FxPassConfig&
    {
        return config_;
    }

    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return config_.enabled;
    }
    [[nodiscard]] auto intensity() const noexcept -> float
    {
        return config_.intensity;
    }
    [[nodiscard]] auto min_tier() const noexcept -> QualityTier
    {
        return config_.min_tier;
    }
    [[nodiscard]] auto target() const noexcept -> FxSurfaceTarget
    {
        return config_.target;
    }

    void set_enabled(bool enabled) noexcept
    {
        config_.enabled = enabled;
    }
    void set_intensity(float value) noexcept
    {
        config_.intensity = value;
    }
    void set_min_tier(QualityTier tier) noexcept
    {
        config_.min_tier = tier;
    }
    void set_target(FxSurfaceTarget surface) noexcept
    {
        config_.target = surface;
    }

    /// Set a named parameter.
    void set_param(const std::string& key, float value)
    {
        config_.params[key] = value;
    }

    /// Get a named parameter (returns 0.0 if not set).
    [[nodiscard]] auto get_param(const std::string& key) const -> float
    {
        auto iter = config_.params.find(key);
        return (iter != config_.params.end()) ? iter->second : 0.0F;
    }

    /// Check if this pass should run under the given quality tier.
    [[nodiscard]] auto is_active_at(QualityTier current_tier) const noexcept -> bool
    {
        return config_.enabled &&
               static_cast<uint8_t>(current_tier) <= static_cast<uint8_t>(config_.min_tier);
    }

    /// Number of named parameters on this pass.
    [[nodiscard]] auto param_count() const noexcept -> int
    {
        return static_cast<int>(config_.params.size());
    }

    /// Check if a named parameter exists.
    [[nodiscard]] auto has_param(const std::string& key) const -> bool
    {
        return config_.params.contains(key);
    }

    /// Whether this pass targets all surfaces.
    [[nodiscard]] auto targets_all() const noexcept -> bool
    {
        return config_.target == FxSurfaceTarget::kAll;
    }

    /// Convert pass type to display string.
    [[nodiscard]] static auto type_name(FxPassType pass_type) -> std::string_view
    {
        switch (pass_type)
        {
            case FxPassType::kBlur:
                return "Blur";
            case FxPassType::kGlow:
                return "Glow";
            case FxPassType::kShadow:
                return "Shadow";
            case FxPassType::kDistortion:
                return "Distortion";
            case FxPassType::kColorGrade:
                return "Color Grade";
            case FxPassType::kComposite:
                return "Composite";
            case FxPassType::kStroke:
                return "Stroke";
            case FxPassType::kBloom:
                return "Bloom";
            case FxPassType::kScanline:
                return "Scanline";
            case FxPassType::kReflection:
                return "Reflection";
            case FxPassType::kChromaShift:
                return "Chroma Shift";
            case FxPassType::kNoiseGrain:
                return "Noise Grain";
            case FxPassType::kVignette:
                return "Vignette";
            case FxPassType::kFrostedGlass:
                return "Frosted Glass";
        }
        return "Unknown";
    }

private:
    std::string name_;
    FxPassType type_;
    FxPassConfig config_;
};

} // namespace markamp::rendering
