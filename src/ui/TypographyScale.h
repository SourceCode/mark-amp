#pragma once

#include "DensityProfile.h"

#include <wx/font.h>

#include <string>

namespace markamp::ui
{

enum class TypeSlot
{
    kCaption,
    kBody,
    kBodyStrong,
    kSubtitle,
    kTitle,
    kHeadline,
    kMono,
    kMonoSmall,

    // V26 Phase 01: Premium typography role slots
    kRowDescription,    /// 11px — settings help text, secondary metadata
    kBadgeLabel,        /// 10px bold — counts, tags, inline chips
    kButtonLabel,       /// 12px semibold — action button text
    kEmptyStateTitle,   /// 16px bold — empty panel headings
    kEmptyStateBody,    /// 12px — empty panel guidance text
};

/// Whether a type slot uses a monospace font family.
[[nodiscard]] constexpr auto type_slot_is_mono(TypeSlot slot) -> bool
{
    return slot == TypeSlot::kMono || slot == TypeSlot::kMonoSmall;
}

struct TypeSpec
{
    std::string family;
    int size_pt;
    int weight;
    int line_height_px;
};

class TypographyScale
{
public:
    TypographyScale() = default;

    TypeSpec resolve(TypeSlot slot) const;
    wxFont font(TypeSlot slot) const;

    void set_density(DensityProfile profile);
    void set_base_sans(const std::string& family);
    void set_base_mono(const std::string& family);

private:
    DensityProfile density_{DensityProfile::kDefault};
    std::string base_sans_{"Inter"};
    std::string base_mono_{"JetBrains Mono"};
};

} // namespace markamp::ui
