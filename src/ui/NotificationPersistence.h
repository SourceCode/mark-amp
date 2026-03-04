#pragma once

#include "NotificationCenterModel.h"

#include <string>

namespace markamp::ui
{

/// Phase 43: Persist notification history to JSON.
class NotificationPersistence
{
public:
    explicit NotificationPersistence(std::string storage_path);

    /// Save current notifications to disk.
    auto save(const NotificationCenterModel& model) -> bool;

    /// Load notifications from disk into model.
    auto load(NotificationCenterModel& model) -> bool;

    /// Clear the persisted file.
    void clear();

    [[nodiscard]] auto storage_path() const -> const std::string&
    {
        return path_;
    }

private:
    std::string path_;
};

} // namespace markamp::ui
