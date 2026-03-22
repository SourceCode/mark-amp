/// @file CompletionInventory.cpp
/// @brief V23 Phase 01 — CompletionInventory implementation.

#include "CompletionInventory.h"

#include <algorithm>
#include <set>
#include <sstream>

namespace markamp::core
{

// ── Registration ──

void CompletionInventory::add_item(InventoryItem item)
{
    items_.push_back(std::move(item));
}

void CompletionInventory::add_items(std::vector<InventoryItem> items)
{
    for (auto& item : items) items_.push_back(std::move(item));
}

void CompletionInventory::clear()
{
    items_.clear();
    feature_chains_.clear();
}

// ── Classification ──

auto CompletionInventory::classify_family(MarkerType type,
                                          std::string_view subsystem,
                                          bool is_production) -> CompletionFamily
{
    // Test markers always classify as TestGap
    if (!is_production && (type == MarkerType::kMock || type == MarkerType::kFake))
        return CompletionFamily::kTestGap;

    // Transport-related stubs
    if (type == MarkerType::kStub || type == MarkerType::kRealImplementationWould)
    {
        if (subsystem == "CloudSync" || subsystem == "AI" ||
            subsystem == "HTTP" || subsystem == "Git")
            return CompletionFamily::kTransportStub;

        if (subsystem == "Search" || subsystem == "Persistence" ||
            subsystem == "Recovery" || subsystem == "Autosave")
            return CompletionFamily::kPersistenceStub;

        return CompletionFamily::kServiceStub;
    }

    if (type == MarkerType::kPlaceholder)
    {
        if (subsystem == "Panel" || subsystem == "Sidebar" || subsystem == "UI")
            return CompletionFamily::kPanelShell;

        if (subsystem == "Rendering" || subsystem == "Preview")
            return CompletionFamily::kPlaceholderRendering;

        return CompletionFamily::kUIPlaceholder;
    }

    if (type == MarkerType::kDeferred || type == MarkerType::kPending)
        return CompletionFamily::kWorkflowPartial;

    if (type == MarkerType::kForNow || type == MarkerType::kHack)
        return CompletionFamily::kMigrationSeam;

    if (type == MarkerType::kMock || type == MarkerType::kFake)
    {
        if (is_production)
            return CompletionFamily::kServiceStub;
        return CompletionFamily::kTestGap;
    }

    return CompletionFamily::kUnclassified;
}

auto CompletionInventory::subsystem_from_path(std::string_view file_path)
    -> std::string
{
    // Normalize forward slashes
    const auto path = std::string(file_path);

    // Canvas subsystem
    if (path.find("canvas/") != std::string::npos ||
        path.find("Canvas") != std::string::npos)
        return "Canvas";

    // Notebook/Kernel subsystem
    if (path.find("Notebook") != std::string::npos ||
        path.find("Kernel") != std::string::npos ||
        path.find("Cell") != std::string::npos)
        return "Notebook";

    // AI subsystem
    if (path.find("AI") != std::string::npos)
        return "AI";

    // Git/SCM subsystem
    if (path.find("Git") != std::string::npos ||
        path.find("SourceControl") != std::string::npos ||
        path.find("Commit") != std::string::npos)
        return "Git";

    // Cloud/Sync subsystem
    if (path.find("Sync") != std::string::npos ||
        path.find("Cloud") != std::string::npos ||
        path.find("S3") != std::string::npos ||
        path.find("WebDav") != std::string::npos)
        return "CloudSync";

    // Settings subsystem (must be before Panel — SettingsPanel contains both)
    if (path.find("Settings") != std::string::npos ||
        path.find("Config") != std::string::npos)
        return "Settings";

    // Search subsystem
    if (path.find("Search") != std::string::npos ||
        path.find("Find") != std::string::npos)
        return "Search";

    // PDF subsystem
    if (path.find("PDF") != std::string::npos ||
        path.find("Pdf") != std::string::npos)
        return "PDF";

    // Panel/sidebar subsystem
    if (path.find("Panel") != std::string::npos ||
        path.find("Sidebar") != std::string::npos)
        return "Panel";

    // Rendering subsystem
    if (path.find("rendering/") != std::string::npos ||
        path.find("Render") != std::string::npos ||
        path.find("Preview") != std::string::npos)
        return "Rendering";

    // Persistence subsystem
    if (path.find("Persist") != std::string::npos ||
        path.find("Save") != std::string::npos ||
        path.find("Autosave") != std::string::npos ||
        path.find("Recovery") != std::string::npos ||
        path.find("Session") != std::string::npos)
        return "Persistence";

    // Extension/Plugin subsystem
    if (path.find("Extension") != std::string::npos ||
        path.find("Plugin") != std::string::npos ||
        path.find("Vsix") != std::string::npos)
        return "Extension";

    // Theme subsystem
    if (path.find("Theme") != std::string::npos)
        return "Theme";

    // Test files
    if (path.find("test") != std::string::npos ||
        path.find("Test") != std::string::npos)
        return "Test";

    // UI subsystem (catch-all for ui/ paths)
    if (path.find("ui/") != std::string::npos)
        return "UI";

    // Core subsystem (catch-all for core/ paths)
    if (path.find("core/") != std::string::npos)
        return "Core";

    return "Other";
}

auto CompletionInventory::severity_for(MarkerType type,
                                       bool is_production) -> CompletionSeverity
{
    if (!is_production)
        return CompletionSeverity::kLow;

    switch (type)
    {
    case MarkerType::kStub:
    case MarkerType::kFake:
    case MarkerType::kRealImplementationWould:
        return CompletionSeverity::kCritical;

    case MarkerType::kPlaceholder:
    case MarkerType::kMock:
        return CompletionSeverity::kHigh;

    case MarkerType::kDeferred:
    case MarkerType::kNotYet:
    case MarkerType::kPending:
        return CompletionSeverity::kMedium;

    case MarkerType::kTodo:
    case MarkerType::kFixme:
    case MarkerType::kForNow:
    case MarkerType::kHack:
        return CompletionSeverity::kLow;
    }

    return CompletionSeverity::kMedium;
}

// ── Queries ──

auto CompletionInventory::item_count() const noexcept -> std::size_t
{
    return items_.size();
}

auto CompletionInventory::items_by_subsystem(std::string_view subsystem) const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (item.subsystem_tag == subsystem)
            result.push_back(&item);
    return result;
}

auto CompletionInventory::items_by_severity(CompletionSeverity severity) const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (item.severity == severity)
            result.push_back(&item);
    return result;
}

auto CompletionInventory::items_by_family(CompletionFamily family) const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (item.family == family)
            result.push_back(&item);
    return result;
}

auto CompletionInventory::production_path_items() const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (item.is_production_path)
            result.push_back(&item);
    return result;
}

auto CompletionInventory::test_only_items() const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (!item.is_production_path)
            result.push_back(&item);
    return result;
}

auto CompletionInventory::blocker_items() const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    for (const auto& item : items_)
        if (item.is_blocker())
            result.push_back(&item);
    return result;
}

auto CompletionInventory::all_items() const
    -> std::vector<const InventoryItem*>
{
    std::vector<const InventoryItem*> result;
    result.reserve(items_.size());
    for (const auto& item : items_)
        result.push_back(&item);
    return result;
}

auto CompletionInventory::all_subsystems() const -> std::vector<std::string>
{
    std::set<std::string> subs;
    for (const auto& item : items_)
        if (!item.subsystem_tag.empty())
            subs.insert(item.subsystem_tag);
    return {subs.begin(), subs.end()};
}

// ── Feature-Chain Linkage ──

void CompletionInventory::add_feature_chain(BlockedFeatureChain chain)
{
    feature_chains_.push_back(std::move(chain));
}

auto CompletionInventory::feature_chain_count() const noexcept -> std::size_t
{
    return feature_chains_.size();
}

auto CompletionInventory::feature_chain_for(std::string_view feature_name) const
    -> const BlockedFeatureChain*
{
    for (const auto& chain : feature_chains_)
        if (chain.feature_name == feature_name)
            return &chain;
    return nullptr;
}

auto CompletionInventory::all_feature_chains() const
    -> std::vector<const BlockedFeatureChain*>
{
    std::vector<const BlockedFeatureChain*> result;
    for (const auto& chain : feature_chains_)
        result.push_back(&chain);
    return result;
}

// ── Summary ──

auto CompletionInventory::summary() const -> InventorySummary
{
    InventorySummary s;
    s.total_items = items_.size();
    for (const auto& item : items_)
    {
        switch (item.severity)
        {
        case CompletionSeverity::kCritical: ++s.critical; break;
        case CompletionSeverity::kHigh:     ++s.high; break;
        case CompletionSeverity::kMedium:   ++s.medium; break;
        case CompletionSeverity::kLow:      ++s.low; break;
        }
        if (item.is_blocker()) ++s.blockers;
        if (item.is_production_path) ++s.production_path;
        else ++s.test_only;
    }
    return s;
}

// ── Export ──

auto CompletionInventory::export_json() const -> std::string
{
    std::ostringstream ss;
    ss << "{\n  \"items\": [\n";
    for (std::size_t i = 0; i < items_.size(); ++i)
    {
        const auto& item = items_[i];
        ss << "    {\n"
           << "      \"file\": \"" << item.file_path << "\",\n"
           << "      \"line\": " << item.line_number << ",\n"
           << "      \"marker_type\": \"" << marker_type_label(item.marker_type) << "\",\n"
           << "      \"subsystem\": \"" << item.subsystem_tag << "\",\n"
           << "      \"severity\": \"" << completion_severity_label(item.severity) << "\",\n"
           << "      \"family\": \"" << completion_family_label(item.family) << "\",\n"
           << "      \"production_path\": " << (item.is_production_path ? "true" : "false") << ",\n"
           << "      \"blocker\": " << (item.is_blocker() ? "true" : "false") << ",\n"
           << "      \"text\": \"" << item.raw_text << "\"\n"
           << "    }";
        if (i + 1 < items_.size()) ss << ",";
        ss << "\n";
    }

    auto s = summary();
    ss << "  ],\n"
       << "  \"summary\": {\n"
       << "    \"total\": " << s.total_items << ",\n"
       << "    \"critical\": " << s.critical << ",\n"
       << "    \"high\": " << s.high << ",\n"
       << "    \"medium\": " << s.medium << ",\n"
       << "    \"low\": " << s.low << ",\n"
       << "    \"blockers\": " << s.blockers << ",\n"
       << "    \"production_path\": " << s.production_path << ",\n"
       << "    \"test_only\": " << s.test_only << "\n"
       << "  }\n"
       << "}\n";
    return ss.str();
}

auto CompletionInventory::export_markdown() const -> std::string
{
    std::ostringstream ss;
    auto s = summary();
    ss << "# V23 Completion Inventory\n\n"
       << "**Total:** " << s.total_items
       << " | **Critical:** " << s.critical
       << " | **High:** " << s.high
       << " | **Medium:** " << s.medium
       << " | **Low:** " << s.low
       << " | **Blockers:** " << s.blockers
       << " | **Blocker Rate:** " << s.blocker_rate_pct() << "%\n\n";

    ss << "| File | Line | Type | Subsystem | Severity | Family | Prod | Blocker |\n"
       << "|---|---|---|---|---|---|---|---|\n";
    for (const auto& item : items_)
    {
        ss << "| " << item.file_path
           << " | " << item.line_number
           << " | " << marker_type_label(item.marker_type)
           << " | " << item.subsystem_tag
           << " | " << completion_severity_label(item.severity)
           << " | " << completion_family_label(item.family)
           << " | " << (item.is_production_path ? "✅" : "❌")
           << " | " << (item.is_blocker() ? "🚫" : "—")
           << " |\n";
    }

    if (!feature_chains_.empty())
    {
        ss << "\n## Blocked Feature Chains\n\n";
        for (const auto& chain : feature_chains_)
        {
            ss << "### " << chain.feature_name << "\n"
               << "- **Blockers:** " << chain.blocker_count() << "\n";
            if (!chain.command_chain.empty())
                ss << "- **Command:** " << chain.command_chain << "\n";
            if (!chain.persistence_chain.empty())
                ss << "- **Persistence:** " << chain.persistence_chain << "\n";
            if (!chain.transport_chain.empty())
                ss << "- **Transport:** " << chain.transport_chain << "\n";
            if (!chain.test_chain.empty())
                ss << "- **Test:** " << chain.test_chain << "\n";
            ss << "\n";
        }
    }

    return ss.str();
}

} // namespace markamp::core
