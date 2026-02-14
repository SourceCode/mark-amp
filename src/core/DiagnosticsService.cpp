#include "DiagnosticsService.h"

#include <algorithm>

namespace markamp::core
{

const std::vector<Diagnostic> DiagnosticsService::kEmptyDiagnostics;

void DiagnosticsService::set(const std::string& uri, std::vector<Diagnostic> diagnostics)
{
    diagnostics_[uri] = std::move(diagnostics);
    fire_change(uri);
}

auto DiagnosticsService::get(const std::string& uri) const -> const std::vector<Diagnostic>&
{
    auto found = diagnostics_.find(uri);
    return found != diagnostics_.end() ? found->second : kEmptyDiagnostics;
}

auto DiagnosticsService::uris() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(diagnostics_.size());
    for (const auto& [uri, diags] : diagnostics_)
    {
        result.push_back(uri);
    }
    return result;
}

void DiagnosticsService::remove(const std::string& uri)
{
    diagnostics_.erase(uri);
    fire_change(uri);
}

void DiagnosticsService::clear()
{
    auto all_uris = uris();
    diagnostics_.clear();
    for (const auto& uri : all_uris)
    {
        fire_change(uri);
    }
}

auto DiagnosticsService::count_by_severity(DiagnosticSeverity severity) const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [uri, diags] : diagnostics_)
    {
        count += static_cast<std::size_t>(std::count_if(diags.begin(),
                                                        diags.end(),
                                                        [severity](const Diagnostic& diag)
                                                        { return diag.severity == severity; }));
    }
    return count;
}

auto DiagnosticsService::total_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& [uri, diags] : diagnostics_)
    {
        count += diags.size();
    }
    return count;
}

auto DiagnosticsService::on_change(ChangeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

void DiagnosticsService::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const auto& pair)
                                    { return pair.first == listener_id; }),
                     listeners_.end());
}

void DiagnosticsService::fire_change(const std::string& uri)
{
    for (const auto& [id, listener] : listeners_)
    {
        listener(uri);
    }
}

} // namespace markamp::core
