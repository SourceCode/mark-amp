#include "TelemetryPrivacyController.h"

namespace markamp::ui
{

auto RetentionPolicy::is_valid() const -> bool
{
    return max_events > 0 && max_age_hours > 0;
}

void TelemetryPrivacyController::set_tier(PrivacyTier tier)
{
    tier_ = tier;
}

auto TelemetryPrivacyController::tier() const -> PrivacyTier
{
    return tier_;
}

auto TelemetryPrivacyController::tier_name() const -> std::string
{
    switch (tier_)
    {
        case PrivacyTier::kOff:
            return "off";
        case PrivacyTier::kAnonymous:
            return "anonymous";
        case PrivacyTier::kDetailed:
            return "detailed";
    }
    return "unknown";
}

auto TelemetryPrivacyController::is_active() const -> bool
{
    return tier_ != PrivacyTier::kOff && user_consent_;
}

auto TelemetryPrivacyController::allows_context() const -> bool
{
    return tier_ == PrivacyTier::kDetailed && user_consent_;
}

void TelemetryPrivacyController::set_user_consent(bool consented)
{
    user_consent_ = consented;
}

auto TelemetryPrivacyController::has_user_consent() const -> bool
{
    return user_consent_;
}

void TelemetryPrivacyController::set_retention(const RetentionPolicy& policy)
{
    retention_ = policy;
}

auto TelemetryPrivacyController::retention() const -> const RetentionPolicy&
{
    return retention_;
}

void TelemetryPrivacyController::register_field(const MetricFieldSchema& field)
{
    fields_.push_back(field);
}

auto TelemetryPrivacyController::fields() const -> const std::vector<MetricFieldSchema>&
{
    return fields_;
}

auto TelemetryPrivacyController::is_pii_field(const std::string& field_name) const -> bool
{
    for (const auto& field : fields_)
    {
        if (field.field_name == field_name)
        {
            return field.pii_sensitive;
        }
    }
    return false;
}

auto TelemetryPrivacyController::pii_field_count() const -> int
{
    int count = 0;
    for (const auto& field : fields_)
    {
        if (field.pii_sensitive)
        {
            ++count;
        }
    }
    return count;
}

auto TelemetryPrivacyController::pii_allowed() const -> bool
{
    return tier_ == PrivacyTier::kDetailed;
}

} // namespace markamp::ui
