#include "ComponentSizeResolver.h"

namespace markamp::ui
{

auto ComponentSizeResolver::get() -> ComponentSizeResolver&
{
    static ComponentSizeResolver instance;
    return instance;
}

ComponentSizeResolver::ComponentSizeResolver()
{
    InitializeTable();
}

void ComponentSizeResolver::set_density(DensityProfile profile)
{
    density_ = profile;
}

auto ComponentSizeResolver::resolve(ComponentKind kind) const -> ComponentMetrics
{
    size_t d_idx = 1; // Default
    if (density_ == DensityProfile::kCompact)
        d_idx = 0;
    else if (density_ == DensityProfile::kComfortable)
        d_idx = 2;

    size_t k_idx = static_cast<size_t>(kind);
    return metrics_table_[d_idx][k_idx];
}

void ComponentSizeResolver::InitializeTable()
{
    // Indexes: 0 = Compact, 1 = Default, 2 = Comfortable

    // kButton
    metrics_table_[0][static_cast<size_t>(ComponentKind::kButton)] = {28, 64, 14, 8, 2};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kButton)] = {32, 72, 16, 12, 4};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kButton)] = {36, 80, 18, 16, 6};

    // kInput
    metrics_table_[0][static_cast<size_t>(ComponentKind::kInput)] = {24, 100, 14, 6, 2};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kInput)] = {28, 120, 16, 8, 4};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kInput)] = {32, 140, 18, 10, 6};

    // kTab
    metrics_table_[0][static_cast<size_t>(ComponentKind::kTab)] = {32, 80, 14, 8, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kTab)] = {36, 100, 16, 12, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kTab)] = {40, 120, 18, 16, 0};

    // kTreeRow
    metrics_table_[0][static_cast<size_t>(ComponentKind::kTreeRow)] = {20, 0, 14, 2, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kTreeRow)] = {24, 0, 16, 4, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kTreeRow)] = {28, 0, 18, 6, 0};

    // kListRow
    metrics_table_[0][static_cast<size_t>(ComponentKind::kListRow)] = {20, 0, 14, 4, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kListRow)] = {24, 0, 16, 8, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kListRow)] = {28, 0, 18, 12, 0};

    // kToolbarButton
    metrics_table_[0][static_cast<size_t>(ComponentKind::kToolbarButton)] = {24, 24, 14, 4, 4};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kToolbarButton)] = {28, 28, 16, 6, 6};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kToolbarButton)] = {32, 32, 18, 8, 8};

    // kActivityBarSlot
    metrics_table_[0][static_cast<size_t>(ComponentKind::kActivityBarSlot)] = {40, 40, 20, 0, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kActivityBarSlot)] = {48, 48, 24, 0, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kActivityBarSlot)] = {56, 56, 28, 0, 0};

    // kStatusBarSegment
    metrics_table_[0][static_cast<size_t>(ComponentKind::kStatusBarSegment)] = {20, 0, 12, 4, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kStatusBarSegment)] = {24, 0, 14, 6, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kStatusBarSegment)] = {28, 0, 16, 8, 0};

    // kBreadcrumbSegment
    metrics_table_[0][static_cast<size_t>(ComponentKind::kBreadcrumbSegment)] = {20, 0, 12, 2, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kBreadcrumbSegment)] = {24, 0, 14, 4, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kBreadcrumbSegment)] = {28, 0, 16, 6, 0};

    // kPanelHeader
    metrics_table_[0][static_cast<size_t>(ComponentKind::kPanelHeader)] = {28, 0, 14, 8, 0};
    metrics_table_[1][static_cast<size_t>(ComponentKind::kPanelHeader)] = {32, 0, 16, 12, 0};
    metrics_table_[2][static_cast<size_t>(ComponentKind::kPanelHeader)] = {36, 0, 18, 16, 0};
}

} // namespace markamp::ui
