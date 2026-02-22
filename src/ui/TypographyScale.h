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
    kMonoSmall
};

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
