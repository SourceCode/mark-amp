#include "ProblemsPanel.h"

namespace markamp::ui
{

void ProblemsPanel::set_service(core::DiagnosticsService* service)
{
    service_ = service;
}

auto ProblemsPanel::problems(core::DiagnosticSeverity min_severity) const
    -> std::vector<ProblemItem>
{
    std::vector<ProblemItem> result;
    if (service_ == nullptr)
    {
        return result;
    }

    for (const auto& uri : service_->uris())
    {
        for (const auto& diag : service_->get(uri))
        {
            if (diag.severity <= min_severity)
            {
                result.push_back({
                    .file_uri = uri,
                    .message = diag.message,
                    .severity = diag.severity,
                    .line = diag.range.start.line,
                    .character = diag.range.start.character,
                    .source = diag.source,
                });
            }
        }
    }
    return result;
}

auto ProblemsPanel::error_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kError) : 0;
}

auto ProblemsPanel::warning_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kWarning)
                               : 0;
}

auto ProblemsPanel::info_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kInformation)
                               : 0;
}

void ProblemsPanel::set_severity_filter(core::DiagnosticSeverity min_severity)
{
    filter_ = min_severity;
}

auto ProblemsPanel::severity_filter() const -> core::DiagnosticSeverity
{
    return filter_;
}

} // namespace markamp::ui
