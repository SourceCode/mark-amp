#include "TypographyScale.h"

namespace markamp::ui
{

TypeSpec TypographyScale::resolve(TypeSlot slot) const
{
    TypeSpec spec;

    // Determine family
    if (slot == TypeSlot::kMono || slot == TypeSlot::kMonoSmall)
    {
        spec.family = base_mono_;
    }
    else
    {
        spec.family = base_sans_;
    }

    // Determine weight
    if (slot == TypeSlot::kBodyStrong || slot == TypeSlot::kSubtitle || slot == TypeSlot::kTitle ||
        slot == TypeSlot::kHeadline || slot == TypeSlot::kBadgeLabel ||
        slot == TypeSlot::kButtonLabel || slot == TypeSlot::kEmptyStateTitle)
    {
        spec.weight = wxFONTWEIGHT_BOLD;
    }
    else
    {
        spec.weight = wxFONTWEIGHT_NORMAL;
    }

    // Base sizes
    int base_size = 13;
    switch (slot)
    {
        case TypeSlot::kCaption:
            base_size = 11;
            break;
        case TypeSlot::kMonoSmall:
            base_size = 11;
            break;
        case TypeSlot::kBody:
        case TypeSlot::kBodyStrong:
        case TypeSlot::kMono:
            base_size = 13;
            break;
        case TypeSlot::kSubtitle:
            base_size = 15;
            break;
        case TypeSlot::kTitle:
            base_size = 18;
            break;
        case TypeSlot::kHeadline:
            base_size = 24;
            break;
        // V26 Phase 01: Premium typography role slots
        case TypeSlot::kRowDescription:
            base_size = 11;
            break;
        case TypeSlot::kBadgeLabel:
            base_size = 10;
            break;
        case TypeSlot::kButtonLabel:
            base_size = 12;
            break;
        case TypeSlot::kEmptyStateTitle:
            base_size = 16;
            break;
        case TypeSlot::kEmptyStateBody:
            base_size = 12;
            break;
    }

    // Density adjustment
    if (density_ == DensityProfile::kCompact)
    {
        spec.size_pt = base_size - 1;
    }
    else if (density_ == DensityProfile::kComfortable)
    {
        spec.size_pt = base_size + 1;
    }
    else
    {
        spec.size_pt = base_size;
    }

    // Line heights (approx 1.4-1.5x)
    spec.line_height_px = static_cast<int>(spec.size_pt * 1.4f);

    return spec;
}

wxFont TypographyScale::font(TypeSlot slot) const
{
    TypeSpec spec = resolve(slot);
    return wxFont(wxFontInfo(spec.size_pt)
                      .FaceName(spec.family)
                      .Weight(static_cast<wxFontWeight>(spec.weight)));
}

void TypographyScale::set_density(DensityProfile profile)
{
    density_ = profile;
}

void TypographyScale::set_base_sans(const std::string& family)
{
    if (!family.empty())
    {
        base_sans_ = family;
    }
}

void TypographyScale::set_base_mono(const std::string& family)
{
    if (!family.empty())
    {
        base_mono_ = family;
    }
}

} // namespace markamp::ui
