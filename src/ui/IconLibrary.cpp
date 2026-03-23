#include "ui/IconLibrary.h"

#include "ui/IconRegistry.h"

namespace markamp::ui
{

auto RegisterCoreIcons(IconRegistry& registry) -> void
{
    // Activity Bar Icons
    // NOTE: Every child element MUST have explicit fill/stroke/stroke-width
    // because our SvgDocument parser does NOT inherit from the parent <svg>.
    registry.register_icon("activity-explorer", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="18" height="18" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="9" y1="3" x2="9" y2="21" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-search", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="11" cy="11" r="8" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="21" y1="21" x2="16.65" y2="16.65" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-source-control", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="18" cy="18" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="6" cy="6" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M13 6h3a2 2 0 0 1 2 2v7" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="6" y1="9" x2="6" y2="21" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-run-debug", R"(
        <svg viewBox="0 0 24 24">
            <path d="M5 3 L19 12 L5 21 Z" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-extensions", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="14" y="3" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="14" y="14" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="3" y="14" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-graph", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="18" cy="5" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="6" cy="12" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="18" cy="19" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="8.59" y1="13.51" x2="15.42" y2="17.49" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="15.41" y1="6.51" x2="8.59" y2="10.49" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-settings", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-account", R"(
        <svg viewBox="0 0 24 24">
            <path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="12" cy="7" r="4" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-notebooks", R"(
        <svg viewBox="0 0 24 24">
            <path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2z" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-canvas", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="18" height="18" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="3" y1="9" x2="21" y2="9" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="9" y1="21" x2="9" y2="9" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-ai", R"(
        <svg viewBox="0 0 24 24">
            <path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="12" cy="12" r="4" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-flashcards", R"(
        <svg viewBox="0 0 24 24">
            <rect x="4" y="4" width="16" height="16" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="9" y="9" width="6" height="6" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-git", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="4" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="8" x2="12" y2="2" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="16" x2="12" y2="22" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-tasks", R"(
        <svg viewBox="0 0 24 24">
            <polyline points="9 11 12 14 22 4" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M21 12v7a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-database", R"(
        <svg viewBox="0 0 24 24">
            <path d="M21 5c0 1.66-4 3-9 3s-9-1.34-9-3c0-1.66 4-3 9-3s9 1.34 9 3z" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-presentation", R"(
        <svg viewBox="0 0 24 24">
            <rect x="2" y="3" width="20" height="14" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="8" y1="21" x2="16" y2="21" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="17" x2="12" y2="21" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-extension", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="14" y="3" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="14" y="14" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
            <rect x="3" y="14" width="7" height="7" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("activity-more", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="5" r="1" fill="currentColor" stroke="none" />
            <circle cx="12" cy="12" r="1" fill="currentColor" stroke="none" />
            <circle cx="12" cy="19" r="1" fill="currentColor" stroke="none" />
        </svg>
    )");

    // Toolbar Icons
    registry.register_icon("toolbar-source-view", R"(
        <svg viewBox="0 0 24 24">
            <polyline points="16 18 22 12 16 6" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="8 6 2 12 8 18" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("toolbar-split-view", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="18" height="18" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="3" x2="12" y2="21" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("toolbar-preview-view", R"(
        <svg viewBox="0 0 24 24">
            <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="12" cy="12" r="3" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("toolbar-save", R"(
        <svg viewBox="0 0 24 24">
            <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="17 21 17 13 7 13 7 21" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="7 3 7 8 15 8" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("toolbar-themes", R"(
        <svg viewBox="0 0 24 24">
            <path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("toolbar-focus", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="10" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="12" cy="12" r="4" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="2" x2="12" y2="6" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="18" x2="12" y2="22" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="2" y1="12" x2="6" y2="12" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="18" y1="12" x2="22" y2="12" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    // File Type Icons
    registry.register_icon("filetype-default", R"(
        <svg viewBox="0 0 24 24">
            <path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="13 2 13 9 20 9" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("filetype-folder", R"(
        <svg viewBox="0 0 24 24">
            <path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("filetype-markdown", R"(
        <svg viewBox="0 0 24 24">
            <path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="13 2 13 9 20 9" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M8 17v-5l2 2 2-2v5" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M14 14l2 3 2-3M16 12v5" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("filetype-cpp", R"(
        <svg viewBox="0 0 24 24">
            <path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="13 2 13 9 20 9" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M10 17c-1.5 0-2.5-1-2.5-2.5S8.5 12 10 12c1 0 1.8.5 2.2 1.3" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="14" y1="14" x2="16" y2="14" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="15" y1="13" x2="15" y2="15" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="17" y1="14" x2="19" y2="14" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="18" y1="13" x2="18" y2="15" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("filetype-header", R"(
        <svg viewBox="0 0 24 24">
            <path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="13 2 13 9 20 9" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="9" y1="12" x2="9" y2="17" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="15" y1="12" x2="15" y2="17" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="9" y1="14.5" x2="15" y2="14.5" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("filetype-python", R"(
        <svg viewBox="0 0 24 24">
            <path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="13 2 13 9 20 9" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M9 14h6v2a2 2 0 0 1-2 2H9a2 2 0 0 1-2-2V10a2 2 0 0 1 2-2h4v2" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    // Panel and Action Icons
    registry.register_icon("panel-close", R"(
        <svg viewBox="0 0 24 24">
            <line x1="18" y1="6" x2="6" y2="18" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="6" y1="6" x2="18" y2="18" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("panel-maximize", R"(
        <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="18" height="18" rx="2" ry="2" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("panel-minimize", R"(
        <svg viewBox="0 0 24 24">
            <line x1="5" y1="12" x2="19" y2="12" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("panel-menu", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="1" fill="currentColor" stroke="none" />
            <circle cx="12" cy="5" r="1" fill="currentColor" stroke="none" />
            <circle cx="12" cy="19" r="1" fill="currentColor" stroke="none" />
        </svg>
    )");

    // Status Bar Icons
    registry.register_icon("status-git", R"(
        <svg viewBox="0 0 24 24">
            <line x1="6" y1="3" x2="6" y2="15" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="18" cy="6" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <circle cx="6" cy="18" r="3" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M18 9a9 9 0 0 1-9 9" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("status-error", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="10" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="15" y1="9" x2="9" y2="15" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="9" y1="9" x2="15" y2="15" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("status-warning", R"(
        <svg viewBox="0 0 24 24">
            <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="9" x2="12" y2="13" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="17" x2="12.01" y2="17" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("status-info", R"(
        <svg viewBox="0 0 24 24">
            <circle cx="12" cy="12" r="10" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="16" x2="12" y2="12" fill="none" stroke="currentColor" stroke-width="2" />
            <line x1="12" y1="8" x2="12.01" y2="8" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");

    registry.register_icon("status-sync", R"(
        <svg viewBox="0 0 24 24">
            <polyline points="23 4 23 10 17 10" fill="none" stroke="currentColor" stroke-width="2" />
            <polyline points="1 20 1 14 7 14" fill="none" stroke="currentColor" stroke-width="2" />
            <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15" fill="none" stroke="currentColor" stroke-width="2" />
        </svg>
    )");
}

} // namespace markamp::ui
