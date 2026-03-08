/// bench_startup.cpp — Benchmarks for startup subsystems
///
/// Per Phase 08 doc: measures each initialization phase independently
/// without wxWidgets GUI. Focuses on non-GUI portions: Config loading,
/// ThemeRegistry scanning, EventBus construction, service construction,
/// and PluginManager activation.

#include "core/Command.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/FeatureRegistry.h"
#include "core/PluginManager.h"
#include "core/StartupPhase.h"
#include "core/ThemeRegistry.h"

// Extension services
#include "core/ContextKeyService.h"
#include "core/DecorationService.h"
#include "core/DiagnosticsService.h"
#include "core/EnvironmentService.h"
#include "core/ExtensionEvents.h"
#include "core/GrammarEngine.h"
#include "core/NotificationService.h"
#include "core/OutputChannelService.h"
#include "core/ProgressService.h"
#include "core/SnippetEngine.h"
#include "core/StatusBarItemService.h"
#include "core/TaskRunnerService.h"
#include "core/TerminalService.h"
#include "core/TextEditorService.h"
#include "core/WebviewService.h"
#include "core/WorkspaceService.h"

#include <benchmark/benchmark.h>

namespace
{

/// BM_Startup_EventBus: EventBus construction time
void BM_Startup_EventBus(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::EventBus bus;
        benchmark::DoNotOptimize(bus);
    }
}
BENCHMARK(BM_Startup_EventBus);

/// BM_Startup_Config: Config construction + default loading
void BM_Startup_Config(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::Config config;
        benchmark::DoNotOptimize(config);
    }
}
BENCHMARK(BM_Startup_Config);

/// BM_Startup_ThemeRegistry: ThemeRegistry construction + builtin load
void BM_Startup_ThemeRegistry(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::ThemeRegistry registry;
        auto result = registry.initialize();
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Startup_ThemeRegistry);

/// BM_Startup_CommandHistory: CommandHistory construction
void BM_Startup_CommandHistory(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::CommandHistory history;
        benchmark::DoNotOptimize(history);
    }
}
BENCHMARK(BM_Startup_CommandHistory);

/// BM_Startup_ExtensionServices: All 21 extension services construction
void BM_Startup_ExtensionServices(benchmark::State& state)
{
    markamp::core::EventBus bus;

    for (auto _ : state)
    {
        auto ctx_key = std::make_unique<markamp::core::ContextKeyService>();
        auto output_chan = std::make_unique<markamp::core::OutputChannelService>();
        auto diag = std::make_unique<markamp::core::DiagnosticsService>();
        auto deco = std::make_unique<markamp::core::DecorationService>();
        auto webview = std::make_unique<markamp::core::WebviewService>();
        auto snippet = std::make_unique<markamp::core::SnippetEngine>();
        auto workspace = std::make_unique<markamp::core::WorkspaceService>();
        auto text_editor = std::make_unique<markamp::core::TextEditorService>();
        auto progress = std::make_unique<markamp::core::ProgressService>();
        auto ext_bus = std::make_unique<markamp::core::ExtensionEventBus>();
        auto env = std::make_unique<markamp::core::EnvironmentService>();
        auto notif = std::make_unique<markamp::core::NotificationService>(bus);
        auto status = std::make_unique<markamp::core::StatusBarItemService>();
        auto grammar = std::make_unique<markamp::core::GrammarEngine>();
        auto terminal = std::make_unique<markamp::core::TerminalService>(bus);
        auto task_runner = std::make_unique<markamp::core::TaskRunnerService>();

        benchmark::DoNotOptimize(ctx_key.get());
        benchmark::DoNotOptimize(task_runner.get());
    }
}
BENCHMARK(BM_Startup_ExtensionServices);

/// BM_Startup_PluginManager: PluginManager construction + activation
void BM_Startup_PluginManager(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::EventBus bus;
        markamp::core::Config config;
        markamp::core::PluginManager pm(bus, config);
        benchmark::DoNotOptimize(pm);
    }
}
BENCHMARK(BM_Startup_PluginManager);

/// BM_Startup_FeatureRegistry: FeatureRegistry construction
void BM_Startup_FeatureRegistry(benchmark::State& state)
{
    markamp::core::EventBus bus;
    markamp::core::Config config;

    for (auto _ : state)
    {
        markamp::core::FeatureRegistry fr(bus, config);
        benchmark::DoNotOptimize(fr);
    }
}
BENCHMARK(BM_Startup_FeatureRegistry);

/// BM_Startup_FullNonGUI: Simulate full non-GUI startup sequence
void BM_Startup_FullNonGUI(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Core infrastructure
        markamp::core::EventBus bus;
        markamp::core::Config config;
        markamp::core::CommandHistory history;

        // Theme subsystem
        markamp::core::ThemeRegistry theme_reg;
        auto theme_result = theme_reg.initialize();
        benchmark::DoNotOptimize(theme_result);

        // Extension services (subset — no GUI-dependent ones)
        auto ctx_key = std::make_unique<markamp::core::ContextKeyService>();
        auto diag = std::make_unique<markamp::core::DiagnosticsService>();
        auto snippet = std::make_unique<markamp::core::SnippetEngine>();
        auto env = std::make_unique<markamp::core::EnvironmentService>();
        auto notif = std::make_unique<markamp::core::NotificationService>(bus);

        // Plugin system
        markamp::core::FeatureRegistry feature_reg(bus, config);
        markamp::core::PluginManager pm(bus, config);

        benchmark::DoNotOptimize(pm);
    }
}
BENCHMARK(BM_Startup_FullNonGUI);

} // namespace
