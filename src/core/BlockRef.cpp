#include "core/BlockRef.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>

namespace markamp::core
{

auto BlockId::is_valid() const -> bool
{
    // Format: "YYYYMMDDHHmmss-xxxxxxx" — 14 digits + dash + 7 alphanumeric = 22 chars
    if (value.size() != 22)
    {
        return false;
    }

    // First 14 chars must be digits
    for (size_t i = 0; i < 14; ++i)
    {
        if (std::isdigit(static_cast<unsigned char>(value[i])) == 0)
        {
            return false;
        }
    }

    // Position 14 must be '-'
    if (value[14] != '-')
    {
        return false;
    }

    // Last 7 chars must be alphanumeric
    for (size_t i = 15; i < 22; ++i)
    {
        if (std::isalnum(static_cast<unsigned char>(value[i])) == 0)
        {
            return false;
        }
    }

    return true;
}

auto BlockId::generate() -> BlockId
{
    // Timestamp portion: YYYYMMDDHHmmss
    auto now = std::chrono::system_clock::now();
    auto time_t_val = std::chrono::system_clock::to_time_t(now);

    std::tm tm_val{};
    localtime_r(&time_t_val, &tm_val);

    char timestamp[15]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", &tm_val);

    // Random suffix: 7 lowercase alphanumeric chars
    static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(alphanum) - 2));

    std::string suffix(7, ' ');
    for (auto& ch : suffix)
    {
        ch = alphanum[dist(rng)];
    }

    return BlockId{std::string(timestamp) + "-" + suffix};
}

// ============================================================================
// RefIndex Implementation
// ============================================================================

void RefIndex::add_ref(const BlockId& source_block, const BlockId& def_block, RefType /*type*/)
{
    forward_index_[def_block].insert(source_block);
    reverse_index_[source_block].insert(def_block);
    ++total_ref_count_;
}

void RefIndex::remove_refs_by_source(const BlockId& source_block)
{
    auto rev_it = reverse_index_.find(source_block);
    if (rev_it == reverse_index_.end())
    {
        return;
    }

    for (const auto& def_id : rev_it->second)
    {
        auto fwd_it = forward_index_.find(def_id);
        if (fwd_it != forward_index_.end())
        {
            fwd_it->second.erase(source_block);
            --total_ref_count_;
            if (fwd_it->second.empty())
            {
                forward_index_.erase(fwd_it);
            }
        }
    }

    reverse_index_.erase(rev_it);
}

auto RefIndex::get_refs_to(const BlockId& def_block) const -> std::vector<BlockId>
{
    auto it = forward_index_.find(def_block);
    if (it == forward_index_.end())
    {
        return {};
    }

    std::vector<BlockId> result;
    result.reserve(it->second.size());
    for (const auto& id : it->second)
    {
        result.push_back(id);
    }
    return result;
}

auto RefIndex::get_refs_from(const BlockId& source_block) const -> std::vector<BlockId>
{
    auto it = reverse_index_.find(source_block);
    if (it == reverse_index_.end())
    {
        return {};
    }

    std::vector<BlockId> result;
    result.reserve(it->second.size());
    for (const auto& id : it->second)
    {
        result.push_back(id);
    }
    return result;
}

auto RefIndex::ref_count(const BlockId& def_block) const -> std::size_t
{
    auto it = forward_index_.find(def_block);
    if (it == forward_index_.end())
    {
        return 0;
    }
    return it->second.size();
}

auto RefIndex::has_circular_ref(const BlockId& block_a, const BlockId& block_b) const -> bool
{
    // Check if A references B AND B references A
    auto a_refs = get_refs_from(block_a);
    auto b_refs = get_refs_from(block_b);

    bool a_refs_b =
        std::any_of(a_refs.begin(), a_refs.end(), [&](const BlockId& id) { return id == block_b; });
    bool b_refs_a =
        std::any_of(b_refs.begin(), b_refs.end(), [&](const BlockId& id) { return id == block_a; });

    return a_refs_b && b_refs_a;
}

void RefIndex::clear()
{
    forward_index_.clear();
    reverse_index_.clear();
    total_ref_count_ = 0;
}

auto RefIndex::total_refs() const -> std::size_t
{
    return total_ref_count_;
}

} // namespace markamp::core
