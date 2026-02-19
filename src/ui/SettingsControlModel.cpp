#include "SettingsControlModel.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <sstream>

namespace markamp::ui
{

void SettingsControlModel::add_constraint(SettingConstraint constraint)
{
    constraints_.push_back(std::move(constraint));
}

auto SettingsControlModel::find_constraint(const std::string& setting_id) const
    -> const SettingConstraint*
{
    for (const auto& constraint : constraints_)
    {
        if (constraint.setting_id == setting_id)
        {
            return &constraint;
        }
    }
    return nullptr;
}

auto SettingsControlModel::validate(const std::string& setting_id,
                                    const std::string& proposed_value) const -> SettingValidation
{
    const auto* constraint = find_constraint(setting_id);
    if (constraint == nullptr)
    {
        return {true, "", ""};
    }

    switch (constraint->value_type)
    {
        case SettingValueType::kBoolean:
        {
            if (proposed_value != "true" && proposed_value != "false")
            {
                return {false, "Must be true or false", "Use 'true' or 'false'"};
            }
            break;
        }
        case SettingValueType::kInteger:
        {
            int parsed_value = 0;
            auto [ptr, ec] = std::from_chars(
                proposed_value.data(), proposed_value.data() + proposed_value.size(), parsed_value);
            if (ec != std::errc{})
            {
                return {false, "Must be a whole number", "Enter a valid integer"};
            }
            if (parsed_value < constraint->int_min || parsed_value > constraint->int_max)
            {
                std::ostringstream hint;
                hint << "Must be between " << constraint->int_min << " and " << constraint->int_max;
                return {false, "Value out of range", hint.str()};
            }
            break;
        }
        case SettingValueType::kDouble:
        {
            double parsed_double = 0.0;
            try
            {
                parsed_double = std::stod(proposed_value);
            }
            catch (...)
            {
                return {false, "Must be a number", "Enter a valid decimal number"};
            }
            if (parsed_double < constraint->double_min || parsed_double > constraint->double_max)
            {
                std::ostringstream hint;
                hint << "Must be between " << constraint->double_min << " and "
                     << constraint->double_max;
                return {false, "Value out of range", hint.str()};
            }
            break;
        }
        case SettingValueType::kString:
        {
            if (proposed_value.empty())
            {
                return {false, "Value cannot be empty", "Enter a non-empty string"};
            }
            break;
        }
        case SettingValueType::kChoice:
        {
            const auto& valid = constraint->choices;
            if (std::find(valid.begin(), valid.end(), proposed_value) == valid.end())
            {
                std::ostringstream hint;
                hint << "Valid options: ";
                for (std::size_t idx = 0; idx < valid.size(); ++idx)
                {
                    if (idx > 0)
                    {
                        hint << ", ";
                    }
                    hint << valid[idx];
                }
                return {false, "Invalid option", hint.str()};
            }
            break;
        }
        case SettingValueType::kColor:
        {
            // Basic hex color validation: #RGB, #RRGGBB, #RRGGBBAA
            if (proposed_value.empty() || proposed_value[0] != '#')
            {
                return {false, "Must be a hex color", "Use format #RRGGBB"};
            }
            const auto hex_len = proposed_value.size() - 1;
            if (hex_len != 3 && hex_len != 6 && hex_len != 8)
            {
                return {false, "Invalid color length", "Use #RGB, #RRGGBB, or #RRGGBBAA"};
            }
            break;
        }
        case SettingValueType::kKeybinding:
        {
            if (proposed_value.empty())
            {
                return {false, "Keybinding cannot be empty", "Enter a key combination"};
            }
            break;
        }
    }

    return {true, "", ""};
}

auto SettingsControlModel::clamp_int(const std::string& setting_id, int value) const -> int
{
    const auto* constraint = find_constraint(setting_id);
    if (constraint == nullptr)
    {
        return value;
    }
    return std::clamp(value, constraint->int_min, constraint->int_max);
}

auto SettingsControlModel::clamp_double(const std::string& setting_id, double value) const -> double
{
    const auto* constraint = find_constraint(setting_id);
    if (constraint == nullptr)
    {
        return value;
    }
    return std::clamp(value, constraint->double_min, constraint->double_max);
}

void SettingsControlModel::stage_change(const std::string& setting_id,
                                        const std::string& old_value,
                                        const std::string& new_value)
{
    // Replace existing staged change for same setting
    for (auto& staged : staged_)
    {
        if (staged.setting_id == setting_id)
        {
            staged.new_value = new_value;
            return;
        }
    }
    staged_.push_back({setting_id, old_value, new_value});
}

auto SettingsControlModel::staged_changes() const -> const std::vector<StagedChange>&
{
    return staged_;
}

auto SettingsControlModel::pending_count() const -> int
{
    return static_cast<int>(staged_.size());
}

void SettingsControlModel::discard_all()
{
    staged_.clear();
}

void SettingsControlModel::revert(const std::string& setting_id)
{
    staged_.erase(std::remove_if(staged_.begin(),
                                 staged_.end(),
                                 [&setting_id](const StagedChange& change)
                                 { return change.setting_id == setting_id; }),
                  staged_.end());
}

auto SettingsControlModel::has_pending(const std::string& setting_id) const -> bool
{
    return std::any_of(staged_.begin(),
                       staged_.end(),
                       [&setting_id](const StagedChange& change)
                       { return change.setting_id == setting_id; });
}

auto SettingsControlModel::default_value(const std::string& setting_id) const -> std::string
{
    const auto* constraint = find_constraint(setting_id);
    if (constraint != nullptr)
    {
        return constraint->default_value;
    }
    return "";
}

} // namespace markamp::ui
