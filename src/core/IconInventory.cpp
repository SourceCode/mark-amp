/// @file IconInventory.cpp
/// @brief P10-T01: Icon inventory and canonical MUI mapping.

#include "IconInventory.h"

#include "Logger.h"

namespace markamp::core
{

IconInventory::IconInventory()
{
    register_built_in_mappings();
}

void IconInventory::register_mapping(const IconInventoryEntry& entry)
{
    entries_.push_back(entry);
    if (!entry.mui_id.empty())
    {
        legacy_to_mui_[entry.legacy_id] = entry.mui_id;
    }
}

auto IconInventory::mui_for(const std::string& legacy_id) const -> std::string
{
    auto iter = legacy_to_mui_.find(legacy_id);
    if (iter != legacy_to_mui_.end())
    {
        return iter->second;
    }
    return legacy_id; // fallback to legacy ID
}

auto IconInventory::is_mapped(const std::string& legacy_id) const -> bool
{
    return legacy_to_mui_.contains(legacy_id);
}

auto IconInventory::mapped_count() const -> int
{
    int count = 0;
    for (const auto& entry : entries_)
    {
        if (entry.status == IconMigrationStatus::kMapped)
        {
            ++count;
        }
    }
    return count;
}

auto IconInventory::pending_count() const -> int
{
    int count = 0;
    for (const auto& entry : entries_)
    {
        if (entry.status == IconMigrationStatus::kPending)
        {
            ++count;
        }
    }
    return count;
}

void IconInventory::register_built_in_mappings()
{
    register_mapping({"explorer", "mui_folder_open", IconRole::kActivityBar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "ActivityBar"});
    register_mapping({"search", "mui_search", IconRole::kActivityBar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "ActivityBar"});
    register_mapping({"settings", "mui_settings", IconRole::kActivityBar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "ActivityBar"});
    register_mapping({"canvas", "mui_dashboard", IconRole::kActivityBar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "ActivityBar"});
    register_mapping({"notebook", "mui_code", IconRole::kActivityBar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "ActivityBar"});
    register_mapping({"save", "mui_save", IconRole::kToolbar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "Toolbar"});
    register_mapping({"undo", "mui_undo", IconRole::kToolbar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "Toolbar"});
    register_mapping({"redo", "mui_redo", IconRole::kToolbar,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "Toolbar"});
    register_mapping({"close", "mui_close", IconRole::kTab,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "TabBar"});
    register_mapping({"chevron_right", "mui_chevron_right", IconRole::kFileTree,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "FileTree"});
    register_mapping({"chevron_down", "mui_expand_more", IconRole::kFileTree,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "FileTree"});
    register_mapping({"file", "mui_description", IconRole::kFileTree,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "FileTree"});
    register_mapping({"folder", "mui_folder", IconRole::kFileTree,
                      IconMigrationStatus::kMapped, "", IconInventorySource::kMui, "FileTree"});

    MARKAMP_LOG_INFO("Icon inventory: {} entries ({} mapped)", total_count(), mapped_count());
}

} // namespace markamp::core
