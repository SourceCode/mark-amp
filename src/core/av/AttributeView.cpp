#include "AttributeView.h"

#include <algorithm>
#include <random>
#include <set>
#include <sstream>

namespace markamp::core::av
{

namespace
{

auto generate_uuid() -> std::string
{
    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 15);

    const char hex_chars[] = "0123456789abcdef";
    const std::string pattern = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

    std::string result;
    result.reserve(pattern.size());

    for (const char chr : pattern)
    {
        if (chr == '-' || chr == '4')
        {
            result += chr;
        }
        else if (chr == 'y')
        {
            result += hex_chars[(dist(gen) & 0x3) | 0x8];
        }
        else
        {
            result += hex_chars[dist(gen)];
        }
    }

    return result;
}

} // anonymous namespace

// ════════════════════════════════════════════════════════════
// Key operations
// ════════════════════════════════════════════════════════════

auto AttributeView::add_key(AVKey key) -> std::string
{
    if (key.id.empty())
    {
        key.id = generate_uuid();
    }

    const auto assigned_id = key.id;

    // Determine existing row block_ids from the first column (Block type)
    std::vector<std::string> existing_rows;
    if (!key_values.empty())
    {
        for (const auto& val : key_values.front().values)
        {
            existing_rows.push_back(val.block_id);
        }
    }

    AVKeyValues kv_entry;
    kv_entry.key = std::move(key);

    // Add empty values for existing rows
    for (const auto& bid : existing_rows)
    {
        AVValue empty_val;
        empty_val.id = generate_uuid();
        empty_val.key_id = kv_entry.key.id;
        empty_val.block_id = bid;
        empty_val.type = kv_entry.key.type;
        kv_entry.values.push_back(std::move(empty_val));
    }

    key_values.push_back(std::move(kv_entry));
    return assigned_id;
}

auto AttributeView::remove_key(const std::string& key_id) -> bool
{
    const auto iter =
        std::find_if(key_values.begin(),
                     key_values.end(),
                     [&key_id](const AVKeyValues& kv) { return kv.key.id == key_id; });

    if (iter == key_values.end())
    {
        return false;
    }

    key_values.erase(iter);

    // Remove column from all views
    for (auto& view : views)
    {
        auto& cols = view.table.columns;
        cols.erase(std::remove_if(cols.begin(),
                                  cols.end(),
                                  [&key_id](const AVViewColumn& col)
                                  { return col.key_id == key_id; }),
                   cols.end());

        auto& gcols = view.gallery.columns;
        gcols.erase(std::remove_if(gcols.begin(),
                                   gcols.end(),
                                   [&key_id](const AVViewColumn& col)
                                   { return col.key_id == key_id; }),
                    gcols.end());

        auto& kcols = view.kanban.columns;
        kcols.erase(std::remove_if(kcols.begin(),
                                   kcols.end(),
                                   [&key_id](const AVViewColumn& col)
                                   { return col.key_id == key_id; }),
                    kcols.end());
    }

    return true;
}

auto AttributeView::find_key(const std::string& key_id) -> AVKey*
{
    for (auto& kv_entry : key_values)
    {
        if (kv_entry.key.id == key_id)
        {
            return &kv_entry.key;
        }
    }
    return nullptr;
}

auto AttributeView::find_key(const std::string& key_id) const -> const AVKey*
{
    for (const auto& kv_entry : key_values)
    {
        if (kv_entry.key.id == key_id)
        {
            return &kv_entry.key;
        }
    }
    return nullptr;
}

auto AttributeView::key_ids() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(key_values.size());
    for (const auto& kv_entry : key_values)
    {
        result.push_back(kv_entry.key.id);
    }
    return result;
}

auto AttributeView::key_count() const -> size_t
{
    return key_values.size();
}

// ════════════════════════════════════════════════════════════
// Row operations
// ════════════════════════════════════════════════════════════

auto AttributeView::add_row(const std::string& block_id) -> std::string
{
    for (auto& kv_entry : key_values)
    {
        AVValue new_val;
        new_val.id = generate_uuid();
        new_val.key_id = kv_entry.key.id;
        new_val.block_id = block_id;
        new_val.type = kv_entry.key.type;
        kv_entry.values.push_back(std::move(new_val));
    }
    return block_id;
}

auto AttributeView::remove_row(const std::string& block_id) -> bool
{
    bool removed = false;
    for (auto& kv_entry : key_values)
    {
        const auto iter =
            std::find_if(kv_entry.values.begin(),
                         kv_entry.values.end(),
                         [&block_id](const AVValue& val) { return val.block_id == block_id; });
        if (iter != kv_entry.values.end())
        {
            kv_entry.values.erase(iter);
            removed = true;
        }
    }
    return removed;
}

auto AttributeView::row_block_ids() const -> std::vector<std::string>
{
    if (key_values.empty())
    {
        return {};
    }

    std::vector<std::string> result;
    std::set<std::string> seen;
    for (const auto& val : key_values.front().values)
    {
        if (seen.insert(val.block_id).second)
        {
            result.push_back(val.block_id);
        }
    }
    return result;
}

auto AttributeView::row_count() const -> size_t
{
    if (key_values.empty())
    {
        return 0;
    }
    return key_values.front().values.size();
}

// ════════════════════════════════════════════════════════════
// Value operations
// ════════════════════════════════════════════════════════════

auto AttributeView::get_value(const std::string& key_id, const std::string& block_id) -> AVValue*
{
    for (auto& kv_entry : key_values)
    {
        if (kv_entry.key.id == key_id)
        {
            for (auto& val : kv_entry.values)
            {
                if (val.block_id == block_id)
                {
                    return &val;
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

auto AttributeView::get_value(const std::string& key_id, const std::string& block_id) const
    -> const AVValue*
{
    for (const auto& kv_entry : key_values)
    {
        if (kv_entry.key.id == key_id)
        {
            for (const auto& val : kv_entry.values)
            {
                if (val.block_id == block_id)
                {
                    return &val;
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

void AttributeView::set_value(const std::string& key_id,
                              const std::string& block_id,
                              AVValueVariant data)
{
    for (auto& kv_entry : key_values)
    {
        if (kv_entry.key.id == key_id)
        {
            for (auto& val : kv_entry.values)
            {
                if (val.block_id == block_id)
                {
                    val.data = std::move(data);
                    return;
                }
            }
            // Value not found — create it
            AVValue new_val;
            new_val.id = generate_uuid();
            new_val.key_id = key_id;
            new_val.block_id = block_id;
            new_val.type = kv_entry.key.type;
            new_val.data = std::move(data);
            kv_entry.values.push_back(std::move(new_val));
            return;
        }
    }
}

// ════════════════════════════════════════════════════════════
// View operations
// ════════════════════════════════════════════════════════════

auto AttributeView::add_view(AVView view) -> std::string
{
    if (view.id.empty())
    {
        view.id = generate_uuid();
    }

    const auto assigned_id = view.id;

    // Auto-populate columns if empty
    if (view.type == AVViewType::Table && view.table.columns.empty())
    {
        for (const auto& kv_entry : key_values)
        {
            view.table.columns.push_back(AVViewColumn{kv_entry.key.id});
        }
    }
    else if (view.type == AVViewType::Gallery && view.gallery.columns.empty())
    {
        for (const auto& kv_entry : key_values)
        {
            view.gallery.columns.push_back(AVViewColumn{kv_entry.key.id});
        }
    }
    else if (view.type == AVViewType::Kanban && view.kanban.columns.empty())
    {
        for (const auto& kv_entry : key_values)
        {
            view.kanban.columns.push_back(AVViewColumn{kv_entry.key.id});
        }
    }

    views.push_back(std::move(view));
    return assigned_id;
}

auto AttributeView::remove_view(const std::string& view_id) -> bool
{
    const auto iter = std::find_if(
        views.begin(), views.end(), [&view_id](const AVView& view) { return view.id == view_id; });
    if (iter == views.end())
    {
        return false;
    }
    views.erase(iter);
    return true;
}

auto AttributeView::find_view(const std::string& view_id) -> AVView*
{
    for (auto& view : views)
    {
        if (view.id == view_id)
        {
            return &view;
        }
    }
    return nullptr;
}

auto AttributeView::find_view(const std::string& view_id) const -> const AVView*
{
    for (const auto& view : views)
    {
        if (view.id == view_id)
        {
            return &view;
        }
    }
    return nullptr;
}

auto AttributeView::default_view() -> AVView&
{
    if (views.empty())
    {
        AVView default_table;
        default_table.id = generate_uuid();
        default_table.name = "Table";
        default_table.type = AVViewType::Table;

        for (const auto& kv_entry : key_values)
        {
            default_table.table.columns.push_back(AVViewColumn{kv_entry.key.id});
        }

        views.push_back(std::move(default_table));
    }
    return views.front();
}

} // namespace markamp::core::av
