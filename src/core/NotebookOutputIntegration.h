/// @file NotebookOutputIntegration.h
/// @brief P08-T04: Notebook outputs, search, and export integration.
///
/// Surfaces output state, trust levels, notebook search, and export
/// through shared workbench feedback channels.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Output trust level.
enum class OutputTrustLevel
{
    kTrusted,
    kUntrusted,
    kUnknown,
};

/// Integrates notebook outputs with workbench search and export.
class NotebookOutputIntegration
{
public:
    explicit NotebookOutputIntegration(EventBus& bus);

    /// Get trust level for a notebook's outputs.
    [[nodiscard]] auto trust_level(const std::string& notebook_id) const -> OutputTrustLevel;

    /// Set trust level.
    void set_trust_level(const std::string& notebook_id, OutputTrustLevel level);

    /// Search notebook outputs.
    void search_outputs(const std::string& query);

    /// Export a notebook.
    void export_notebook(const std::string& notebook_id, const std::string& format);

    /// Restore outputs from session.
    void restore_outputs(const std::string& notebook_id);

    /// Clear all outputs for a notebook.
    void clear_outputs(const std::string& notebook_id);

private:
    EventBus& event_bus_;
    OutputTrustLevel default_trust_{OutputTrustLevel::kUntrusted};
};

} // namespace markamp::core
