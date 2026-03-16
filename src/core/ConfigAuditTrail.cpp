#include "ConfigAuditTrail.h"

#include <algorithm>
#include <unordered_map>

namespace markamp::core
{

ConfigAuditTrail::ConfigAuditTrail(std::size_t max_entries)
    : max_entries_(max_entries)
{
    ring_.resize(max_entries_);
}

void ConfigAuditTrail::record(std::string key,
                              std::string old_value,
                              std::string new_value,
                              std::string source)
{
    ring_[head_] = AuditEntry{
        std::chrono::steady_clock::now(),
        std::move(key),
        std::move(old_value),
        std::move(new_value),
        std::move(source),
    };

    head_ = (head_ + 1) % max_entries_;
    if (count_ < max_entries_)
    {
        ++count_;
    }
}

auto ConfigAuditTrail::entries() const -> std::vector<AuditEntry>
{
    std::vector<AuditEntry> result;
    result.reserve(count_);

    if (count_ < max_entries_)
    {
        // Not wrapped yet — entries are 0..count_-1
        for (std::size_t idx = 0; idx < count_; ++idx)
        {
            result.push_back(ring_[idx]);
        }
    }
    else
    {
        // Wrapped — oldest is at head_, newest is at head_-1
        for (std::size_t idx = 0; idx < max_entries_; ++idx)
        {
            result.push_back(ring_[(head_ + idx) % max_entries_]);
        }
    }

    return result;
}

auto ConfigAuditTrail::size() const -> std::size_t
{
    return count_;
}

auto ConfigAuditTrail::capacity() const -> std::size_t
{
    return max_entries_;
}

void ConfigAuditTrail::clear()
{
    head_ = 0;
    count_ = 0;
}

auto ConfigAuditTrail::last_entry() const -> const AuditEntry*
{
    if (count_ == 0)
    {
        return nullptr;
    }
    std::size_t last_idx = (head_ == 0) ? max_entries_ - 1 : head_ - 1;
    return &ring_[last_idx];
}

// (#92) Filter entries for a specific config key.
auto ConfigAuditTrail::entries_for_key(const std::string& key) const -> std::vector<AuditEntry>
{
    std::vector<AuditEntry> result;
    auto all = entries();
    for (auto& entry : all)
    {
        if (entry.key == key)
        {
            result.push_back(std::move(entry));
        }
    }
    return result;
}

// ── Batch 19-22 improvements (#124-126) ──

auto ConfigAuditTrail::unique_key_count() const -> std::size_t
{
    std::vector<std::string> seen;
    auto all = entries();
    for (const auto& entry : all)
    {
        if (std::find(seen.begin(), seen.end(), entry.key) == seen.end())
        {
            seen.push_back(entry.key);
        }
    }
    return seen.size();
}

auto ConfigAuditTrail::is_full() const -> bool
{
    return count_ >= max_entries_;
}

auto ConfigAuditTrail::most_changed_key() const -> std::string
{
    if (count_ == 0)
    {
        return {};
    }

    std::unordered_map<std::string, int> counts;
    auto all = entries();
    for (const auto& entry : all)
    {
        ++counts[entry.key];
    }

    std::string best_key;
    int best_count = 0;
    for (const auto& [key_name, change_count] : counts)
    {
        if (change_count > best_count)
        {
            best_count = change_count;
            best_key = key_name;
        }
    }
    return best_key;
}

} // namespace markamp::core
