#include "AttributeViewStore.h"

#include <fstream>
#include <random>
#include <sstream>

namespace markamp::core::av
{

AttributeViewStore::AttributeViewStore(std::filesystem::path storage_root)
    : storage_root_(std::move(storage_root))
    , av_directory_(storage_root_ / "av")
{
}

auto AttributeViewStore::generate_uuid() -> std::string
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

auto AttributeViewStore::ensure_directory() -> std::expected<void, std::string>
{
    std::error_code error_code;
    std::filesystem::create_directories(av_directory_, error_code);
    if (error_code)
    {
        return std::unexpected("Failed to create AV directory: " + error_code.message());
    }
    return {};
}

auto AttributeViewStore::file_path_for(const std::string& av_id) const -> std::filesystem::path
{
    return av_directory_ / (av_id + ".json");
}

auto AttributeViewStore::create(const std::string& name)
    -> std::expected<AttributeView, std::string>
{
    if (const auto dir_result = ensure_directory(); !dir_result)
    {
        return std::unexpected(dir_result.error());
    }

    AttributeView attribute_view;
    attribute_view.id = generate_uuid();
    attribute_view.name = name;
    attribute_view.spec = kAVSpecVersion;

    // Add default Block key
    AVKey block_key;
    block_key.id = generate_uuid();
    block_key.name = "Block";
    block_key.type = AVKeyType::Block;
    (void)attribute_view.add_key(std::move(block_key));

    // Add default table view
    AVView default_view;
    default_view.id = generate_uuid();
    default_view.name = "Table";
    default_view.type = AVViewType::Table;
    (void)attribute_view.add_view(std::move(default_view));

    if (const auto save_result = save(attribute_view); !save_result)
    {
        return std::unexpected(save_result.error());
    }

    return attribute_view;
}

auto AttributeViewStore::load(const std::string& av_id) -> std::expected<AttributeView, std::string>
{
    const std::lock_guard<std::mutex> lock(mutex_);

    // Check cache
    if (const auto cache_iter = cache_.find(av_id); cache_iter != cache_.end())
    {
        return cache_iter->second;
    }

    const auto path = file_path_for(av_id);

    std::error_code error_code;
    if (!std::filesystem::exists(path, error_code))
    {
        return std::unexpected("Attribute view not found: " + av_id);
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to open AV file: " + path.string());
    }

    std::ostringstream content_stream;
    content_stream << file.rdbuf();

    auto result = deserialize(content_stream.str());
    if (!result)
    {
        return result;
    }

    cache_[av_id] = result.value();
    return result;
}

auto AttributeViewStore::save(const AttributeView& attribute_view)
    -> std::expected<void, std::string>
{
    if (const auto dir_result = ensure_directory(); !dir_result)
    {
        return std::unexpected(dir_result.error());
    }

    const auto json_str = serialize(attribute_view);
    const auto path = file_path_for(attribute_view.id);

    std::ofstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to write AV file: " + path.string());
    }

    file << json_str;
    file.close();

    if (file.fail())
    {
        return std::unexpected("Error writing AV file: " + path.string());
    }

    const std::lock_guard<std::mutex> lock(mutex_);
    cache_[attribute_view.id] = attribute_view;
    return {};
}

auto AttributeViewStore::delete_av(const std::string& av_id) -> std::expected<void, std::string>
{
    const auto path = file_path_for(av_id);

    std::error_code error_code;
    if (std::filesystem::exists(path, error_code))
    {
        std::filesystem::remove(path, error_code);
        if (error_code)
        {
            return std::unexpected("Failed to delete AV file: " + error_code.message());
        }
    }

    const std::lock_guard<std::mutex> lock(mutex_);
    cache_.erase(av_id);
    return {};
}

auto AttributeViewStore::get_all_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;

    std::error_code error_code;
    if (!std::filesystem::exists(av_directory_, error_code))
    {
        return ids;
    }

    for (const auto& entry : std::filesystem::directory_iterator(av_directory_, error_code))
    {
        if (entry.path().extension() == ".json")
        {
            ids.push_back(entry.path().stem().string());
        }
    }
    return ids;
}

auto AttributeViewStore::exists(const std::string& av_id) const -> bool
{
    std::error_code error_code;
    return std::filesystem::exists(file_path_for(av_id), error_code);
}

void AttributeViewStore::evict(const std::string& av_id)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    cache_.erase(av_id);
}

void AttributeViewStore::clear_cache()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

// ════════════════════════════════════════════════════════════
// JSON Serialization (manual, compact)
// ════════════════════════════════════════════════════════════

namespace
{

auto escape_json(const std::string& str) -> std::string
{
    std::string result;
    result.reserve(str.size() + 8);
    for (const char chr : str)
    {
        switch (chr)
        {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += chr;
                break;
        }
    }
    return result;
}

} // anonymous namespace

auto AttributeViewStore::serialize(const AttributeView& attribute_view) const -> std::string
{
    std::ostringstream json;
    json << "{";
    json << "\"id\":\"" << escape_json(attribute_view.id) << "\",";
    json << "\"name\":\"" << escape_json(attribute_view.name) << "\",";
    json << "\"spec\":" << attribute_view.spec << ",";

    // key_values
    json << "\"keyValues\":[";
    for (size_t ki = 0; ki < attribute_view.key_values.size(); ++ki)
    {
        if (ki > 0)
        {
            json << ",";
        }
        const auto& kv_entry = attribute_view.key_values[ki];
        json << "{\"key\":{";
        json << "\"id\":\"" << escape_json(kv_entry.key.id) << "\",";
        json << "\"name\":\"" << escape_json(kv_entry.key.name) << "\",";
        json << "\"type\":\"" << key_type_to_string(kv_entry.key.type) << "\"";
        if (!kv_entry.key.icon.empty())
        {
            json << ",\"icon\":\"" << escape_json(kv_entry.key.icon) << "\"";
        }
        if (!kv_entry.key.desc.empty())
        {
            json << ",\"desc\":\"" << escape_json(kv_entry.key.desc) << "\"";
        }
        if (kv_entry.key.number_format != AVNumberFormat::None)
        {
            json << ",\"numberFormat\":\"" << number_format_to_string(kv_entry.key.number_format)
                 << "\"";
        }
        if (!kv_entry.key.options.empty())
        {
            json << ",\"options\":[";
            for (size_t oi = 0; oi < kv_entry.key.options.size(); ++oi)
            {
                if (oi > 0)
                {
                    json << ",";
                }
                json << "{\"name\":\"" << escape_json(kv_entry.key.options[oi].name)
                     << "\",\"color\":\"" << escape_json(kv_entry.key.options[oi].color) << "\"}";
            }
            json << "]";
        }
        json << "},\"values\":[";
        for (size_t vi = 0; vi < kv_entry.values.size(); ++vi)
        {
            if (vi > 0)
            {
                json << ",";
            }
            const auto& val = kv_entry.values[vi];
            json << "{\"id\":\"" << escape_json(val.id) << "\",";
            json << "\"keyId\":\"" << escape_json(val.key_id) << "\",";
            json << "\"blockId\":\"" << escape_json(val.block_id) << "\",";
            json << "\"type\":\"" << key_type_to_string(val.type) << "\",";
            json << "\"display\":\"" << escape_json(val.to_display_string()) << "\"}";
        }
        json << "]}";
    }
    json << "],";

    // views
    json << "\"views\":[";
    for (size_t vi = 0; vi < attribute_view.views.size(); ++vi)
    {
        if (vi > 0)
        {
            json << ",";
        }
        const auto& view = attribute_view.views[vi];
        json << "{\"id\":\"" << escape_json(view.id) << "\",";
        json << "\"name\":\"" << escape_json(view.name) << "\",";
        json << "\"type\":\"" << view_type_to_string(view.type) << "\",";
        json << "\"pageSize\":" << view.page_size << "}";
    }
    json << "]";

    json << "}";
    return json.str();
}

auto AttributeViewStore::deserialize(const std::string& json_str)
    -> std::expected<AttributeView, std::string>
{
    // Minimal validation
    if (json_str.empty() || json_str.front() != '{')
    {
        return std::unexpected("Invalid JSON: expected object");
    }

    AttributeView attribute_view;

    // Extract simple string fields using basic parsing
    auto extract_string = [&json_str](const std::string& field_name) -> std::string
    {
        const auto key = "\"" + field_name + "\":\"";
        const auto pos = json_str.find(key);
        if (pos == std::string::npos)
        {
            return {};
        }
        const auto start = pos + key.size();
        auto end = start;
        while (end < json_str.size() && json_str[end] != '"')
        {
            if (json_str[end] == '\\')
            {
                ++end;
            } // skip escaped char
            ++end;
        }
        return json_str.substr(start, end - start);
    };

    attribute_view.id = extract_string("id");
    attribute_view.name = extract_string("name");

    if (attribute_view.id.empty())
    {
        return std::unexpected("Invalid AV JSON: missing 'id' field");
    }

    // Extract spec version
    const auto spec_key = std::string("\"spec\":");
    const auto spec_pos = json_str.find(spec_key);
    if (spec_pos != std::string::npos)
    {
        attribute_view.spec = std::stoi(json_str.substr(spec_pos + spec_key.size()));
    }

    return attribute_view;
}

} // namespace markamp::core::av
