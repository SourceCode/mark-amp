/// @file CompletionSeverityPolicy.cpp
/// @brief V23 Phase 01 — CompletionSeverityPolicy implementation.

#include "CompletionSeverityPolicy.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ── Waiver Management ──

void CompletionSeverityPolicy::add_waiver(SeverityWaiver waiver)
{
    waivers_.push_back(std::move(waiver));
}

auto CompletionSeverityPolicy::waiver_count() const noexcept -> std::size_t
{
    return waivers_.size();
}

auto CompletionSeverityPolicy::has_waiver(std::string_view item_id) const -> bool
{
    return std::any_of(waivers_.begin(), waivers_.end(),
        [item_id](const auto& w) { return w.item_id == item_id; });
}

auto CompletionSeverityPolicy::get_waiver(std::string_view item_id) const
    -> const SeverityWaiver*
{
    for (const auto& w : waivers_)
        if (w.item_id == item_id)
            return &w;
    return nullptr;
}

auto CompletionSeverityPolicy::all_waivers() const
    -> std::vector<const SeverityWaiver*>
{
    std::vector<const SeverityWaiver*> result;
    result.reserve(waivers_.size());
    for (const auto& w : waivers_)
        result.push_back(&w);
    return result;
}

void CompletionSeverityPolicy::clear_waivers()
{
    waivers_.clear();
}

// ── Severity Classification ──

auto CompletionSeverityPolicy::effective_severity(const InventoryItem& item) const
    -> CompletionSeverity
{
    auto key = waiver_key_for(item);
    const auto* waiver = get_waiver(key);
    if (waiver)
        return waiver->waived_severity;
    return item.severity;
}

auto CompletionSeverityPolicy::is_effective_blocker(const InventoryItem& item) const
    -> bool
{
    auto eff = effective_severity(item);
    return eff == CompletionSeverity::kCritical && item.is_production_path;
}

// ── Release Gate ──

auto CompletionSeverityPolicy::check_release_gate(
    const CompletionInventory& inventory) const -> GateResult
{
    GateResult result;
    result.total_items = inventory.item_count();

    auto all = inventory.all_items();
    for (const auto* item : all)
    {
        auto key = waiver_key_for(*item);
        bool waived = has_waiver(key);

        if (item->is_blocker())
        {
            ++result.blockers;
            if (waived)
            {
                ++result.waived;
            }
            else
            {
                ++result.effective_blockers;
                result.blocking_reasons.push_back(
                    item->file_path + ":" + std::to_string(item->line_number) +
                    " [" + std::string(marker_type_label(item->marker_type)) + "] " +
                    item->raw_text);
            }
        }
        else if (item->severity == CompletionSeverity::kHigh &&
                 item->is_production_path && !waived)
        {
            // Un-waived High production-path items also block
            ++result.effective_blockers;
            result.blocking_reasons.push_back(
                item->file_path + ":" + std::to_string(item->line_number) +
                " [HIGH] " + item->raw_text);
        }
    }

    result.passes = result.effective_blockers == 0;
    return result;
}

// ── Export ──

auto CompletionSeverityPolicy::export_gate_json(const GateResult& result) const
    -> std::string
{
    std::ostringstream ss;
    ss << "{\n"
       << "  \"passes\": " << (result.passes ? "true" : "false") << ",\n"
       << "  \"total_items\": " << result.total_items << ",\n"
       << "  \"blockers\": " << result.blockers << ",\n"
       << "  \"waived\": " << result.waived << ",\n"
       << "  \"effective_blockers\": " << result.effective_blockers << ",\n"
       << "  \"blocking_reasons\": [\n";

    for (std::size_t i = 0; i < result.blocking_reasons.size(); ++i)
    {
        ss << "    \"" << result.blocking_reasons[i] << "\"";
        if (i + 1 < result.blocking_reasons.size()) ss << ",";
        ss << "\n";
    }

    ss << "  ]\n}\n";
    return ss.str();
}

auto CompletionSeverityPolicy::export_gate_markdown(const GateResult& result) const
    -> std::string
{
    std::ostringstream ss;
    ss << "# V23 Release Gate\n\n"
       << "**Result:** " << (result.passes ? "✅ PASS" : "🚫 FAIL") << "\n\n"
       << "| Metric | Value |\n"
       << "|---|---|\n"
       << "| Total Items | " << result.total_items << " |\n"
       << "| Blockers | " << result.blockers << " |\n"
       << "| Waived | " << result.waived << " |\n"
       << "| Effective Blockers | " << result.effective_blockers << " |\n\n";

    if (!result.blocking_reasons.empty())
    {
        ss << "## Blocking Reasons\n\n";
        for (const auto& reason : result.blocking_reasons)
            ss << "- " << reason << "\n";
    }

    return ss.str();
}

// ── Private ──

auto CompletionSeverityPolicy::waiver_key_for(const InventoryItem& item)
    -> std::string
{
    return item.file_path + ":" + std::to_string(item.line_number);
}

} // namespace markamp::core
