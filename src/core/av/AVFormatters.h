#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core::av
{

[[nodiscard]] auto format_number(double value,
                                 AVNumberFormat format,
                                 const std::string& custom_format = "") -> std::string;
[[nodiscard]] auto format_date(int64_t timestamp, bool is_not_time = false) -> std::string;
[[nodiscard]] auto format_date_range(int64_t start, int64_t end_ts, bool is_not_time = false)
    -> std::string;
[[nodiscard]] auto format_select(const AVSelectOption& option) -> std::string;
[[nodiscard]] auto format_mselect(const std::vector<AVSelectOption>& options) -> std::string;
[[nodiscard]] auto smart_url(const std::string& full_url) -> std::string;
[[nodiscard]] auto format_asset(const AVValueAssetEntry& asset) -> std::string;
[[nodiscard]] auto format_masset(const std::vector<AVValueAssetEntry>& assets) -> std::string;
[[nodiscard]] auto format_value_for_display(const AVValue& value, const AVKey& key) -> std::string;

} // namespace markamp::core::av
