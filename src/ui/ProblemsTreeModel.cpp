#include "ProblemsTreeModel.h"

#include <algorithm>

namespace markamp::ui
{

void ProblemsTreeModel::rebuild(const core::DiagnosticsService& service)
{
    nodes_.clear();

    auto uris = service.uris();
    for (const auto& uri : uris)
    {
        const auto& diagnostics = service.get(uri);
        if (diagnostics.empty())
        {
            continue;
        }

        ProblemFileNode node;
        node.file_uri = uri;
        node.display_name = extract_basename(uri);
        node.diagnostics = diagnostics;
        nodes_.push_back(std::move(node));
    }

    apply_filters();
    apply_sort();
}

auto ProblemsTreeModel::file_nodes() const -> const std::vector<ProblemFileNode>&
{
    return filtered_nodes_;
}

auto ProblemsTreeModel::total_error_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& node : filtered_nodes_)
    {
        count += node.error_count();
    }
    return count;
}

auto ProblemsTreeModel::total_warning_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& node : filtered_nodes_)
    {
        count += node.warning_count();
    }
    return count;
}

auto ProblemsTreeModel::total_info_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& node : filtered_nodes_)
    {
        count += node.info_count();
    }
    return count;
}

auto ProblemsTreeModel::total_diagnostic_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& node : filtered_nodes_)
    {
        count += node.diagnostics.size();
    }
    return count;
}

void ProblemsTreeModel::set_severity_filter(core::DiagnosticSeverity min_severity)
{
    severity_filter_ = min_severity;
    apply_filters();
    apply_sort();
}

auto ProblemsTreeModel::severity_filter() const -> core::DiagnosticSeverity
{
    return severity_filter_;
}

void ProblemsTreeModel::set_source_filter(const std::string& source)
{
    source_filter_ = source;
    apply_filters();
    apply_sort();
}

auto ProblemsTreeModel::source_filter() const -> const std::string&
{
    return source_filter_;
}

void ProblemsTreeModel::set_text_filter(const std::string& text)
{
    text_filter_ = text;
    apply_filters();
    apply_sort();
}

auto ProblemsTreeModel::text_filter() const -> const std::string&
{
    return text_filter_;
}

void ProblemsTreeModel::set_sort_mode(ProblemSortMode mode)
{
    sort_mode_ = mode;
    apply_sort();
}

auto ProblemsTreeModel::sort_mode() const -> ProblemSortMode
{
    return sort_mode_;
}

void ProblemsTreeModel::collapse_all()
{
    for (auto& node : filtered_nodes_)
    {
        node.collapsed = true;
    }
}

void ProblemsTreeModel::expand_all()
{
    for (auto& node : filtered_nodes_)
    {
        node.collapsed = false;
    }
}

void ProblemsTreeModel::toggle_file(const std::string& file_uri)
{
    for (auto& node : filtered_nodes_)
    {
        if (node.file_uri == file_uri)
        {
            node.collapsed = !node.collapsed;
            break;
        }
    }
}

void ProblemsTreeModel::apply_filters()
{
    filtered_nodes_.clear();

    for (const auto& node : nodes_)
    {
        ProblemFileNode filtered_node;
        filtered_node.file_uri = node.file_uri;
        filtered_node.display_name = node.display_name;
        filtered_node.collapsed = node.collapsed;

        for (const auto& diag : node.diagnostics)
        {
            // Severity filter — keep diagnostics at or above minimum severity
            // Lower enum value = higher severity (kError=0, kHint=3)
            if (diag.severity > severity_filter_)
            {
                continue;
            }

            // Source filter
            if (!source_filter_.empty() && diag.source != source_filter_)
            {
                continue;
            }

            // Text filter
            if (!text_filter_.empty())
            {
                if (diag.message.find(text_filter_) == std::string::npos &&
                    diag.code.find(text_filter_) == std::string::npos)
                {
                    continue;
                }
            }

            filtered_node.diagnostics.push_back(diag);
        }

        if (!filtered_node.diagnostics.empty())
        {
            filtered_nodes_.push_back(std::move(filtered_node));
        }
    }
}

void ProblemsTreeModel::apply_sort()
{
    switch (sort_mode_)
    {
        case ProblemSortMode::kSeverity:
            std::sort(filtered_nodes_.begin(),
                      filtered_nodes_.end(),
                      [](const ProblemFileNode& lhs, const ProblemFileNode& rhs)
                      { return lhs.max_severity() < rhs.max_severity(); });
            // Also sort diagnostics within each file by severity
            for (auto& node : filtered_nodes_)
            {
                std::sort(node.diagnostics.begin(),
                          node.diagnostics.end(),
                          [](const core::Diagnostic& lhs, const core::Diagnostic& rhs)
                          { return lhs.severity < rhs.severity; });
            }
            break;

        case ProblemSortMode::kFile:
            std::sort(filtered_nodes_.begin(),
                      filtered_nodes_.end(),
                      [](const ProblemFileNode& lhs, const ProblemFileNode& rhs)
                      { return lhs.display_name < rhs.display_name; });
            break;

        case ProblemSortMode::kPosition:
            for (auto& node : filtered_nodes_)
            {
                std::sort(node.diagnostics.begin(),
                          node.diagnostics.end(),
                          [](const core::Diagnostic& lhs, const core::Diagnostic& rhs)
                          {
                              if (lhs.range.start.line != rhs.range.start.line)
                              {
                                  return lhs.range.start.line < rhs.range.start.line;
                              }
                              return lhs.range.start.character < rhs.range.start.character;
                          });
            }
            break;
    }
}

auto ProblemsTreeModel::extract_basename(const std::string& uri) -> std::string
{
    auto last_slash = uri.rfind('/');
    if (last_slash != std::string::npos)
    {
        return uri.substr(last_slash + 1);
    }
    auto last_backslash = uri.rfind('\\');
    if (last_backslash != std::string::npos)
    {
        return uri.substr(last_backslash + 1);
    }
    return uri;
}

} // namespace markamp::ui
