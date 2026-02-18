/// ContributionValidator.h — V9 Phase 04 Task 10: Contribution validation
///
/// Validates that extension contribution points are well-formed before
/// registration with the PluginManager. Checks for duplicate command IDs,
/// missing required fields, and invalid references.

#pragma once

#include "ExtensionManifest.h"
#include "IPlugin.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A single validation diagnostic.
struct ContributionDiagnostic
{
    enum class Severity
    {
        kWarning,
        kError
    };

    Severity severity{Severity::kWarning};
    std::string contribution_type; ///< e.g. "command", "keybinding", "theme"
    std::string field;             ///< e.g. "command", "key", "label"
    std::string message;           ///< Human-readable description
    std::string extension_id;      ///< Which extension triggered this
};

/// Validates contribution points from extensions and built-in plugins.
class ContributionValidator
{
public:
    ContributionValidator() = default;

    /// Validate all contribution points from an ExtensionContributions struct.
    /// Returns a list of diagnostics (warnings and errors).
    [[nodiscard]] auto validate_extension(const std::string& extension_id,
                                          const ExtensionContributions& contributions)
        -> std::vector<ContributionDiagnostic>;

    /// Validate all contribution points from a PluginManifest's ContributionPoints.
    [[nodiscard]] auto validate_plugin(const std::string& plugin_id,
                                       const PluginManifest::ContributionPoints& contributions)
        -> std::vector<ContributionDiagnostic>;

    /// Check if the last validation had any errors (not just warnings).
    [[nodiscard]] auto has_errors() const -> bool
    {
        return has_errors_;
    }

    /// Reset internal state.
    void reset();

private:
    bool has_errors_{false};

    static void check_commands(const std::string& ext_id,
                               const std::vector<ExtensionCommand>& commands,
                               std::vector<ContributionDiagnostic>& diagnostics);

    static void check_keybindings(const std::string& ext_id,
                                  const std::vector<ExtensionKeybinding>& keybindings,
                                  std::vector<ContributionDiagnostic>& diagnostics);

    static void check_views(const std::string& ext_id,
                            const std::vector<ExtensionView>& views,
                            std::vector<ContributionDiagnostic>& diagnostics);

    static void check_themes(const std::string& ext_id,
                             const std::vector<ExtensionTheme>& themes,
                             std::vector<ContributionDiagnostic>& diagnostics);
};

} // namespace markamp::core
