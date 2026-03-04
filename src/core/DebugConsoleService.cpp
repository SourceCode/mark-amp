#include "DebugConsoleService.h"

#include <algorithm>
#include <utility>

namespace markamp::core
{

void DebugConsoleService::add_output(const std::string& text,
                                     ConsoleEntryType type,
                                     const std::string& source)
{
    ConsoleEntry entry;
    entry.type = type;
    entry.text = text;
    entry.source = source;
    entry.level = (type == ConsoleEntryType::kError) ? LogLevel::kError : LogLevel::kInfo;
    entry.timestamp = std::chrono::system_clock::now();
    entries_.push_back(std::move(entry));
    trim_entries();
    fire_change();
}

void DebugConsoleService::add_output(const std::string& text,
                                     LogLevel level,
                                     const std::string& source)
{
    ConsoleEntry entry;
    entry.type = (level >= LogLevel::kError) ? ConsoleEntryType::kError : ConsoleEntryType::kOutput;
    entry.text = text;
    entry.source = source;
    entry.level = level;
    entry.timestamp = std::chrono::system_clock::now();
    entries_.push_back(std::move(entry));
    trim_entries();
    fire_change();
}

void DebugConsoleService::add_object_result(const std::string& expression, ConsoleObjectValue value)
{
    ConsoleEntry entry;
    entry.type = ConsoleEntryType::kResult;
    entry.text = expression;
    entry.object_value = std::move(value);
    entry.has_object = true;
    entry.timestamp = std::chrono::system_clock::now();
    entries_.push_back(std::move(entry));
    trim_entries();
    fire_change();
}

auto DebugConsoleService::entries() const -> const std::deque<ConsoleEntry>&
{
    return entries_;
}

auto DebugConsoleService::entries_filtered(LogLevel min_level) const -> std::vector<ConsoleEntry>
{
    std::vector<ConsoleEntry> filtered;
    filtered.reserve(entries_.size());
    for (const auto& entry : entries_)
    {
        if (entry.level >= min_level)
        {
            filtered.push_back(entry);
        }
    }
    return filtered;
}

void DebugConsoleService::clear()
{
    entries_.clear();
    fire_change();
}

auto DebugConsoleService::entry_count() const -> std::size_t
{
    return entries_.size();
}

void DebugConsoleService::push_history(const std::string& command)
{
    if (command.empty())
    {
        return;
    }
    // Avoid consecutive duplicates.
    if (!history_.empty() && history_.back() == command)
    {
        return;
    }
    history_.push_back(command);
    while (history_.size() > kMaxHistory)
    {
        history_.pop_front();
    }
    history_pos_ = -1;
}

auto DebugConsoleService::navigate_history(int direction) -> std::string
{
    if (history_.empty())
    {
        return {};
    }

    if (history_pos_ == -1)
    {
        // At bottom: only allow going up.
        if (direction < 0)
        {
            history_pos_ = static_cast<int>(history_.size()) - 1;
        }
        else
        {
            return {};
        }
    }
    else
    {
        history_pos_ += direction;
        if (history_pos_ < 0)
        {
            history_pos_ = 0;
        }
        if (history_pos_ >= static_cast<int>(history_.size()))
        {
            history_pos_ = -1;
            return {};
        }
    }

    return history_[static_cast<std::size_t>(history_pos_)];
}

void DebugConsoleService::reset_history_position()
{
    history_pos_ = -1;
}

auto DebugConsoleService::history() const -> const std::deque<std::string>&
{
    return history_;
}

auto DebugConsoleService::history_position() const -> int
{
    return history_pos_;
}

auto DebugConsoleService::on_change(ChangeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

void DebugConsoleService::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const auto& pair)
                                    { return pair.first == listener_id; }),
                     listeners_.end());
}

void DebugConsoleService::trim_entries()
{
    while (entries_.size() > kMaxEntries)
    {
        entries_.pop_front();
    }
}

void DebugConsoleService::fire_change()
{
    for (const auto& [listener_id, listener] : listeners_)
    {
        listener();
    }
}

auto DebugConsoleService::detect_stack_traces(const std::string& text)
    -> std::vector<StackTraceLink>
{
    std::vector<StackTraceLink> links;

    // Pattern 1: GCC/Clang style — /path/to/file.cpp:42:10
    // Matches: /absolute/path:line[:column]
    static const std::regex kGccPattern(R"re((/[^\s:]+\.[a-zA-Z]+):(\d+)(?::(\d+))?)re");

    // Pattern 2: Python style — File "path/to/file.py", line 42
    static const std::regex kPythonPattern(R"re(File "([^"]+)", line (\d+))re");

    // Pattern 3: Node.js style — at Something (/path/to/file.js:42:10)
    static const std::regex kNodePattern(R"re(at\s+\S+\s+\(([^)]+):(\d+):(\d+)\))re");

    auto search_pattern = [&](const std::regex& pattern, bool has_column)
    {
        auto match_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
        auto match_end = std::sregex_iterator();

        for (auto match_it = match_begin; match_it != match_end; ++match_it)
        {
            const auto& match = *match_it;
            StackTraceLink link;
            link.file = match[1].str();
            link.line = std::stoi(match[2].str());
            if (has_column && match.size() > 3 && match[3].matched)
            {
                link.column = std::stoi(match[3].str());
            }
            link.match_start = static_cast<std::size_t>(match.position());
            link.match_length = static_cast<std::size_t>(match.length());
            links.push_back(link);
        }
    };

    search_pattern(kGccPattern, true);
    search_pattern(kPythonPattern, false);
    search_pattern(kNodePattern, true);

    return links;
}

auto DebugConsoleService::toggle_expansion(std::size_t entry_index, const std::string& /*path*/)
    -> bool
{
    if (entry_index >= entries_.size())
    {
        return false;
    }

    auto& entry = entries_[entry_index];
    if (!entry.has_object || !entry.object_value.expandable)
    {
        return false;
    }

    // Toggle the root object expansion state
    // In a full implementation, 'path' would navigate to nested children
    entry.object_value.expanded = !entry.object_value.expanded;
    fire_change();
    return true;
}

void DebugConsoleService::intercept_console_output(const std::string& text, LogLevel level)
{
    ConsoleEntry entry;
    entry.type = ConsoleEntryType::kOutput;
    entry.text = text;
    entry.source = "console.log";
    entry.level = level;
    entry.timestamp = std::chrono::system_clock::now();

    entries_.push_back(std::move(entry));
    trim_entries();
    fire_change();
}

auto DebugConsoleService::auto_complete(const std::string& prefix) const -> std::vector<std::string>
{
    std::vector<std::string> matches;
    if (prefix.empty())
    {
        return matches;
    }

    // Search history in reverse (most recent first), deduplicate
    for (auto iter = history_.rbegin(); iter != history_.rend(); ++iter)
    {
        if (iter->length() >= prefix.length() && iter->substr(0, prefix.length()) == prefix)
        {
            // Avoid duplicates
            if (std::find(matches.begin(), matches.end(), *iter) == matches.end())
            {
                matches.push_back(*iter);
            }
        }
    }
    return matches;
}

auto DebugConsoleService::inspect_variable(const std::string& name) const
    -> std::optional<ConsoleObjectValue>
{
    // Search entries in reverse for the most recent object result with matching name
    for (auto iter = entries_.rbegin(); iter != entries_.rend(); ++iter)
    {
        if (iter->has_object && iter->object_value.name == name)
        {
            return iter->object_value;
        }
    }
    return std::nullopt;
}

auto DebugConsoleService::format_entry_for_display(const ConsoleEntry& entry) -> std::string
{
    std::string result;

    // Prefix with entry type marker
    switch (entry.type)
    {
        case ConsoleEntryType::kInput:
            result += "> ";
            break;
        case ConsoleEntryType::kOutput:
            result += "< ";
            break;
        case ConsoleEntryType::kResult:
            result += "= ";
            break;
        case ConsoleEntryType::kError:
            result += "! ";
            break;
        case ConsoleEntryType::kSystem:
            result += "# ";
            break;
        case ConsoleEntryType::kDebug:
            result += "~ ";
            break;
    }

    // For expandable objects, show expansion marker
    if (entry.has_object && entry.object_value.expandable)
    {
        result += entry.object_value.expanded ? "▾ " : "▸ ";
        result += entry.object_value.name + ": " + entry.object_value.value_text;

        // Show children if expanded
        if (entry.object_value.expanded)
        {
            for (const auto& child : entry.object_value.children)
            {
                result += "\n  " + child.name + ": " + child.value_text;
            }
        }
    }
    else
    {
        result += entry.text;
    }

    return result;
}

} // namespace markamp::core
