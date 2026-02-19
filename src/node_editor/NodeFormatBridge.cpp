#include "NodeFormatBridge.h"

#include <set>
#include <sstream>

namespace markamp::node_editor
{

NodeFormatBridge::NodeFormatBridge()
{
    register_builtin_adapters();
}

void NodeFormatBridge::register_builtin_adapters()
{
    // JSON adapter (native format)
    {
        FormatAdapter adapter;
        adapter.format_name = "MarkAmp JSON";
        adapter.file_extensions = {".json", ".magraph"};
        adapter.supports_import = true;
        adapter.supports_export = true;
        adapter.version = "1.0";
        register_adapter(std::move(adapter));
    }

    // SVG export-only adapter
    {
        FormatAdapter adapter;
        adapter.format_name = "SVG";
        adapter.file_extensions = {".svg"};
        adapter.supports_import = false;
        adapter.supports_export = true;
        adapter.version = "1.0";
        register_adapter(std::move(adapter));
    }
}

auto NodeFormatBridge::register_adapter(FormatAdapter adapter) -> AdapterId
{
    AdapterId aid{next_id_++};
    adapter.adapter_id = aid;
    adapters_.emplace(aid.value, std::move(adapter));
    return aid;
}

auto NodeFormatBridge::unregister_adapter(AdapterId adapter_id) -> bool
{
    return adapters_.erase(adapter_id.value) > 0;
}

auto NodeFormatBridge::find_adapter(AdapterId adapter_id) const -> const FormatAdapter*
{
    auto iter = adapters_.find(adapter_id.value);
    if (iter == adapters_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeFormatBridge::adapters_for_extension(const std::string& ext) const
    -> std::vector<AdapterId>
{
    std::vector<AdapterId> result;
    for (const auto& [key, adapter] : adapters_)
    {
        for (const auto& supported_ext : adapter.file_extensions)
        {
            if (supported_ext == ext)
            {
                result.push_back(AdapterId{key});
                break;
            }
        }
    }
    return result;
}

auto NodeFormatBridge::adapter_count() const -> std::size_t
{
    return adapters_.size();
}

auto NodeFormatBridge::export_graph(const NodeGraph& graph, AdapterId adapter_id) const
    -> FormatBridgeResult
{
    FormatBridgeResult result;
    const auto* adapter = find_adapter(adapter_id);

    if (adapter == nullptr)
    {
        result.errors.emplace_back("Unknown adapter ID");
        return result;
    }
    if (!adapter->supports_export)
    {
        result.errors.emplace_back("Adapter does not support export");
        return result;
    }

    // Simplified export: produce a representation with node/link count
    std::ostringstream oss;
    oss << "{\"format\":\"" << adapter->format_name << "\""
        << ",\"node_count\":" << graph.node_count() << ",\"link_count\":" << graph.link_count()
        << "}";

    result.success = true;
    result.data = oss.str();
    return result;
}

auto NodeFormatBridge::import_graph(const std::string& data, AdapterId adapter_id) const
    -> FormatBridgeResult
{
    FormatBridgeResult result;
    const auto* adapter = find_adapter(adapter_id);

    if (adapter == nullptr)
    {
        result.errors.emplace_back("Unknown adapter ID");
        return result;
    }
    if (!adapter->supports_import)
    {
        result.errors.emplace_back("Adapter does not support import");
        return result;
    }
    if (data.empty())
    {
        result.errors.emplace_back("Import data is empty");
        return result;
    }

    result.success = true;
    result.data = data;
    return result;
}

auto NodeFormatBridge::supported_import_extensions() const -> std::vector<std::string>
{
    std::set<std::string> exts;
    for (const auto& [_, adapter] : adapters_)
    {
        if (adapter.supports_import)
        {
            for (const auto& ext : adapter.file_extensions)
            {
                exts.insert(ext);
            }
        }
    }
    return {exts.begin(), exts.end()};
}

auto NodeFormatBridge::supported_export_extensions() const -> std::vector<std::string>
{
    std::set<std::string> exts;
    for (const auto& [_, adapter] : adapters_)
    {
        if (adapter.supports_export)
        {
            for (const auto& ext : adapter.file_extensions)
            {
                exts.insert(ext);
            }
        }
    }
    return {exts.begin(), exts.end()};
}

void NodeFormatBridge::clear()
{
    adapters_.clear();
}

} // namespace markamp::node_editor
