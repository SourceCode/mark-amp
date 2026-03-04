/// @file BuildLogManager.cpp
/// @brief Phase 38 Task 29 — Build log persistence implementation.

#include "core/BuildLogManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace markamp::core
{

void BuildLogManager::add_entry(BuildLogEntry entry)
{
    entries_.push_back(std::move(entry));
    if (entries_.size() > kMaxLogs)
        entries_.erase(entries_.begin());
}

auto BuildLogManager::entries() const -> const std::vector<BuildLogEntry>&
{
    return entries_;
}

auto BuildLogManager::latest() const -> const BuildLogEntry*
{
    return entries_.empty() ? nullptr : &entries_.back();
}

auto BuildLogManager::entry_count() const -> std::size_t
{
    return entries_.size();
}

void BuildLogManager::clear()
{
    entries_.clear();
}

auto BuildLogManager::export_text(const BuildLogEntry& entry) -> std::string
{
    std::ostringstream ss;
    ss << "Build Log: " << entry.timestamp << "\n"
       << "Config: " << entry.build_config << "\n"
       << "Target: " << entry.target << "\n"
       << "Result: " << (entry.success ? "SUCCESS" : "FAILED") << "\n"
       << "Errors: " << entry.error_count << " Warnings: " << entry.warning_count << "\n"
       << "Duration: " << entry.duration_seconds << "s\n"
       << "---\n"
       << entry.output;
    return ss.str();
}

auto BuildLogManager::export_html(const BuildLogEntry& entry) -> std::string
{
    std::ostringstream ss;
    ss << "<html><body><pre>" << entry.output << "</pre></body></html>";
    return ss.str();
}

auto BuildLogManager::compare(const BuildLogEntry& a, const BuildLogEntry& b) -> std::string
{
    std::ostringstream ss;
    ss << "Build A: " << a.timestamp << " vs Build B: " << b.timestamp << "\n";
    ss << "Errors: " << a.error_count << " vs " << b.error_count;
    if (a.error_count != b.error_count)
        ss << " (" << (b.error_count - a.error_count > 0 ? "+" : "")
           << (b.error_count - a.error_count) << ")";
    ss << "\nWarnings: " << a.warning_count << " vs " << b.warning_count << "\n";
    ss << "Duration: " << a.duration_seconds << "s vs " << b.duration_seconds << "s\n";
    return ss.str();
}

void BuildLogManager::save_logs(const std::string& /*dir_path*/) const
{ /* File I/O stub */
}
void BuildLogManager::load_logs(const std::string& /*dir_path*/)
{ /* File I/O stub */
}

} // namespace markamp::core
