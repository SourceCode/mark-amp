#pragma once
#include <wx/geometry.h>

#include <cstdint>

namespace markamp::ui::layout
{

enum class WorkbenchZoneId : uint8_t
{
    kActivityBar,
    kPrimarySidebar,
    kEditorArea,
    kSecondarySidebar,
    kPanelArea,
    kStatusBar,
    kCount
};

struct WorkbenchZoneConstraints
{
    int min_width{0};
    int max_width{10000};
    int min_height{0};
    int max_height{10000};
    int default_width{0};
    int default_height{0};
};

struct WorkbenchZoneState
{
    WorkbenchZoneId id;
    bool visible{true};
    int current_width{0};
    int current_height{0};
    int restored_width{0};  // Width before being hidden
    int restored_height{0}; // Height before being hidden
    wxRect bounds;          // Calculated absolute rect relative to root layout
};

} // namespace markamp::ui::layout
