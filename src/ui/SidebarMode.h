#pragma once

#include <string>

namespace markamp::ui
{

/// Sidebar display mode — replaced with string IDs for dynamic registration.
using SidebarMode = std::string;

// Standard sidebar modes:
constexpr const char* kSidebarModeExplorer = "workbench.view.explorer";
constexpr const char* kSidebarModeSearch = "workbench.view.search";
constexpr const char* kSidebarModeSettings = "workbench.view.settings";
constexpr const char* kSidebarModeThemes = "workbench.view.themes";
constexpr const char* kSidebarModeExtensions = "workbench.view.extensions";
constexpr const char* kSidebarModeNotebooks = "workbench.view.notebooks";
constexpr const char* kSidebarModeCanvas = "workbench.view.canvas";
constexpr const char* kSidebarModeGraph = "workbench.view.graph";
constexpr const char* kSidebarModeAI = "workbench.view.ai";
constexpr const char* kSidebarModeFlashcards = "workbench.view.flashcards";
constexpr const char* kSidebarModeGit = "workbench.view.git";
constexpr const char* kSidebarModeTasks = "workbench.view.tasks";
constexpr const char* kSidebarModeDatabase = "workbench.view.database";
constexpr const char* kSidebarModePresentation = "workbench.view.presentation";

} // namespace markamp::ui
