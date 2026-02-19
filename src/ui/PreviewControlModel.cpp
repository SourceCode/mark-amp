#include "PreviewControlModel.h"

namespace markamp::ui
{

void PreviewControlModel::set_sync_mode(PreviewSyncMode mode)
{
    sync_mode_ = mode;
}
auto PreviewControlModel::sync_mode() const -> PreviewSyncMode
{
    return sync_mode_;
}

void PreviewControlModel::cycle_sync_mode()
{
    switch (sync_mode_)
    {
        case PreviewSyncMode::kFollowCursor:
            sync_mode_ = PreviewSyncMode::kFollowHeading;
            break;
        case PreviewSyncMode::kFollowHeading:
            sync_mode_ = PreviewSyncMode::kLocked;
            break;
        case PreviewSyncMode::kLocked:
            sync_mode_ = PreviewSyncMode::kFollowCursor;
            break;
    }
}

auto PreviewControlModel::sync_mode_label() const -> std::string
{
    switch (sync_mode_)
    {
        case PreviewSyncMode::kFollowCursor:
            return "Follow Cursor";
        case PreviewSyncMode::kFollowHeading:
            return "Follow Heading";
        case PreviewSyncMode::kLocked:
            return "Locked";
    }
    return "Unknown";
}

void PreviewControlModel::set_scroll_anchor(ScrollAnchor anchor)
{
    scroll_anchor_ = anchor;
}
auto PreviewControlModel::scroll_anchor() const -> ScrollAnchor
{
    return scroll_anchor_;
}

void PreviewControlModel::set_pinned(bool pinned)
{
    pinned_ = pinned;
}
auto PreviewControlModel::is_pinned() const -> bool
{
    return pinned_;
}

void PreviewControlModel::set_live_refresh(bool enabled)
{
    live_refresh_ = enabled;
}
auto PreviewControlModel::live_refresh() const -> bool
{
    return live_refresh_;
}

void PreviewControlModel::set_stale(bool stale)
{
    stale_ = stale;
}
auto PreviewControlModel::is_stale() const -> bool
{
    return stale_;
}

} // namespace markamp::ui
