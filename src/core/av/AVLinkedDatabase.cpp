#include "AVLinkedDatabase.h"

#include "AVColumnType.h"
#include "AVQueryEngine.h"

#include <chrono>
#include <random>
#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Constructor
// ════════════════════════════════════════════════════════════

AVLinkedDatabase::AVLinkedDatabase(AttributeViewStore& store)
    : store_(store)
{
}

// ════════════════════════════════════════════════════════════
// ID generation
// ════════════════════════════════════════════════════════════

auto AVLinkedDatabase::generate_link_id() -> std::string
{
    auto now = std::chrono::steady_clock::now();
    auto seed = static_cast<unsigned int>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(0, 15);

    std::ostringstream oss;
    oss << "link_";
    for (int idx = 0; idx < 8; ++idx)
    {
        oss << std::hex << dist(gen);
    }
    return oss.str();
}

// ════════════════════════════════════════════════════════════
// Create link
// ════════════════════════════════════════════════════════════

auto AVLinkedDatabase::create_link(const std::string& source_av_id, const std::string& view_name)
    -> AVLinkedConfig
{
    AVLinkedConfig config;
    config.link_id = generate_link_id();
    config.source_av_id = source_av_id;
    config.view_name = view_name.empty() ? "Linked View" : view_name;
    links_.push_back(config);
    return config;
}

// ════════════════════════════════════════════════════════════
// Override setters
// ════════════════════════════════════════════════════════════

void AVLinkedDatabase::set_filter_override(AVLinkedConfig& config, AVFilterGroup filters)
{
    config.filter_overrides = std::move(filters);
}

void AVLinkedDatabase::set_sort_override(AVLinkedConfig& config, std::vector<AVSortSpec> sort_specs)
{
    config.sort_overrides = std::move(sort_specs);
}

void AVLinkedDatabase::hide_columns(AVLinkedConfig& config, const std::vector<std::string>& key_ids)
{
    for (const auto& kid : key_ids)
    {
        config.hidden_key_ids.insert(kid);
    }
}

void AVLinkedDatabase::show_columns(AVLinkedConfig& config, const std::vector<std::string>& key_ids)
{
    for (const auto& kid : key_ids)
    {
        config.hidden_key_ids.erase(kid);
    }
}

// ════════════════════════════════════════════════════════════
// Resolve
// ════════════════════════════════════════════════════════════

auto AVLinkedDatabase::resolve(const AVLinkedConfig& config) const
    -> std::expected<AVLinkedSnapshot, std::string>
{
    auto source_opt = store_.load(config.source_av_id);
    if (!source_opt)
    {
        return std::unexpected("Source AV not found: " + config.source_av_id);
    }

    const auto& source = *source_opt;

    AVLinkedSnapshot snapshot;
    snapshot.link_id = config.link_id;
    snapshot.source_av_id = config.source_av_id;
    snapshot.source_av_name = source.name;
    snapshot.total_rows = static_cast<int>(source.row_block_ids().size());

    // Filter visible keys
    for (const auto& kv : source.key_values)
    {
        if (!config.hidden_key_ids.contains(kv.key.id))
        {
            snapshot.visible_keys.push_back(kv.key);
        }
    }

    // Apply filter and sort via query engine
    AVColumnTypeRegistry registry;
    AVQueryEngine query_engine(registry);

    auto result = query_engine.execute(source,
                                       config.filter_overrides,
                                       config.sort_overrides,
                                       1,      // page
                                       10000); // large page to get all
    snapshot.block_ids = result.block_ids;

    return snapshot;
}

auto AVLinkedDatabase::refresh(const AVLinkedConfig& config) const
    -> std::expected<AVLinkedSnapshot, std::string>
{
    return resolve(config);
}

// ════════════════════════════════════════════════════════════
// Validation
// ════════════════════════════════════════════════════════════

auto AVLinkedDatabase::is_valid(const AVLinkedConfig& config) const -> bool
{
    auto source_opt = store_.load(config.source_av_id);
    return source_opt.has_value();
}

auto AVLinkedDatabase::link_count() const -> size_t
{
    return links_.size();
}

} // namespace markamp::core::av
