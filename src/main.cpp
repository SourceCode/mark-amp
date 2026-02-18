// main.cpp -- MarkAmp entry point
// Include full headers for types used in MarkAmpApp's unique_ptr members,
// because wxIMPLEMENT_APP_NO_MAIN triggers the implicit default constructor.
#include "app/MarkAmpApp.h"
#include "core/AppState.h"
#include "core/Command.h"
#include "core/Config.h"
#include "core/ContextKeyService.h"
#include "core/DecorationService.h"
#include "core/DiagnosticsService.h"
#include "core/EnvironmentService.h"
#include "core/EventBus.h"
#include "core/ExtensionEvents.h"
#include "core/FeatureRegistry.h"
#include "core/FileSystemProviderRegistry.h"
#include "core/GrammarEngine.h"
#include "core/InputBoxService.h"
#include "core/LanguageProviderRegistry.h"
#include "core/NotificationService.h"
#include "core/OutputChannelService.h"
#include "core/PluginManager.h"
#include "core/ProgressService.h"
#include "core/QuickPickService.h"
#include "core/SnippetEngine.h"
#include "core/StatusBarItemService.h"
#include "core/TaskRunnerService.h"
#include "core/TerminalService.h"
#include "core/TextEditorService.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"
#include "core/WorkspaceService.h"
#include "platform/PlatformAbstraction.h"

#include <wx/wx.h>

#include <cstdlib>
#include <exception>
#include <iostream>

wxIMPLEMENT_APP_NO_MAIN(markamp::app::MarkAmpApp);

auto main(int argc, char* argv[]) -> int
{
    // Phase 01 Task 3: Top-level exception barrier.
    // Catches unhandled exceptions so we get a clean diagnostic instead
    // of an OS crash dialog. The logger may not be initialized yet, so
    // we also write to std::cerr.
    try
    {
        return wxEntry(argc, argv);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[FATAL] Unhandled exception: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "[FATAL] Unknown unhandled exception\n";
        return EXIT_FAILURE;
    }
}
