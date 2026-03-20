#!/usr/bin/env python3

from __future__ import annotations

from pathlib import Path
import re


ROOT = Path("/Users/ryanrentfro/code/markamp")
OUT_DIR = ROOT / "docs" / "v17_docs"


def ws(title: str, focus: str, examples: list[str], refs: list[str]) -> dict:
    return {
        "title": title,
        "focus": focus,
        "examples": examples,
        "refs": refs,
    }


PHASES = [
    {
        "id": 1,
        "title": "Canvas_Input_Workbench_And_Interaction_Foundation",
        "objective": "Establish the board shell, input pipeline, command model, and workspace scaffolding required for Miro-grade editing and creation flows.",
        "common_refs": [
            "src/ui/CanvasWorkspacePanel.cpp",
            "src/ui/CanvasPanel.cpp",
            "src/canvas/CanvasWorkbench.cpp",
            "src/canvas/CanvasInputManager.cpp",
            "src/canvas/CanvasToolStateMachine.cpp",
            "src/core/CanvasIntegrationService.cpp",
        ],
        "workstreams": [
            ws("Workspace Shell", "Make canvas a first-class workspace surface with predictable chrome, entry points, and mode transitions.", ["persistent tool rail", "board title and status strip", "inspector and minimap docking"], ["src/ui/CanvasWorkspacePanel.h", "src/ui/ActivityBar.cpp", "src/core/ActivityCommandProvider.cpp"]),
            ws("Board Lifecycle", "Tighten new/open/rename/duplicate/close flows so board management feels native and low-friction.", ["new board from command palette", "inline board rename", "duplicate board with preserved metadata"], ["src/canvas/Board.cpp", "src/canvas/BoardSerializer.cpp", "src/canvas/BoardTemplate.cpp"]),
            ws("Viewport Camera", "Deliver responsive pan, zoom, fit, and focal-point management across small and large boards.", ["zoom to fit", "zoom around cursor", "smooth camera recenter"], ["src/canvas/CameraModel.cpp", "src/canvas/ViewportTransform.cpp", "src/canvas/LargeBoardModel.cpp"]),
            ws("Tool Rail", "Rationalize tool discovery, activation, grouping, and customization for frequent creators.", ["select and pan tools", "drawing tool groups", "recent tool memory"], ["src/canvas/ToolRailModel.cpp", "src/ui/FloatingToolbar.cpp", "src/ui/FloatingToolbarManager.cpp"]),
            ws("Pointer Routing", "Standardize mouse, trackpad, stylus, and touch routing so tools receive coherent pointer state.", ["pointer capture", "hover target updates", "gesture-safe drag start"], ["src/canvas/PointerEventRouter.cpp", "src/canvas/HitTestModel.cpp", "src/canvas/CanvasInputModel.cpp"]),
            ws("Input State Machine", "Harden tool transitions and transient states so creation, editing, and cancellation never conflict.", ["escape to cancel", "tool-local preview state", "resume previous tool after modal action"], ["src/canvas/CanvasToolHost.cpp", "src/canvas/CanvasTool.h", "src/canvas/DrawTool.cpp"]),
            ws("Keyboard Commanding", "Expand keyboard-first execution so canvas power users can stay off the mouse when precision matters.", ["tool shortcuts", "command chords", "mode-aware key handling"], ["src/canvas/KeyboardCommandModel.cpp", "src/core/CommandRegistry.cpp", "src/core/ChordShortcutManager.cpp"]),
            ws("Cursor And Mode Feedback", "Make the active tool, pending action, and allowed drop target legible through cursor and status feedback.", ["crosshair for draw", "grab/grabbing for pan", "invalid-target cursor"], ["src/ui/DragAffordanceController.cpp", "src/ui/FocusRingRenderer.cpp", "src/canvas/ContextMenuModel.cpp"]),
            ws("Grid And Rulers", "Add the spatial scaffolding creators expect when placing or measuring objects.", ["toggle grid", "ruler units", "origin marker"], ["src/canvas/GridGuideController.cpp", "src/canvas/AlignmentGuides.cpp", "src/canvas/CanvasRenderer.cpp"]),
            ws("Snap Preferences", "Make snapping predictable, configurable, and transparent across all transform tools.", ["snap to grid", "snap to geometry", "temporary snap disable modifier"], ["src/canvas/SnapEngine.cpp", "src/canvas/CanvasCommands.cpp", "src/core/Config.cpp"]),
            ws("Context Menus", "Turn right-click and long-press into mode-aware quick action hubs instead of generic command dumps.", ["object menu", "empty-canvas menu", "selection-specific bulk actions"], ["src/canvas/CanvasContextMenu.cpp", "src/ui/ContextMenuRegistry.cpp", "src/ui/ContextMenuBuilder.cpp"]),
            ws("Undo And Redo Plumbing", "Make all canvas operations produce crisp history entries with reliable inversion.", ["coalesced move history", "redo after style edit", "history labels visible in UI"], ["src/canvas/CanvasCommands.h", "src/canvas/CollabUndoRedoManager.cpp", "src/ui/DragUndoAction.cpp"]),
            ws("Autosave And Recovery", "Ensure users can trust boards to survive crashes, force-quits, and sync interruptions.", ["debounced autosave", "restore unsaved board", "dirty-state indicator"], ["src/canvas/BoardSerializerModel.cpp", "src/core/AtomicWriter.cpp", "src/ui/ErrorRecoveryModel.cpp"]),
            ws("Inspector Shell", "Define a scalable inspector frame that supports board, single-object, and multi-object editing states.", ["board properties view", "multi-select mixed values", "contextual action slots"], ["src/canvas/CanvasInspector.cpp", "src/canvas/InspectorModel.cpp", "src/canvas/MetadataPanel.cpp"]),
            ws("Minimap Shell", "Expose orientation and rapid relocation without overloading the main board surface.", ["viewport rectangle", "click to pan", "section labels in minimap"], ["src/canvas/MinimapPanel.cpp", "src/canvas/MinimapModel.cpp", "src/canvas/BoardNavigator.cpp"]),
            ws("Onboarding And Empty States", "Give first-run canvas users enough guidance to succeed without burying experts in chrome.", ["blank board hints", "tool coachmarks", "template-first creation"], ["src/canvas/OnboardingModel.cpp", "src/ui/FirstRunWizard.cpp", "src/ui/EmptyPanelState.cpp"]),
            ws("Board Settings", "Centralize board-level controls for size, background, permissions, and default behaviors.", ["board background picker", "grid defaults", "authoring defaults per board"], ["src/core/ConfigProfile.cpp", "src/canvas/CanvasThemeModel.cpp", "src/canvas/BoardLockController.cpp"]),
            ws("Theme Tokens", "Align the canvas shell and interaction layers with the broader theme system.", ["theme-aware selection color", "board chrome tokens", "high-contrast token fallbacks"], ["src/canvas/CanvasColorToken.cpp", "src/canvas/CanvasColorModel.cpp", "src/core/BuiltInThemes.cpp"]),
            ws("Command Palette Integration", "Make canvas commands discoverable, searchable, and context-ranked.", ["Canvas: New Board", "Canvas: Toggle Grid", "Canvas: Zoom To Selection"], ["src/ui/CommandPalette.cpp", "src/ui/CommandPaletteModel.cpp", "src/core/Command.cpp"]),
            ws("Event And Analytics Contracts", "Define the event vocabulary and instrumentation needed to evolve the canvas safely.", ["tool-switch event", "board-open event", "interaction latency metric"], ["src/canvas/CanvasObjectLifecycleEvents.cpp", "src/core/CommandExecutionLog.cpp", "src/core/ActivityFeed.cpp"]),
        ],
    },
    {
        "id": 2,
        "title": "Selection_Transform_Alignment_And_Spatial_Editing",
        "objective": "Close the gap on selection, movement, transforms, alignment, grouping, and precision editing so authors can manipulate boards at Miro-like speed.",
        "common_refs": [
            "src/canvas/SelectionManager.cpp",
            "src/canvas/SelectionRenderer.cpp",
            "src/canvas/GeometryHandleController.cpp",
            "src/canvas/CanvasObjectAligner.cpp",
            "src/canvas/LayeringService.cpp",
            "src/ui/DragAutoScroll.cpp",
        ],
        "workstreams": [
            ws("Single Selection", "Make object hit selection reliable, legible, and reversible across all object classes.", ["click to select", "selection halo", "clear selection on empty click"], ["src/canvas/SelectionManager.h", "src/canvas/HitTestModel.cpp", "src/canvas/CanvasObject.cpp"]),
            ws("Multi Selection", "Enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.", ["shift-click add", "marquee select", "select all of same type"], ["src/canvas/SelectionSemantics.h", "src/canvas/SelectionManager.cpp", "src/ui/BulkActionController.cpp"]),
            ws("Move Operations", "Support smooth drag-move behavior with clear ownership, snap, and collision feedback.", ["drag selection", "hold space while moving", "coalesced movement history"], ["src/ui/DragController.cpp", "src/canvas/PointerEventRouter.cpp", "src/canvas/SnapEngine.cpp"]),
            ws("Resize Operations", "Deliver handle-based resize flows that preserve object intent and feel precise on high-DPI screens.", ["corner resize", "edge resize", "aspect ratio lock"], ["src/canvas/GeometryHandleController.h", "src/ui/DpiScalingController.cpp", "src/canvas/CanvasTypes.cpp"]),
            ws("Rotation Operations", "Add rotation controls that are discoverable for novices and efficient for expert users.", ["rotation handle", "angle snap", "numeric rotation edit"], ["src/canvas/GeometryHandleController.cpp", "src/canvas/InspectorModel.cpp", "src/canvas/CanvasCommands.cpp"]),
            ws("Alignment Guides", "Make alignment intent visible through guides, measurements, and snap previews.", ["center alignment guide", "edge guide", "distance overlay"], ["src/canvas/AlignmentGuides.h", "src/canvas/SelectionRenderer.cpp", "src/canvas/CanvasRenderer.h"]),
            ws("Object Snapping", "Extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.", ["snap to sibling edge", "snap to frame center", "snap to connector port"], ["src/canvas/ConnectorAnchorModel.cpp", "src/canvas/FrameContainerModel.cpp", "src/canvas/SnapEngine.h"]),
            ws("Distribute And Align Actions", "Expose one-click cleanup actions for board tidying and diagram maintenance.", ["align left", "distribute horizontally", "equalize sizes"], ["src/canvas/CanvasObjectAligner.h", "src/canvas/AutoLayoutModel.cpp", "src/canvas/CanvasInspector.cpp"]),
            ws("Smart Spacing", "Visualize and preserve even spacing while users drag and nudge grouped elements.", ["same-gap preview", "auto-snap to equal spacing", "measurements between cards"], ["src/canvas/LayoutEngine.cpp", "src/canvas/AlignmentGuides.cpp", "src/ui/DragGhostWindow.cpp"]),
            ws("Grouping", "Make group creation, entry, exit, and nested editing understandable and low-risk.", ["group selected objects", "double-click to edit group", "nested groups"], ["src/canvas/GroupingService.h", "src/canvas/GroupObject.cpp", "src/canvas/FrameObject.cpp"]),
            ws("Frame Membership", "Clarify how objects join, leave, and inherit behavior from frames during transforms.", ["drop into frame", "frame title follows selection", "move frame and contents"], ["src/canvas/FrameContainerModel.h", "src/canvas/FrameRenderer.cpp", "src/canvas/Board.cpp"]),
            ws("Layers And Z Order", "Make stacking order explicit so creators can resolve overlaps quickly.", ["bring to front", "send backward", "layer ordering in outline"], ["src/canvas/LayeringModel.cpp", "src/canvas/LayeringService.h", "src/canvas/OutlinePanel.cpp"]),
            ws("Locking And Pinning", "Protect stable content while keeping inspection and collaboration possible.", ["lock position", "unlock from inspector", "pin board chrome"], ["src/canvas/LockingService.h", "src/canvas/BoardLockController.h", "src/canvas/CanvasContextMenu.cpp"]),
            ws("Precision Nudge", "Support keyboard nudges, big-step moves, and coordinate editing for exact placement.", ["arrow-key move", "shift-arrow larger step", "x/y inspector edit"], ["src/canvas/KeyboardCommandModel.h", "src/canvas/CanvasInspector.h", "src/canvas/CanvasInputManager.h"]),
            ws("Duplicate And Paste In Place", "Make duplication preserve intent, ordering, and connector relationships.", ["duplicate with offset", "paste in place", "clone connected card cluster"], ["src/canvas/CanvasClipboardService.cpp", "src/canvas/ClipboardModel.cpp", "src/core/ClipboardService.cpp"]),
            ws("Drag Autoscroll", "Keep transforms fluid when users move objects beyond the current viewport edge.", ["edge autoscroll", "speed ramps with proximity", "auto-pan while drawing selection box"], ["src/ui/DragAutoScroll.h", "src/canvas/CameraModel.h", "src/ui/CanvasPanel.h"]),
            ws("Viewport-Aware Transforms", "Prevent camera changes from breaking object transforms on very large boards.", ["zoom while dragging", "pan mid-transform", "retain handle tracking"], ["src/canvas/LargeBoardModel.h", "src/canvas/ViewportTransform.h", "src/canvas/CameraModel.cpp"]),
            ws("Multi User Selection", "Surface remote locks and live transforms so multiplayer editing stays legible.", ["remote selection outline", "conflict warning on same object", "follow collaborator transform"], ["src/canvas/RemoteSelectionOverlay.h", "src/canvas/RemoteCursorOverlay.cpp", "src/core/CanvasCollabService.cpp"]),
            ws("Transform History Quality", "Tune history granularity so undo feels human, not mechanical.", ["single undo for full drag", "separate undo for rotate and resize", "history labels"], ["src/canvas/CollabUndoRedoManager.h", "src/core/CommandExecutionLog.cpp", "src/ui/HistoryPanel.cpp"]),
            ws("Selection Action Surfaces", "Expose the highest-value transform actions in context-aware mini toolbars.", ["floating action bar", "selection quick align", "bulk style and arrange"], ["src/ui/SelectionActionBar.cpp", "src/ui/FloatingToolbarManager.cpp", "src/ui/CanvasControlModel.cpp"]),
        ],
    },
    {
        "id": 3,
        "title": "Drawing_Text_Shapes_And_Style_Authoring",
        "objective": "Upgrade pen, shape, text, note, and style authoring so the canvas feels built for active creation rather than passive placement.",
        "common_refs": [
            "src/canvas/DrawTool.cpp",
            "src/canvas/CanvasTextEditor.cpp",
            "src/canvas/TextBoxRenderer.cpp",
            "src/canvas/FreehandPathRenderer.cpp",
            "src/canvas/CanvasStylePresetService.cpp",
            "src/canvas/ColorPaletteController.cpp",
        ],
        "workstreams": [
            ws("Freehand Pen Engine", "Improve stroke capture, smoothing, replay, and cleanup for rapid sketching.", ["pressure-sensitive stroke", "stabilized line", "ink that stays editable"], ["src/canvas/FreehandModel.cpp", "src/canvas/FreehandPath.cpp", "src/canvas/BenchmarkModel.cpp"]),
            ws("Pen Presets", "Offer meaningful brush presets and recently used styles without cluttering the tool rail.", ["marker preset", "highlighter preset", "saved custom pen"], ["src/canvas/CanvasStylePresetService.h", "src/canvas/FillEffectsController.cpp", "src/ui/FloatingToolbar.cpp"]),
            ws("Eraser And Lasso", "Add correction tools that are fast enough for active sketching sessions.", ["stroke eraser", "segment erase", "lasso select hand-drawn content"], ["src/canvas/FreehandModel.h", "src/canvas/SelectionSemantics.h", "src/canvas/CanvasInputModel.h"]),
            ws("Shape Tool", "Make shape creation, switching, and resizing feel immediate and coherent.", ["rectangle and ellipse drag-create", "last-used shape memory", "shape quick insert"], ["src/canvas/DiagramShapeObject.h", "src/canvas/CanvasObjectFactory.cpp", "src/canvas/ShapeRenderer.cpp"]),
            ws("Text Box Authoring", "Turn canvas text into a serious authoring surface with inline editing and robust layout behavior.", ["double-click to edit", "auto-resizing text box", "text selection inside object"], ["src/canvas/CanvasTextModel.h", "src/canvas/CanvasTextEditor.h", "src/canvas/TextBoxRenderer.h"]),
            ws("Sticky Notes", "Make notes fast to create, easy to color, and optimized for workshop scale.", ["click-to-create sticky", "color cycle shortcut", "author initials on note"], ["src/canvas/StickyNoteRenderer.cpp", "src/canvas/CanvasObjectFactory.h", "src/canvas/CanvasCommands.h"]),
            ws("Color Authoring", "Provide intentional palette workflows instead of raw color pickers everywhere.", ["theme palette swatches", "recent colors", "board palette"], ["src/canvas/CanvasColorModel.h", "src/ui/ColorPaletteGenerator.cpp", "src/core/Color.cpp"]),
            ws("Typography Controls", "Expose the most useful typographic controls for boards, labels, and diagrams.", ["font size presets", "line height control", "text alignment"], ["src/canvas/CanvasTextModel.cpp", "src/canvas/CanvasInspector.cpp", "src/ui/FloatingFormatBar.cpp"]),
            ws("Fill Stroke And Effects", "Support modern object styling without making objects visually noisy.", ["fill opacity", "stroke width presets", "soft shadow toggle"], ["src/canvas/FillEffectsController.h", "src/canvas/CanvasColorToken.h", "src/ui/ElevationSystem.cpp"]),
            ws("Style Presets", "Make style reuse one-click so teams can author visually consistent boards.", ["apply preset to selection", "save brand preset", "preset categories"], ["src/canvas/CanvasStylePresetService.cpp", "src/canvas/CanvasObject.cpp", "src/core/ConfigAuditTrail.cpp"]),
            ws("Inline Rich Text", "Allow more than plain text in objects where richer authoring improves board clarity.", ["bold and italic inside text box", "bullet lists in notes", "inline links"], ["src/canvas/CanvasTextEditor.cpp", "src/ui/FloatingFormatBar.h", "src/core/InlineFrontmatterRenderer.h"]),
            ws("Shape Library Quick Insert", "Reduce time-to-object by improving discovery and insertion of common shapes.", ["searchable shape picker", "recent shapes", "drag from library"], ["src/canvas/DiagramLibraryPanel.h", "src/ui/IconGalleryDialog.cpp", "src/canvas/DiagramCoordinator.cpp"]),
            ws("Shape Recognition", "Convert sketch intent into clean geometry when the user wants precision.", ["recognize rough rectangle", "suggest arrow cleanup", "keep hand-drawn when confidence is low"], ["src/canvas/ShapeRecognizer.cpp", "src/canvas/FreehandPathRenderer.h", "src/canvas/DiagramShapeObject.cpp"]),
            ws("Image Annotation", "Let users mark up screenshots and reference images without switching tools.", ["draw over image", "add pin comment", "blur a sensitive area"], ["src/canvas/ImageObject.cpp", "src/canvas/ImageObjectRenderer.cpp", "src/canvas/CommentObject.cpp"]),
            ws("Comments And Callouts", "Support review and critique flows directly on authored board content.", ["callout arrow with text", "threaded comment badge", "resolve comment"], ["src/ui/CanvasCommentsPanel.cpp", "src/canvas/CommentObject.h", "src/core/ActivityTimeline.cpp"]),
            ws("Tables For Authoring", "Make simple tables useful for planning and comparison inside the canvas.", ["insert table", "resize column", "paste spreadsheet cells"], ["src/canvas/TableRenderer.cpp", "src/canvas/CanvasClipboardService.h", "src/canvas/CanvasObjectFactory.cpp"]),
            ws("Icons Stickers And Emoji", "Speed up expressive communication with lightweight graphic elements.", ["emoji picker", "colorable icon", "sticker pack"], ["src/canvas/IconObject.h", "src/canvas/IconLibrary.h", "src/ui/IconLibrary.cpp"]),
            ws("Creator Shortcuts", "Map common authoring actions to shortcuts that scale for daily heavy use.", ["toggle pen tool", "duplicate style", "cycle note color"], ["src/canvas/KeyboardCommandModel.cpp", "src/core/CommandRegistry.h", "src/ui/CommandPaletteModel.cpp"]),
            ws("Bulk Style Editing", "Make multi-object styling practical instead of a sequence of one-off edits.", ["apply one text style to 40 notes", "normalize border widths", "batch recolor selection"], ["src/canvas/InspectorModel.h", "src/ui/BulkActionController.h", "src/canvas/CanvasInspector.cpp"]),
            ws("Authoring Motion Feedback", "Use restrained motion and previews to make creation and editing feel responsive.", ["shape creation preview", "subtle selection fade", "toolbar reveal animation"], ["src/ui/AnimationDriver.cpp", "src/ui/ControlAnimator.cpp", "src/ui/ControlMotionTokens.cpp"]),
        ],
    },
    {
        "id": 4,
        "title": "Connectors_Diagrams_And_Structured_Content_Systems",
        "objective": "Make connectors, diagrams, maps, structured content, and board organization strong enough for process design, systems thinking, and planning boards.",
        "common_refs": [
            "src/canvas/ConnectorData.cpp",
            "src/canvas/ConnectorRouteEngine.cpp",
            "src/canvas/DiagramCoordinator.cpp",
            "src/canvas/MindMapController.cpp",
            "src/canvas/KanbanController.cpp",
            "src/canvas/FrameObject.cpp",
        ],
        "workstreams": [
            ws("Connector Creation", "Reduce connector creation friction across object types and board densities.", ["drag from port", "click-click connector", "connector from quick action"], ["src/canvas/ConnectorModel.cpp", "src/canvas/ConnectorAnchorModel.h", "src/canvas/CanvasObjectLifecycleEvents.h"]),
            ws("Connector Routing", "Support straight, curved, and orthogonal routing with reliable reflow.", ["orthogonal elbow", "curved line", "reroute around obstacles"], ["src/canvas/ConnectorRoutingService.cpp", "src/canvas/ConnectorRenderer.h", "src/canvas/LayoutEngine.h"]),
            ws("Connector Labels And Ports", "Make relationships readable by exposing labels, port choice, and endpoint semantics.", ["label on connection", "switch endpoint port", "show directional arrowhead"], ["src/canvas/ConnectorAnchorModel.cpp", "src/canvas/ConnectorRenderer.cpp", "src/canvas/CanvasInspector.h"]),
            ws("Diagram Library", "Offer a high-signal library of flowchart and systems shapes.", ["flowchart diamond", "database cylinder", "service hexagon"], ["src/canvas/DiagramLibraryPanel.cpp", "src/canvas/DiagramShapeObject.h", "src/ui/IconRenderer.cpp"]),
            ws("Flowchart Grammar", "Encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.", ["decision shape defaults", "connector arrow presets", "label placement rules"], ["src/canvas/DiagramCoordinator.h", "src/canvas/ConnectorData.h", "src/canvas/CanvasTemplateEngine.cpp"]),
            ws("Mind Maps", "Make central-node ideation and branch editing feel purpose-built.", ["tab adds child", "enter adds sibling", "collapse branch"], ["src/canvas/MindMapNode.h", "src/canvas/MindMapModel.cpp", "src/canvas/AutoLayoutModel.h"]),
            ws("Kanban Boards", "Support structured planning boards without forcing users into a separate product mode.", ["insert kanban", "drag card between lanes", "swimlane settings"], ["src/canvas/KanbanModel.h", "src/canvas/KanbanObjects.h", "src/ui/av/AVKanbanPanel.cpp"]),
            ws("Frames And Sections", "Use structural containers to organize large diagrams and workshop spaces.", ["frame title", "section color band", "quick jump to section"], ["src/canvas/FrameRenderer.h", "src/canvas/SectionRenderer.cpp", "src/canvas/OutlinePanel.h"]),
            ws("Swimlanes And Grids", "Add layout primitives for service design, journey maps, and process boards.", ["lane headers", "resizable lanes", "grid templates"], ["src/canvas/BoardTemplate.h", "src/canvas/LayoutEngine.cpp", "src/canvas/GridGuideController.h"]),
            ws("Container Auto Layout", "Help diagrams stay tidy by letting containers manage spacing and order.", ["auto-stack cards", "equal spacing in frame", "wrap items within section"], ["src/canvas/AutoLayoutModel.cpp", "src/canvas/FrameContainerModel.cpp", "src/canvas/CanvasObjectAligner.cpp"]),
            ws("Advanced Tables", "Increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.", ["header freeze", "reorder columns", "cell alignment presets"], ["src/canvas/TableRenderer.cpp", "src/canvas/CanvasTextEditor.cpp", "src/canvas/CanvasClipboardService.cpp"]),
            ws("Cross Board Links", "Let creators connect related boards and navigate systems of boards.", ["link object to board", "preview linked board", "open in new tab"], ["src/canvas/CrossBoardLink.cpp", "src/core/CrossSurfaceRouter.h", "src/canvas/BoardNavigator.cpp"]),
            ws("Diagram From Selection", "Accelerate structure creation from existing content clusters.", ["convert notes to flowchart", "auto-connect selected boxes", "generate frame per cluster"], ["src/canvas/CanvasObjectFactory.h", "src/canvas/DiagramCoordinator.cpp", "src/canvas/AutoLayoutModel.h"]),
            ws("Connected Layout Cleanup", "Preserve connector readability while users align, distribute, and move connected objects.", ["reroute after align", "keep labels visible", "respect locked endpoints"], ["src/canvas/ConnectorRouteEngine.h", "src/canvas/CanvasObjectAligner.h", "src/canvas/SelectionManager.cpp"]),
            ws("Relationship Metadata", "Support metadata on links and diagrams so boards can carry richer meaning.", ["connector type property", "owner on flow step", "status chip on node"], ["src/canvas/MetadataPanel.h", "src/canvas/CanvasInspector.cpp", "src/core/AttributeService.cpp"]),
            ws("Diagram Templates", "Seed structured content creation with reusable diagram starters.", ["service blueprint template", "sequence flow template", "org chart starter"], ["src/canvas/CanvasTemplateEngine.h", "src/canvas/BoardTemplate.cpp", "src/canvas/DiagramLibraryPanel.h"]),
            ws("Dependency Validation", "Catch broken or invalid diagram states before they confuse users.", ["dangling connector warning", "cyclic dependency marker", "invalid port assignment"], ["src/canvas/IntegrityModel.cpp", "src/canvas/AuditModel.h", "src/canvas/ConnectorModel.h"]),
            ws("Diagram Export Readiness", "Prepare structured boards for export without losing legibility.", ["fit export bounds to frame", "preserve connector labels", "export transparent background"], ["src/canvas/CanvasExportService.cpp", "src/core/ExportService.cpp", "src/canvas/ExportModel.h"]),
            ws("Collaborative Diagram Editing", "Make multi-user diagram editing feel safe and predictable.", ["remote connector reroute", "shared node editing", "conflict message on same label"], ["src/canvas/CollabEventLogger.h", "src/core/CanvasCollabService.h", "src/canvas/RemoteSelectionOverlay.cpp"]),
            ws("Diagram Test Matrix", "Build durable coverage for structured content interactions and regressions.", ["connector round-trip tests", "mind map layout tests", "kanban drag regression suite"], ["tests/unit", "src/canvas/BenchmarkModel.h", "src/canvas/IntegrityModel.h"]),
        ],
    },
    {
        "id": 5,
        "title": "Assets_Embeds_And_Rich_Content_Authoring",
        "objective": "Strengthen asset handling so images, files, embeds, PDFs, links, icons, and widgets behave like first-class canvas materials.",
        "common_refs": [
            "src/canvas/ImageObject.cpp",
            "src/canvas/MediaEmbedModel.cpp",
            "src/canvas/PDFPageObject.cpp",
            "src/canvas/BookmarkCardObject.cpp",
            "src/canvas/AppWidgetObject.cpp",
            "src/core/AssetService.cpp",
        ],
        "workstreams": [
            ws("Image Placement", "Make image insertion, scaling, and replacement intuitive for reference-heavy boards.", ["drag image onto canvas", "replace image while keeping size", "double-click to open crop"], ["src/canvas/ImageObjectRenderer.h", "src/canvas/ImageAssetModel.cpp", "src/ui/ImagePreviewToolbar.cpp"]),
            ws("Video And Web Embeds", "Support rich external content without destabilizing the board shell.", ["paste video URL", "preview embed card", "open inline or external"], ["src/canvas/MediaEmbedModel.h", "src/core/ContentSecurityPolicy.cpp", "src/core/BookmarkService.cpp"]),
            ws("PDF Pages", "Treat PDFs as editable board materials instead of opaque file blobs.", ["insert page range", "resize page thumbnail", "open source PDF"], ["src/canvas/PDFImporter.cpp", "src/canvas/PdfObjectModel.cpp", "src/core/AssetLinkIntegrity.cpp"]),
            ws("Bookmark Cards", "Turn raw URLs into useful reference objects with preview quality and safe behavior.", ["auto-preview pasted URL", "editable title and summary", "favicon and domain chip"], ["src/canvas/MetadataScraper.cpp", "src/core/BookmarkExporter.cpp", "src/core/BookmarkNavigator.cpp"]),
            ws("App Widgets", "Define a consistent UX contract for interactive widgets on the board.", ["widget insert menu", "resize live widget", "widget loading state"], ["src/core/CanvasAppManifest.h", "src/canvas/AppWidgetObject.h", "src/core/BuiltInPlugins.cpp"]),
            ws("Icon Insertion", "Make icon use faster than importing arbitrary SVGs for common needs.", ["search icon set", "insert with theme color", "swap icon in place"], ["src/canvas/IconLibrary.cpp", "src/canvas/IconObject.cpp", "src/ui/IconGalleryDialog.h"]),
            ws("Drag And Drop Intake", "Make external content ingestion predictable regardless of source application.", ["drop image file", "drop markdown snippet", "drop URL from browser"], ["src/ui/DragDropModel.cpp", "src/ui/DropTargetHighlighter.cpp", "src/canvas/CanvasObjectFactory.cpp"]),
            ws("Clipboard Ingest", "Support rich pastes from browsers, office tools, and screenshots.", ["paste screenshot", "paste rich text as card", "paste spreadsheet range"], ["src/core/ClipboardSanitizer.cpp", "src/canvas/CanvasClipboardService.h", "src/core/ClipboardService.h"]),
            ws("Media Optimization", "Balance quality and performance for boards that carry many assets.", ["deferred thumbnail generation", "lazy load offscreen media", "image compression on save"], ["src/core/AssetOptimizer.cpp", "src/canvas/CanvasPerformanceMonitor.h", "src/core/AsyncFileLoader.cpp"]),
            ws("Cropping And Masking", "Support the basic media editing affordances users expect inside a board.", ["crop image", "round mask avatar", "fit/fill image modes"], ["src/canvas/ImageObjectRenderer.cpp", "src/canvas/GeometryHandleController.cpp", "src/canvas/CanvasInspector.h"]),
            ws("Captions And Metadata", "Expose source, alt text, owner, and caption metadata where it matters.", ["image caption", "embed source link", "asset alt text"], ["src/canvas/MetadataPanel.cpp", "src/core/AssetTypes.h", "src/core/AccessibilityManager.cpp"]),
            ws("Asset Library Panel", "Give frequent creators a reusable asset shelf instead of repeated file picking.", ["recent uploads", "favorite icons", "team image kit"], ["src/core/AssetService.h", "src/ui/ExplorerPanel.cpp", "src/ui/FileTreeModel.cpp"]),
            ws("Asset Styling", "Let assets adopt frames, shadows, borders, and presentation treatments quickly.", ["polaroid frame", "device mock frame", "image border presets"], ["src/canvas/FillEffectsController.cpp", "src/ui/ElevationSystem.h", "src/canvas/CanvasStylePresetService.cpp"]),
            ws("Mixed Media Boards", "Ensure text, media, and shapes compose cleanly in presentations and working boards.", ["captioned screenshot cluster", "video with notes", "PDF page with annotation"], ["src/canvas/CanvasRenderer.cpp", "src/canvas/FrameObject.h", "src/canvas/SelectionRenderer.h"]),
            ws("Import And Convert", "Offer better conversion paths from common external artifacts into editable canvas content.", ["import slide as images", "convert URL list to cards", "turn markdown list into notes"], ["src/core/DocumentImporter.cpp", "src/core/MarkdownImporter.cpp", "src/core/HtmlImporter.cpp"]),
            ws("Media Review Flows", "Support review-centric actions on rich assets without leaving the board.", ["comment on image region", "approve embed", "resolve PDF annotation"], ["src/ui/CanvasCommentsPanel.h", "src/core/NotificationService.cpp", "src/canvas/CommentObject.cpp"]),
            ws("Licensing And Security", "Handle external content safely while preserving expected workflows.", ["blocked insecure embed", "license metadata on asset", "trusted domain allowlist"], ["src/core/ContentSecurityPolicy.h", "src/core/PrivacyManager.cpp", "src/core/UrlSanitizer.cpp"]),
            ws("Asset Serialization And Sync", "Preserve references, thumbnails, and metadata across save and sync paths.", ["embedded image round-trip", "link integrity after move", "asset sync conflict resolution"], ["src/canvas/BoardSerializer.h", "src/core/CloudSyncService.cpp", "src/core/AssetLinkIntegrity.h"]),
            ws("Rich Content Export", "Ensure rich objects export clearly to image, PDF, and HTML targets.", ["export image with caption", "PDF export of mixed-media frame", "HTML export keeps links"], ["src/canvas/CanvasExportService.h", "src/core/HtmlExporter.cpp", "src/core/PdfExporter.cpp"]),
            ws("Asset Test Coverage", "Build coverage for insertion, rendering, persistence, and failure handling across asset types.", ["image load failure test", "bookmark preview fallback", "PDF page serialization test"], ["tests/unit", "src/canvas/IntegrityModel.cpp", "src/core/CrossModuleTestSuite.cpp"]),
        ],
    },
    {
        "id": 6,
        "title": "Collaboration_Facilitation_And_Multiplayer_Canvas_UX",
        "objective": "Raise real-time and async collaboration so teams can workshop, review, and facilitate directly on the canvas with trust in shared state.",
        "common_refs": [
            "src/core/CanvasCollabService.cpp",
            "src/canvas/RemoteCursorOverlay.cpp",
            "src/canvas/RemoteSelectionOverlay.cpp",
            "src/canvas/FollowModeController.cpp",
            "src/canvas/OfflineSyncQueue.cpp",
            "src/ui/CanvasFacilitationPanel.cpp",
        ],
        "workstreams": [
            ws("Live Presence", "Show collaborators clearly without drowning the board in noise.", ["colored remote cursors", "presence list", "author badge on active object"], ["src/canvas/RemoteCursorOverlay.h", "src/core/ActivityFeed.h", "src/ui/Badge.cpp"]),
            ws("Remote Selections And Follow", "Make shared focus visible and controllable during collaboration sessions.", ["follow presenter", "show remote selection box", "jump to teammate"], ["src/canvas/FollowModeController.h", "src/canvas/RemoteSelectionOverlay.h", "src/canvas/BoardNavigator.h"]),
            ws("Conflict Handling", "Reduce surprise when two people edit the same thing at once.", ["soft lock on text edit", "merge-safe style changes", "conflict toast with recovery action"], ["src/core/ChangeTracker.cpp", "src/canvas/CollabUndoRedoManager.cpp", "src/ui/ErrorRecoveryModel.h"]),
            ws("Comment Threads", "Turn comments into anchored, navigable review conversations.", ["pin comment to object", "thread replies", "resolve and reopen"], ["src/ui/CanvasCommentsPanel.cpp", "src/canvas/CommentObject.h", "src/core/NotificationService.h"]),
            ws("Voting And Reactions", "Support lightweight facilitation mechanics used in workshops and prioritization.", ["dot voting", "emoji reaction burst", "timer-bound voting session"], ["src/ui/CanvasFacilitationPanel.h", "src/core/ActivityTimeline.h", "src/canvas/PolishModel.cpp"]),
            ws("Timer And Facilitation Controls", "Give facilitators on-board controls instead of requiring external tools.", ["countdown timer", "agenda checkpoints", "lock board during instructions"], ["src/ui/CanvasFacilitationPanel.cpp", "src/canvas/BoardLockController.cpp", "src/core/AutomationRule.cpp"]),
            ws("Private Reveal And Presentation", "Support workshop reveal patterns without duplicating the whole presentation system.", ["hide unrevealed frame", "reveal next section", "private prep area"], ["src/canvas/PrivateRevealController.cpp", "src/core/PresentationManager.cpp", "src/canvas/SectionRenderer.h"]),
            ws("Permissions And Shared Locks", "Let collaborators know what is editable, review-only, or temporarily protected.", ["editable vs locked frame", "role-based control", "unlock request"], ["src/core/RuntimePolicy.cpp", "src/canvas/LockingService.cpp", "src/core/PrivacyManager.h"]),
            ws("Offline Queue", "Protect collaboration flows across intermittent connectivity.", ["queue edits offline", "replay with status", "surface sync conflict"], ["src/canvas/OfflineSyncQueue.h", "src/core/CloudSyncTypes.h", "src/core/Backpressure.h"]),
            ws("Activity Feed", "Expose meaningful recent activity without turning the UI into a log viewer.", ["who moved frame", "who resolved comment", "board activity summary"], ["src/core/ActivityFeed.cpp", "src/core/ActivityTimeline.cpp", "src/ui/HistoryPanel.h"]),
            ws("Change Highlights", "Help users reorient to what changed since they last looked at a board.", ["recent edits glow", "jump to new comments", "highlight unresolved changes"], ["src/canvas/AuditModel.cpp", "src/canvas/RemoteSelectionOverlay.cpp", "src/ui/NotificationCenter.cpp"]),
            ws("Invites And Session Entry", "Make joining a canvas session simple and comprehensible for new collaborators.", ["join from link", "guest entry state", "first-run collab hints"], ["src/ui/FirstRunWizard.h", "src/core/CanvasIntegrationService.h", "src/core/EnvironmentService.cpp"]),
            ws("Collaborator Panels", "Provide optional side panels for awareness, moderation, and facilitation.", ["participants drawer", "speaker indicator", "moderator actions"], ["src/ui/CanvasFacilitationPanel.h", "src/ui/ToolWindowHost.cpp", "src/ui/RichTooltip.cpp"]),
            ws("Co Editing Text", "Support multiplayer text editing inside canvas objects without corrupting content.", ["shared text cursor", "presence in text object", "safe exit from inline editor"], ["src/canvas/CanvasTextEditor.h", "src/core/AIConversationHistory.cpp", "src/core/ChangeTracker.h"]),
            ws("Workshop Templates", "Package recurring collaborative rituals as ready-to-run board flows.", ["retrospective board", "brainwriting template", "impact-effort matrix workshop"], ["src/canvas/BoardTemplate.h", "src/canvas/CanvasTemplateEngine.cpp", "src/canvas/OnboardingModel.h"]),
            ws("Async Review", "Support asynchronous sign-off and review workflows beyond live sessions.", ["approve frame", "request changes", "review status chip"], ["src/core/NotificationService.cpp", "src/core/TaskService.cpp", "src/canvas/MetadataPanel.cpp"]),
            ws("Moderation And Recovery", "Give facilitators tools to recover from chaos or misuse during live boards.", ["freeze edits", "clear reactions", "remove disruptive collaborator from follow mode"], ["src/core/SafeMode.cpp", "src/canvas/BoardLockController.h", "src/ui/DialogModel.cpp"]),
            ws("Multiplayer Performance", "Keep presence and live updates lightweight as collaboration scale grows.", ["defer offscreen remote cursors", "batch remote selection updates", "presence throttling"], ["src/core/AdaptiveThrottle.h", "src/canvas/CanvasPerformanceMonitor.cpp", "src/core/Backpressure.h"]),
            ws("Collaboration Analytics", "Measure the right workshop signals without over-collecting.", ["active collaborators count", "comment resolution time", "voting participation"], ["src/core/ActivityFeed.h", "src/core/CommandExecutionLog.h", "src/core/PrivacyManager.cpp"]),
            ws("Collaboration Coverage", "Build testable contracts for multi-user behavior and edge conditions.", ["offline replay test", "same-object edit race", "presenter follow regression"], ["tests/unit", "src/core/CrossModuleTestSuite.h", "src/canvas/CollabEventLogger.cpp"]),
        ],
    },
    {
        "id": 7,
        "title": "Navigation_Search_Organization_And_Large_Board_Wayfinding",
        "objective": "Make large boards explorable through minimaps, outlines, search, metadata, navigation history, and saved viewpoints.",
        "common_refs": [
            "src/canvas/MinimapPanel.cpp",
            "src/canvas/OutlinePanel.cpp",
            "src/canvas/CanvasSearch.cpp",
            "src/canvas/BoardNavigator.cpp",
            "src/canvas/LargeBoardModel.cpp",
            "src/ui/BreadcrumbBar.cpp",
        ],
        "workstreams": [
            ws("Minimap Navigation", "Make the minimap accurate, interactive, and useful at both small and huge scales.", ["viewport drag", "section labels", "selection markers"], ["src/canvas/MinimapModel.h", "src/canvas/CanvasRenderer.cpp", "src/canvas/CameraModel.h"]),
            ws("Outline Navigation", "Turn the outline into a serious structural navigator, not just an object list.", ["hierarchy by frame", "inline rename from outline", "jump to selected object"], ["src/canvas/OutlinePanel.h", "src/canvas/FrameObject.h", "src/canvas/GroupObject.h"]),
            ws("Search And Find", "Support fast search across text, labels, comments, and metadata directly on boards.", ["find note text", "jump to next match", "highlight all results"], ["src/canvas/CanvasSearchModel.h", "src/ui/FindBar.cpp", "src/core/SearchQueryParser.cpp"]),
            ws("Tagging And Filters", "Help users isolate board slices using tags, object types, and collaboration state.", ["filter by tag", "show only unresolved comments", "filter by owner"], ["src/canvas/TagManager.cpp", "src/canvas/TagPanel.h", "src/core/Tag.h"]),
            ws("Sections And Landmarks", "Use structural landmarks to anchor orientation on large canvases.", ["section navigator", "landmark headers", "jump to frame"], ["src/canvas/SectionRenderer.h", "src/canvas/BoardNavigator.cpp", "src/canvas/FrameRenderer.cpp"]),
            ws("Large Board Streaming", "Keep navigation responsive as object counts and board size increase.", ["lazy render offscreen objects", "stream metadata into outline", "fast camera jumps"], ["src/canvas/Quadtree.cpp", "src/canvas/CanvasPerformanceMonitor.h", "src/canvas/LargeBoardModel.h"]),
            ws("Breadcrumbs And History", "Track where users have been so they can recover orientation quickly.", ["board breadcrumb", "back to previous view", "open recent frames"], ["src/ui/BreadcrumbModel.cpp", "src/core/AppState.cpp", "src/core/BookmarkNavigator.h"]),
            ws("Zoom Presets", "Expose practical zoom presets and fit modes instead of raw percentage control alone.", ["fit board", "fit selection", "100 percent"], ["src/canvas/ViewportTransform.h", "src/ui/CanvasControlModel.h", "src/canvas/CameraModel.cpp"]),
            ws("Board Bookmarks", "Let users save camera states and important regions for repeat navigation.", ["bookmark current view", "named board waypoint", "share view link"], ["src/core/BookmarkService.cpp", "src/canvas/BoardNavigator.h", "src/core/CrossSurfaceRouter.h"]),
            ws("Metadata Driven Navigation", "Surface owner, status, and object type data as navigation aids.", ["jump to overdue area", "navigate by assignee", "find all decision nodes"], ["src/canvas/MetadataPanel.h", "src/core/AttributeService.h", "src/canvas/CanvasSearch.cpp"]),
            ws("Cross Board Traversal", "Make linked boards feel like part of one navigable workspace.", ["open linked board in split", "back to source board", "peek linked board"], ["src/canvas/CrossBoardLink.h", "src/core/CrossSurfaceRouter.h", "src/ui/EditorGroupManager.cpp"]),
            ws("Selection Sync", "Keep side panels and navigation surfaces synced with the active canvas context.", ["outline follows selection", "inspector reveals selected object", "minimap highlights selection"], ["src/canvas/SelectionManager.h", "src/canvas/InspectorModel.cpp", "src/canvas/MinimapModel.cpp"]),
            ws("Saved Views", "Package complex board states into reusable named views.", ["review mode view", "presentation view", "editing focus view"], ["src/core/ConfigProfile.h", "src/canvas/CanvasThemeModel.h", "src/canvas/BoardSerializerModel.h"]),
            ws("Presentation Navigation", "Bridge working-board navigation and presentation-style movement through a board.", ["next frame", "focus path", "present from section"], ["src/core/PresentationManager.h", "src/canvas/PrivateRevealController.h", "src/canvas/BoardNavigator.cpp"]),
            ws("Semantic Navigation", "Use clustering and object meaning to help users move through idea-heavy boards.", ["navigate by cluster", "jump to all decisions", "find all connectors with warnings"], ["src/canvas/AutoLayoutModel.cpp", "src/core/GraphService.cpp", "src/canvas/IntegrityModel.h"]),
            ws("Quick Action Navigation", "Let users navigate by intent, not only by visual scanning.", ["jump to unresolved comments", "center newest objects", "show all locked items"], ["src/ui/CommandPalette.h", "src/core/CommandRegistry.cpp", "src/core/ActivityCommandProvider.cpp"]),
            ws("Discoverability", "Improve the learnability of navigation controls and shortcuts.", ["tooltip with shortcut", "discover saved views", "first-use zoom tips"], ["src/ui/RichTooltip.h", "src/canvas/OnboardingModel.cpp", "src/ui/CanvasControlModel.cpp"]),
            ws("Responsive Layouts", "Keep navigation surfaces usable across narrow windows and high-density desktops.", ["collapse minimap", "dock outline", "compact canvas chrome"], ["src/ui/CompactLayoutModel.cpp", "src/ui/ComponentSizeResolver.cpp", "src/ui/CanvasWorkspacePanel.h"]),
            ws("Wayfinding Telemetry", "Measure where users get lost so navigation work stays grounded in real friction.", ["frequent zoom thrash", "repeat search without clickthrough", "outline-open after long idle"], ["src/core/CommandExecutionLog.cpp", "src/core/AppState.h", "src/core/ActivityTimeline.cpp"]),
            ws("Navigation Coverage", "Build coverage around large-board travel, search, and synchronized panels.", ["minimap jump test", "search-result focus test", "saved view round-trip"], ["tests/unit", "src/canvas/BenchmarkModel.cpp", "src/core/CrossModuleTestSuite.cpp"]),
        ],
    },
    {
        "id": 8,
        "title": "Templates_Automation_AI_Assistance_And_Creation_Flows",
        "objective": "Use templates, automation, AI, import/export workflows, and extensibility to accelerate content creation and reduce repetitive board labor.",
        "common_refs": [
            "src/canvas/BoardTemplate.cpp",
            "src/canvas/CanvasTemplateEngine.cpp",
            "src/core/AutomationRule.cpp",
            "src/core/AIService.cpp",
            "src/core/AIDocumentGenerator.cpp",
            "src/core/ExportService.cpp",
        ],
        "workstreams": [
            ws("Board Templates", "Make new-board creation start from strong, role-specific defaults.", ["brainstorm starter", "journey map starter", "team retro starter"], ["src/canvas/BoardTemplate.h", "src/canvas/OnboardingModel.cpp", "src/ui/DialogStackManager.cpp"]),
            ws("Object Templates", "Allow reusable object clusters and design modules inside the canvas.", ["team card cluster", "feature planning frame", "service blueprint block"], ["src/canvas/CanvasTemplateEngine.h", "src/canvas/CanvasObjectFactory.h", "src/canvas/FrameObject.cpp"]),
            ws("Advanced Style Presets", "Bring team-wide styling systems to the board layer.", ["brand sticky presets", "diagram theme presets", "presentation frame styles"], ["src/canvas/CanvasStylePresetService.h", "src/core/ConfigProfile.cpp", "src/ui/DesignTokenRegistry.cpp"]),
            ws("Automation Rules", "Automate low-risk repetitive actions so boards stay organized as they grow.", ["auto-tag new sticky", "snap comments into review frame", "assign default owner"], ["src/core/AutomationRule.h", "src/core/AutomationCommandProvider.cpp", "src/canvas/CanvasObjectLifecycleEvents.cpp"]),
            ws("AI Board Generation", "Generate starting structures from prompts without hiding the editable model.", ["create workshop board from prompt", "generate flowchart skeleton", "build comparison matrix"], ["src/core/AIService.h", "src/core/AIDocumentGenerator.h", "src/core/AITypes.h"]),
            ws("AI Cleanup And Refinement", "Use AI to normalize or restructure messy boards after a session.", ["cluster related notes", "rewrite sticky titles", "convert raw notes into sections"], ["src/core/AIWritingAssistant.cpp", "src/canvas/AutoLayoutModel.h", "src/canvas/CanvasSearchModel.cpp"]),
            ws("AI Summaries And Explanations", "Turn dense boards into digestible summaries and narratives.", ["summarize selected frame", "explain diagram path", "write action-item recap"], ["src/core/AIWritingAssistant.h", "src/core/AIConversationHistory.h", "src/canvas/SelectionManager.cpp"]),
            ws("AI Tags And Links", "Suggest metadata and board connections to improve findability.", ["suggest tags for cluster", "link related board", "identify duplicate notes"], ["src/core/AITagSuggester.cpp", "src/core/AILinkSuggester.cpp", "src/canvas/CrossBoardLink.cpp"]),
            ws("Batch Operations", "Bundle repetitive edits into scalable multi-object workflows.", ["bulk rename selected notes", "batch recolor lanes", "replace icon set"], ["src/ui/BulkActionController.cpp", "src/canvas/CanvasInspector.cpp", "src/core/CommandRegistry.h"]),
            ws("Markdown To Canvas", "Bridge document and board workflows with meaningful imports.", ["convert bullet list to notes", "turn headings into sections", "import checklist as kanban"], ["src/core/MarkdownImporter.cpp", "src/core/BlockService.cpp", "src/canvas/CanvasObjectFactory.cpp"]),
            ws("CSV And Database Imports", "Convert tabular data into useful board structures.", ["csv to table", "database rows to cards", "timeline import"], ["src/core/av/AVImportExport.cpp", "src/core/AttributeService.cpp", "src/canvas/KanbanObjects.cpp"]),
            ws("Export Workflows", "Turn working boards into publishable artifacts with predictable quality.", ["export frame as PNG", "batch export sections", "HTML export with links"], ["src/canvas/CanvasExportService.cpp", "src/core/BatchExportEngine.cpp", "src/core/HtmlExporter.cpp"]),
            ws("Reusable Components", "Support repeated insertion of high-value modules without external copy-paste hacks.", ["saved widget cluster", "meeting template block", "diagram legend component"], ["src/canvas/CustomObjectTypeRegistry.cpp", "src/core/BuiltInPlugins.h", "src/canvas/AppWidgetObject.cpp"]),
            ws("Plugin Hooks", "Make canvas automation and creation flows extensible through stable integration seams.", ["plugin adds canvas object", "plugin contributes template", "plugin registers slash action"], ["src/core/ContributionValidator.cpp", "src/core/PluginManager.cpp", "src/core/ContextKeyService.cpp"]),
            ws("Command Macros", "Allow reusable multi-step board actions for power users and team admins.", ["prepare review board", "normalize sticky wall", "publish selected frame"], ["src/core/AutomationCommandProvider.h", "src/core/Command.cpp", "src/core/CommandExecutionLog.h"]),
            ws("Quick Insert", "Make insertion faster through slash menus and context-aware quick-add surfaces.", ["slash create note", "quick insert connector label", "insert recent asset"], ["src/ui/CommandPalette.cpp", "src/canvas/CanvasContextMenu.h", "src/ui/FloatingToolbarManager.cpp"]),
            ws("Smart Defaults", "Bias the UI toward likely next actions without becoming opaque.", ["remember last note color", "default connector style by diagram", "auto-open text editor after create"], ["src/core/AppState.cpp", "src/core/Config.cpp", "src/canvas/CanvasToolStateMachine.h"]),
            ws("Profile And Preference Sync", "Carry team and personal defaults across boards and devices where appropriate.", ["sync style presets", "workspace template pack", "shared board defaults"], ["src/core/ConfigProfile.h", "src/core/CloudSyncService.h", "src/canvas/CanvasThemeModel.cpp"]),
            ws("Onboarding Programs", "Use checklists and progressive disclosure to shorten time-to-value for new creators.", ["template recommendations", "first board checklist", "coachmark sequencing"], ["src/canvas/OnboardingModel.h", "src/ui/FirstRunWizard.cpp", "src/ui/DialogModel.h"]),
            ws("Agent Ready Scaffolds", "Document creation flows so AI coding agents can implement them incrementally.", ["task scaffolds per template", "acceptance checklist blocks", "reference mapping"], ["docs", "src/canvas/CanvasTemplateEngine.cpp", "src/core/CrossModuleTestSuite.h"]),
        ],
    },
    {
        "id": 9,
        "title": "Accessibility_Performance_Reliability_And_Operational_Quality",
        "objective": "Make the canvas durable, inclusive, observable, and performant enough to support large boards and diverse input methods without regressions.",
        "common_refs": [
            "src/canvas/CanvasAccessibility.cpp",
            "src/canvas/CanvasPerformanceMonitor.cpp",
            "src/canvas/IntegrityModel.cpp",
            "src/ui/AccessibilitySemanticsController.cpp",
            "src/core/AccessibilityManager.cpp",
            "src/ui/ErrorRecoveryModel.cpp",
        ],
        "workstreams": [
            ws("Keyboard Only Canvas", "Ensure the core board can be navigated and edited without pointer input.", ["tab through objects", "keyboard move and resize", "open context actions from keyboard"], ["src/canvas/KeyboardCommandModel.h", "src/ui/FocusManager.cpp", "src/canvas/SelectionManager.cpp"]),
            ws("Screen Reader Semantics", "Expose objects, state, and actions with useful spoken semantics.", ["announce selected object", "read comment count", "describe board region"], ["src/canvas/AccessibilityModel.cpp", "src/ui/AccessibilitySemanticsController.h", "src/core/AccessibilityAudit.cpp"]),
            ws("Contrast And Color Blindness", "Preserve legibility and meaning across a wider range of visual abilities.", ["contrast-safe selection", "color blind preview", "pattern fallback for status"], ["src/core/ContrastRatioValidator.cpp", "src/ui/ColorBlindnessPreviewOverlay.cpp", "src/core/ColorBlindnessSupport.cpp"]),
            ws("Focus And Announcements", "Make focus transitions and live region events understandable during active editing.", ["announce mode change", "visible focus ring", "read undo result"], ["src/ui/FocusRingRenderer.h", "src/canvas/CanvasAccessibility.h", "src/ui/NotificationCenter.cpp"]),
            ws("Touch And Pen Parity", "Treat touch and stylus as first-class creation inputs, not degraded mouse emulation.", ["two-finger pan", "pen button erase", "touch selection handles"], ["src/canvas/PointerEventRouter.h", "src/canvas/CanvasInputManager.cpp", "src/ui/DpiScalingController.h"]),
            ws("Performance Budgets", "Set and enforce objective performance targets for common board operations.", ["16ms frame budget", "search latency budget", "drag latency budget"], ["src/canvas/BenchmarkModel.cpp", "src/ui/EditorPerformanceBudget.h", "src/canvas/CanvasPerformanceMonitor.h"]),
            ws("Virtualization", "Render only what matters on very large boards without breaking interaction fidelity.", ["offscreen culling", "deferred panel updates", "virtualized outline"], ["src/canvas/Quadtree.h", "src/canvas/LargeBoardModel.cpp", "src/ui/CanvasPanel.cpp"]),
            ws("Crash Recovery", "Detect bad states early and help users recover without data loss.", ["recover last autosave", "disable crashing feature flag", "surface corruption warning"], ["src/core/CrashReporter.h", "src/ui/ErrorRecoveryModel.cpp", "src/core/SafeMode.h"]),
            ws("Data Integrity", "Continuously validate board state so corruption is caught before export or sync.", ["missing asset detection", "orphaned connector check", "invalid frame membership"], ["src/canvas/IntegrityModel.h", "src/canvas/AuditModel.cpp", "src/canvas/BoardSerializer.cpp"]),
            ws("Accessibility Tooling", "Provide tooling that makes regressions visible during development.", ["automated semantics audit", "contrast checker", "keyboard traversal report"], ["src/core/AccessibilityAudit.h", "src/core/AccessibilityCommandProvider.cpp", "src/ui/ControlSnapshotTester.cpp"]),
            ws("Safe Degradation", "Design graceful fallbacks for slow machines, failed embeds, and broken plugins.", ["fallback media placeholder", "reduced motion mode", "disable live shadows"], ["src/core/AdaptiveThrottle.h", "src/ui/ControlVisibilityPrefs.cpp", "src/core/ExtensionHostRecovery.cpp"]),
            ws("Observability", "Instrument the canvas well enough to explain regressions and field issues.", ["render timing log", "tool failure metric", "autosave success event"], ["src/core/CommandExecutionLog.cpp", "src/core/BuildLogManager.cpp", "src/canvas/AuditModel.h"]),
            ws("Feature Flags", "Roll out high-risk canvas upgrades safely across teams and builds.", ["flag new connector routing", "beta-only pen engine", "kill switch for embed preview"], ["src/core/FeatureRegistry.cpp", "src/core/FeatureToggledEvent.h", "src/core/ContextKeyService.h"]),
            ws("Snapshot And Harnesses", "Expand deterministic tooling for visual and behavioral regressions.", ["canvas snapshot test", "serialized board fixture", "input replay harness"], ["src/ui/ControlSnapshotTester.h", "src/canvas/BoardSerializerModel.h", "src/core/CrossModuleTestSuite.cpp"]),
            ws("Unit And Integration Gaps", "Close the most expensive correctness gaps before broad UX expansion.", ["selection edge-case test", "undo history integration test", "board lifecycle contract test"], ["tests/unit", "src/canvas/CanvasCommands.cpp", "src/core/CrossModuleTestSuite.h"]),
            ws("End To End Determinism", "Build stable top-level workflows that tolerate timing variance and platform differences.", ["new board smoke", "draw and export smoke", "collaboration reconnect smoke"], ["scripts/run_tests.sh", "src/ui/CanvasWorkspacePanel.cpp", "src/canvas/BenchmarkModel.h"]),
            ws("Security And Privacy", "Audit the canvas surface for unsafe ingest, embeds, and collaboration leaks.", ["sanitize pasted HTML", "redact private metadata", "restrict dangerous URLs"], ["src/core/ClipboardSanitizer.h", "src/core/DataRedactionEngine.cpp", "src/core/ContentSecurityPolicy.h"]),
            ws("Localization And IME", "Support multilingual authoring, bidirectional text, and input method editors.", ["RTL text box", "IME candidate flow", "localized command labels"], ["src/core/LocalizationManager.cpp", "src/canvas/CanvasTextEditor.cpp", "src/ui/CommandPaletteModel.h"]),
            ws("Compliance Checklists", "Turn accessibility and reliability standards into repeatable release checks.", ["a11y exit criteria", "performance release gate", "embed security checklist"], ["docs", "src/core/AccessibilityAudit.cpp", "src/core/CompilerConfigValidator.cpp"]),
            ws("Operational Dashboards", "Expose board health signals to engineering and support teams.", ["slow board cohorts", "crash-free rate", "failed export trend"], ["src/core/BuildService.cpp", "src/core/ActivityFeed.cpp", "src/canvas/BenchmarkModel.cpp"]),
        ],
    },
    {
        "id": 10,
        "title": "Polish_Parity_Validation_And_Platform_Completion",
        "objective": "Finish the Miro-parity program with audit, polish, platform fit, release gates, and long-term backlog shaping so the canvas can ship coherently.",
        "common_refs": [
            "src/canvas/PolishModel.cpp",
            "src/ui/AnimationDriver.cpp",
            "src/core/ConfigMigration.cpp",
            "src/core/CommandExecutionLog.cpp",
            "scripts/run_tests.sh",
            "scripts/benchmark_compare.py",
        ],
        "workstreams": [
            ws("Parity Audit", "Systematically compare the shipped canvas against target competitor behaviors and close the highest-value gaps.", ["miro feature matrix", "gap severity scoring", "cut list for release"], ["docs/v9_docs/Phase_11__Canvas_Workbench_Shell.md", "docs/v9_docs/Phase_12__Canvas_Advanced_Objects_And_Diagrams.md", "src/canvas/AuditModel.cpp"]),
            ws("Control Polish", "Refine control density, labels, placement, and affordances so the UI reads as intentional.", ["clean toolbar spacing", "button grouping", "less modal friction"], ["src/ui/DesignSystemConstants.h", "src/ui/ComponentVariants.cpp", "src/ui/CustomChrome.cpp"]),
            ws("Motion And Microinteraction", "Use motion sparingly to improve clarity and perceived responsiveness.", ["toolbar reveal", "selection settle animation", "export progress transition"], ["src/ui/AnimationDriver.h", "src/ui/ControlMotionTokens.h", "src/ui/ControlAnimator.h"]),
            ws("Platform Conventions", "Make the canvas feel native across macOS, Windows, and Linux input and chrome conventions.", ["mac trackpad gestures", "windows context menu expectation", "linux window behavior"], ["src/platform/MacPlatformIntegration.cpp", "src/platform/WindowsPlatformIntegration.cpp", "src/platform/LinuxPlatformIntegration.cpp"]),
            ws("Settings Migration", "Ensure new canvas capabilities land without breaking existing user preferences.", ["migrate snap settings", "default new token values", "repair missing board prefs"], ["src/core/ConfigMigration.h", "src/core/ConfigProfile.h", "src/core/ConfigAuditTrail.h"]),
            ws("Documentation", "Ship human-facing docs that match the implemented workflows and shortcuts.", ["canvas getting started", "diagram guide", "collaboration facilitation guide"], ["docs", "src/ui/ChangelogPanel.cpp", "src/core/CommandRegistry.cpp"]),
            ws("Benchmarks And Baselines", "Track performance progress and prevent slow regressions from hiding in polish work.", ["baseline render metrics", "drag latency benchmark", "search latency benchmark"], ["src/canvas/BenchmarkModel.h", "scripts/benchmark_update_baseline.sh", "scripts/benchmark_compare.py"]),
            ws("Release Gates", "Define objective exit criteria before promoting the new canvas UX broadly.", ["all critical e2e pass", "a11y checklist complete", "no P0 parity gaps"], ["scripts/smoke_test.sh", "scripts/run_tests.sh", "scripts/run_chaos_tests.sh"]),
            ws("Beta Feedback Loops", "Make user feedback actionable and tied to board states that engineers can replay.", ["feedback with board snapshot", "session tag for beta features", "in-app issue prompt"], ["src/core/NotificationService.cpp", "src/core/CommandExecutionLog.h", "src/ui/DialogModel.cpp"]),
            ws("Extension Ecosystem", "Give plugins a coherent way to align with the improved canvas UX.", ["extension contributes tool", "extension adds inspector panel", "extension reacts to selection"], ["src/core/PluginManager.h", "src/core/ContributionValidator.h", "src/core/ContextKeyService.cpp"]),
            ws("Advanced Drawing Polish", "Finish rough edges in pen, shape, and text authoring so the surface feels production-grade.", ["stroke smoothing tune", "text editing handoff polish", "shape preview polish"], ["src/canvas/DrawTool.h", "src/canvas/FreehandPathRenderer.cpp", "src/canvas/CanvasTextEditor.h"]),
            ws("Advanced Layout Polish", "Refine transform and alignment behavior under complex board conditions.", ["smarter guide filtering", "less jumpy snap", "cleaner group resize"], ["src/canvas/AlignmentGuides.cpp", "src/canvas/SnapEngine.h", "src/canvas/GroupingService.cpp"]),
            ws("Advanced Collaboration Polish", "Resolve edge frictions in multi-user sessions before broad release.", ["less noisy presence", "faster comment resolve", "clean presenter follow exit"], ["src/core/CanvasCollabService.cpp", "src/canvas/FollowModeController.cpp", "src/ui/CanvasFacilitationPanel.h"]),
            ws("Advanced Navigation Polish", "Make large-board travel feel calm, not frantic.", ["less minimap jitter", "search focus settle", "saved view transition"], ["src/canvas/MinimapPanel.h", "src/canvas/BoardNavigator.cpp", "src/ui/BreadcrumbBar.h"]),
            ws("Advanced Export Polish", "Verify the last mile from board editing to artifact production.", ["crisp PNG export", "print-safe PDF", "shareable HTML bundle"], ["src/canvas/CanvasExportService.h", "src/core/PdfExporter.cpp", "src/core/HtmlExporter.h"]),
            ws("Enterprise And Admin Fit", "Cover governance and admin concerns needed for larger deployments.", ["policy-controlled embeds", "admin feature flags", "audit-ready activity"], ["src/core/RuntimePolicy.h", "src/core/SecurityAuditLog.cpp", "src/core/FeatureRegistry.h"]),
            ws("Quality Backlog Triage", "Turn defects found during rollout into a prioritized, bounded program.", ["severity rubric", "owner assignment", "cut vs defer call"], ["src/canvas/AuditModel.h", "src/core/ActivityFeed.h", "src/ui/ChangelogPanel.h"]),
            ws("Adoption Metrics", "Measure whether the upgraded canvas is actually being used more effectively.", ["tool adoption", "board completion rate", "export usage after session"], ["src/core/ActivityTimeline.cpp", "src/core/AppState.cpp", "src/core/CommandExecutionLog.cpp"]),
            ws("Support Playbooks", "Prepare engineering and support for the issues users will actually report.", ["recovery steps for broken board", "embed troubleshooting guide", "slow board diagnosis"], ["docs", "src/ui/ErrorRecoveryModel.h", "src/core/CrashReporter.h"]),
            ws("Architecture Follow Ups", "Capture the longer-term technical debt and platform investments revealed by the parity push.", ["rendering abstraction backlog", "input model cleanup", "plugin surface simplification"], ["src/canvas/CanvasWorkbench.h", "src/core/CrossModuleTestSuite.cpp", "src/ui/DesignSystemContext.h"]),
        ],
    },
]


TASK_ASPECTS = [
    {
        "label": "UX Parity Audit",
        "definition": "Define the implementation contract for {ws} by auditing current behavior, parity gaps, and release-critical outcomes.",
        "description": "Review the current implementation around {focus}. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.",
        "context": "This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.",
    },
    {
        "label": "Domain Model And State Contract",
        "definition": "Extend or normalize the internal state and model contracts needed to support {ws}.",
        "description": "Audit the relevant state holders, board model seams, and service boundaries around {focus}. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.",
        "context": "MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.",
    },
    {
        "label": "Commands Events And Context Keys",
        "definition": "Wire the command, event, and context-key vocabulary needed for {ws}.",
        "description": "Add or revise the commands, events, shortcuts, and context key checks that make {focus} accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.",
        "context": "Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.",
    },
    {
        "label": "Workspace Surface And Controls",
        "definition": "Implement or refine the visible controls that expose {ws} in the main canvas workspace.",
        "description": "Build the primary UI surfaces for {focus}, including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.",
        "context": "This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.",
    },
    {
        "label": "Pointer Interaction Flow",
        "definition": "Implement the mouse, touch, and stylus interaction flow for {ws}.",
        "description": "Define pointer-down, drag, hover, cancel, and completion behavior for {focus}. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.",
        "context": "Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.",
    },
    {
        "label": "Keyboard Gesture And Shortcut Flow",
        "definition": "Support keyboard-first access and modifier behavior for {ws}.",
        "description": "Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for {focus}. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.",
        "context": "Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.",
    },
    {
        "label": "Visual Feedback And Rendering Polish",
        "definition": "Add the rendering, motion, and feedback treatments that make {ws} legible in use.",
        "description": "Implement the visible feedback for {focus}: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.",
        "context": "Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.",
    },
    {
        "label": "Persistence Preferences And Serialization",
        "definition": "Persist the board state, user settings, and serialization details required by {ws}.",
        "description": "Identify what parts of {focus} belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.",
        "context": "Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.",
    },
    {
        "label": "Safeguards Telemetry And Recovery",
        "definition": "Add instrumentation, safeguards, and failure handling for {ws}.",
        "description": "Instrument {focus} with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.",
        "context": "AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.",
    },
    {
        "label": "Tests Documentation And Rollout Gate",
        "definition": "Add the tests, documentation, and exit criteria needed to ship {ws}.",
        "description": "Create or expand unit, integration, snapshot, and end-to-end coverage for {focus}. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.",
        "context": "This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.",
    },
]


def slug(title: str) -> str:
    return re.sub(r"[^A-Za-z0-9]+", "_", title).strip("_")


def full_path(path: str) -> str:
    if path == "docs" or path == "tests/unit":
        return str(ROOT / path)
    return str(ROOT / path)


def build_phase_doc(phase: dict) -> str:
    phase_id = phase["id"]
    title_text = phase["title"].replace("_", " ")
    lines: list[str] = []
    lines.append(f"# Phase {phase_id:02d}: {title_text}")
    lines.append("")
    lines.append("## Objective")
    lines.append(phase["objective"])
    lines.append("")
    lines.append("## AI Agent Execution Rules")
    lines.append("- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.")
    lines.append("- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.")
    lines.append("- Every implementation task should finish with tests or a documented testing gap.")
    lines.append("- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.")
    lines.append("- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.")
    lines.append("")
    lines.append("## Workstreams")
    for idx, workstream in enumerate(phase["workstreams"], start=1):
        lines.append(f"- W{idx:02d} {workstream['title']}: {workstream['focus']}")
    lines.append("")
    task_number = 1
    for ws_idx, workstream in enumerate(phase["workstreams"], start=1):
        lines.append(f"## Workstream {ws_idx:02d}: {workstream['title']}")
        lines.append(workstream["focus"])
        lines.append("")
        refs = list(dict.fromkeys(phase["common_refs"] + workstream["refs"]))
        example_text = "; ".join(workstream["examples"])
        for aspect in TASK_ASPECTS:
            task_id = f"P{phase_id:02d}-T{task_number:03d}"
            lines.append(f"### {task_id}: {workstream['title']} {aspect['label']}")
            lines.append(f"**Task Title:** {workstream['title']} {aspect['label']}")
            lines.append(f"**Definition:** {aspect['definition'].format(ws=workstream['title'].lower())}")
            lines.append(f"**Description:** {aspect['description'].format(focus=workstream['focus'].lower())}")
            lines.append(f"**Examples:** {example_text}.")
            lines.append(f"**Context:** {aspect['context']}")
            lines.append("**References:**")
            for ref in refs:
                lines.append(f"- {full_path(ref)}")
            lines.append("")
            task_number += 1
    lines.append("## Phase Exit Criteria")
    lines.append("- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.")
    lines.append("- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.")
    lines.append("- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.")
    lines.append("- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.")
    lines.append("")
    return "\n".join(lines)


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for phase in PHASES:
        filename = OUT_DIR / f"Phase_{phase['id']:02d}__{slug(phase['title'])}.md"
        filename.write_text(build_phase_doc(phase), encoding="utf-8")


if __name__ == "__main__":
    main()
