#pragma once

/**
 * @file PanelHeaderIcons.h
 * @brief Phase 27: Factory functions for per-panel action button definitions.
 *
 * Each function returns a vector of PanelHeaderAction entries pre-configured
 * with icon names, tooltips, shortcuts, and separator/toggle flags appropriate
 * for that panel type.
 */

#include "PanelHeaderBar.h"

#include <vector>

namespace markamp::ui::panel_actions
{

/// Explorer panel: New File, New Folder, Collapse All, Refresh, Filter, Open Terminal
auto explorer_actions() -> std::vector<PanelHeaderAction>;

/// Search panel: Clear, Collapse, Regex toggle, Case toggle, Whole Word toggle, Open in Editor
auto search_actions() -> std::vector<PanelHeaderAction>;

/// Output panel: Clear, Lock Scroll toggle, Word Wrap toggle, Timestamps toggle, Open in Editor
auto output_actions() -> std::vector<PanelHeaderAction>;

/// Problems panel: Error filter, Warning filter, Info filter, Collapse All, Quick Fix All
auto problems_actions() -> std::vector<PanelHeaderAction>;

/// Terminal panel: New Terminal, Split Terminal, Kill, Scroll to Bottom, Clear
auto terminal_actions() -> std::vector<PanelHeaderAction>;

/// Build panel: Build, Rebuild, Clean, Configure, Stop
auto build_actions() -> std::vector<PanelHeaderAction>;

/// Debug Console panel: Clear, Log Level, Timestamps toggle
auto debug_console_actions() -> std::vector<PanelHeaderAction>;

} // namespace markamp::ui::panel_actions
