#include "InspectorModel.h"

#include <algorithm>

namespace markamp::canvas
{

void InspectorModel::set_visible_sections(std::vector<InspectorSection> sections)
{
    visible_sections_ = std::move(sections);
}

auto InspectorModel::visible_sections() const -> const std::vector<InspectorSection>&
{
    return visible_sections_;
}

auto InspectorModel::is_section_visible(InspectorSection section) const -> bool
{
    return std::find(visible_sections_.begin(), visible_sections_.end(), section) !=
           visible_sections_.end();
}

void InspectorModel::set_selected_count(int count)
{
    selected_count_ = (count < 0) ? 0 : count;
}
auto InspectorModel::selected_count() const -> int
{
    return selected_count_;
}
auto InspectorModel::is_multi_select() const -> bool
{
    return selected_count_ > 1;
}

void InspectorModel::set_property(const std::string& key, InspectorPropertyValue value)
{
    for (auto& [prop_key, entry] : properties_)
    {
        if (prop_key == key)
        {
            entry.value = std::move(value);
            return;
        }
    }
    properties_.push_back({key, {std::move(value), ""}});
}

auto InspectorModel::property(const std::string& key) const -> InspectorPropertyValue
{
    for (const auto& [prop_key, entry] : properties_)
    {
        if (prop_key == key)
        {
            return entry.value;
        }
    }
    return {"", false};
}

void InspectorModel::reset_property(const std::string& key)
{
    for (auto& [prop_key, entry] : properties_)
    {
        if (prop_key == key)
        {
            entry.value = {entry.default_value, false};
            return;
        }
    }
}

void InspectorModel::set_constraint(const std::string& key, double min_val, double max_val)
{
    for (auto& [con_key, constraint] : constraints_)
    {
        if (con_key == key)
        {
            constraint = {min_val, max_val};
            return;
        }
    }
    constraints_.push_back({key, {min_val, max_val}});
}

auto InspectorModel::validate(const std::string& key, double value) const -> ValidationResult
{
    for (const auto& [con_key, constraint] : constraints_)
    {
        if (con_key == key)
        {
            if (value < constraint.min_val || value > constraint.max_val)
            {
                return ValidationResult::kOutOfRange;
            }
            return ValidationResult::kValid;
        }
    }
    return ValidationResult::kValid;
}

void InspectorModel::copy_style()
{
    has_copied_style_ = true;
}
auto InspectorModel::has_copied_style() const -> bool
{
    return has_copied_style_;
}

} // namespace markamp::canvas
