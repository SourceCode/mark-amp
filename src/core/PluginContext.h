#pragma once

#include "ContextKeyService.h"
#include "DecorationService.h"
#include "DiagnosticsService.h"
#include "FileSystemProviderRegistry.h"
#include "LanguageProviderRegistry.h"
#include "OutputChannelService.h"
#include "TreeDataProviderRegistry.h"
#include "WebviewService.h"

#include <string>

namespace markamp::core
{

/// Enhanced plugin context providing access to all VS Code-equivalent API services.
/// This is passed to plugins during activation, giving them access to the full
/// extension API surface.
struct PluginContext
{
    /// Unique extension ID (publisher.name format).
    std::string extension_id;

    /// Absolute path to the extension's installation directory.
    std::string extension_path;

    /// Path to the extension's global storage directory.
    std::string global_storage_path;

    /// Path to the extension's workspace storage directory.
    std::string workspace_storage_path;

    /// Path to the extension's log directory.
    std::string log_path;

    // ── Core Services ──
    ContextKeyService* context_key_service{nullptr};
    OutputChannelService* output_channel_service{nullptr};
    DiagnosticsService* diagnostics_service{nullptr};
    TreeDataProviderRegistry* tree_data_provider_registry{nullptr};
    WebviewService* webview_service{nullptr};
    DecorationService* decoration_service{nullptr};
    FileSystemProviderRegistry* file_system_provider_registry{nullptr};
    LanguageProviderRegistry* language_provider_registry{nullptr};
};

} // namespace markamp::core
