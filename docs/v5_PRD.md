1. Infinite Canvas

Definition: A virtually limitless 2D workspace where content can be placed without spatial restrictions.
How It Works: Users drag, drop, and create content anywhere; canvas expands dynamically.
How Miro Achieves It: Uses a zoomable vector canvas with sections loaded on demand (tiling for performance).
UX/UI: Zoom bar + scroll; smooth pinch-zoom; blank grid background; auto-expanding scroll boundaries.

2. Sticky Notes

Definition: Digital notes that can be placed, colored, sized, and edited.
How It Works: Insert → choose sticky, type, drag, duplicate.
How Miro Achieves It: HTML Canvas elements with drag & inline edit; persistent objects.
UX/UI: Color palette; quick double-click to edit; drag handles; sticky previews.

3. Text Boxes

Definition: Freeform text elements.
How It Works: Click → type; supports formatting (bold, lists).
How Miro Achieves It: Rich-text rendering engine within canvas.
UX/UI: Inline toolbar for formatting, drag handles.

4. Shapes Library

Definition: Pre-built geometric shapes.
How It Works: Drag shapes from toolbar; resize/rotate.
How Miro Achieves It: Vector shape library with dynamic SVG rendering.
UX/UI: Shape palette + contextual toolbar.

5. Connectors & Lines

Definition: Lines that visually link elements.
How It Works: Drag from connection point → attach to target.
How Miro Achieves It: Auto-layout algorithm for line routing.
UX/UI: Snapping guides; customizable line styles.

6. Comments

Definition: Annotations tied to specific points.
How It Works: Click comment icon → type → assign.
How Miro Achieves It: Sidebar thread system with anchors on canvas.
UX/UI: Expand/collapse threads.

10. Templates

Definition: Pre-built board setups.
How It Works: Choose category → preview → apply.
How Miro Achieves It: Template library with meta tags.
UX/UI: Grid/list browse with filter.

11. Frames

Definition: Defined board areas used as slides/pages.
How It Works: Draw frame → add content → navigate.
How Miro Achieves It: Logical container objects, exportable as pages.
UX/UI: Frame outlines; breadcrumb navigator.

12. Presentation Mode

Definition: Linear slide view of frames.
How It Works: Enter mode → arrows navigate.
How Miro Achieves It: Extract frame content; render sequentially.
UX/UI: Full-screen; minimal controls.

13. Freehand Drawing

Definition: Pen/brush tool.
How It Works: Draw with pointer; supports smoothing.
How Miro Achieves It: Path capture + vector smoothing.
UX/UI: Pen controls; color/size selector.

14. Smart Drawing Recognition

Definition: Converts scribbles into clean shapes.
How It Works: Scribble detection → shape replace.
How Miro Achieves It: Recognition ML + vector mapping.
UX/UI: Auto prompt to convert.

15. Upload Files

Definition: Add documents/images.
How It Works: Drag/drop or upload selector.
How Miro Achieves It: Cloud storage integration.
UX/UI: Upload dialog with previews.

16. PDF Import/Pages

Definition: Import multi-page PDFs as frames.
How It Works: Parses pages → generates boards.
How Miro Achieves It: PDF rendering service backend.
UX/UI: Page navigator on import.

17. Image Editing

Definition: Crop, filter, arrange images.
How It Works: Select image → toolbar editing.
How Miro Achieves It: Canvas bitmap adjustments.
UX/UI: Floating image toolbar.

18. Table Creation

Definition: Create data grid tables.
How It Works: Insert → define rows/columns.
How Miro Achieves It: Structured object with text cells.
UX/UI: Cell option bar; context formatting.

20. Version History

Definition: Board change log.
How It Works: Snapshot diff → browse.
How Miro Achieves It: Time-series storage.
UX/UI: Timeline slider; compare/restore.

23. Export (PNG/PDF/SVG)

Definition: Output board or frames.
How It Works: Choose area → export format.
How Miro Achieves It: Render engine backend.
UX/UI: Export dialog with settings.

30. Templates API

Definition: Programmatic template generation.
How It Works: REST API for template operations.
How Miro Achieves It: API endpoints with permissions.
UX/UI: Developer portal docs.

34. Board Templates (Team Library)

Definition: Team-scoped template sets.
How It Works: Admin uploads templates.
How Miro Achieves It: Team metadata.
UX/UI: Library pane.

35. Outline Panel

Definition: Navigation through board structure.
How It Works: Auto detects frames/headings.
How Miro Achieves It: Content parser.
UX/UI: Left-panel nav.

36. Search

Definition: Search board content.
How It Works: Query text across objects.
How Miro Achieves It: Indexed board metadata.
UX/UI: Search bar; highlight results.

37. Tags

Definition: Labels that denote group/type.
How It Works: Apply tag to objects.
How Miro Achieves It: Metadata store.
UX/UI: Tag badges.

38. Color Palettes

Definition: Predefined board colors.
How It Works: Pick palette for consistency.
How Miro Achieves It: Swatch library.
UX/UI: Color panel.

39. Sticky Note Grouping

Definition: Combine multiple notes.
How It Works: Select → group.
How Miro Achieves It: Container object.
UX/UI: Bounding box with handles.

40. Advanced Alignment

Definition: Guides for snapping/aligning.
How It Works: Drag → auto snap.
How Miro Achieves It: Geometry/constraint system.
UX/UI: Blue guide lines.

41. Grid & Snap

Definition: Implicit alignment grid.
How It Works: Toggle grid → snap on.
How Miro Achieves It: Rendering grid matrix.
UX/UI: Light grid; toggle button.

42. Hotkeys

Definition: Keyboard shortcuts.
How It Works: Press keys → action.
How Miro Achieves It: Shortcut mapping layer.
UX/UI: Cheat sheet overlay.

43. Bulk Select/Actions

Definition: Multi-select + batch operations.
How It Works: Shift drag; actions toolbar.
How Miro Achieves It: Selection manager.
UX/UI: Selection marquee.

44. Export to CSV

Definition: Export structured data (boards/notes).
How It Works: Generate CSV of objects.
How Miro Achieves It: Backend exporter.
UX/UI: Download modal.


56. Smart Layout

Definition: Auto-arranges objects for visual clarity.
How It Works: Select → apply layout template.
How Miro Achieves It: Constraint-based layout engine.
UX/UI: Layout options panel.

57. Object Locking

Definition: Prevent editing/movement.
How It Works: Lock toggle on object.
How Miro Achieves It: Object state flag enforced in edit layer.
UX/UI: Lock icon overlay.

58. Object Layering (Z-Index)

Definition: Arrange front/back ordering.
How It Works: Bring forward/back.
How Miro Achieves It: Z-index stack model.
UX/UI: Contextual layering menu.

59. Board Sections

Definition: Semantic grouping areas beyond frames.
How It Works: Create section → drag objects inside.
How Miro Achieves It: Metadata grouping containers.
UX/UI: Subtle background tint + label header.

60. Board Map / Mini-Map

Definition: Visual overview navigator.
How It Works: Mini-map preview of full canvas.
How Miro Achieves It: Scaled canvas thumbnail rendering.
UX/UI: Floating minimap bottom corner.

65. Reactions (Emoji)

Definition: Quick visual reactions.
How It Works: Click reaction → emoji appears.
How Miro Achieves It: Lightweight event broadcast.
UX/UI: Floating animated emojis.

66. Embedded Video

Definition: Insert playable videos.
How It Works: Paste link → auto embed.
How Miro Achieves It: oEmbed integration.
UX/UI: Inline video player with play overlay.

67. Live Data Widgets

Definition: Boards reflect external data updates.
How It Works: Connect API → auto-refresh.
How Miro Achieves It: Webhooks + polling service.
UX/UI: Widget card with refresh indicator.

68. Diagram Libraries (UML, BPMN)

Definition: Structured modeling components.
How It Works: Select library → drag elements.
How Miro Achieves It: Predefined shape sets + connectors.
UX/UI: Expandable diagram tool drawer.

69. Mind Map Tool

Definition: Hierarchical idea mapping.
How It Works: Central node → branch with tab.
How Miro Achieves It: Tree data structure rendering.
UX/UI: Auto-aligned radial layout.

70. Kanban Board

Definition: Task columns with cards.
How It Works: Create columns → drag cards.
How Miro Achieves It: Card objects + state metadata.
UX/UI: Swimlane-style layout.

71. Table Sorting & Filtering

Definition: Manipulate tabular data.
How It Works: Sort/filter rows.
How Miro Achieves It: Client-side table logic.
UX/UI: Column header dropdown menus.

72. Custom Icons & Emoji Packs

Definition: Extend visual language.
How It Works: Upload icon sets.
How Miro Achieves It: SVG library injection.
UX/UI: Custom icon picker.

79. Bulk Export

Definition: Export multiple boards.
How It Works: Select boards → export.
How Miro Achieves It: Batch rendering backend jobs.
UX/UI: Bulk action toolbar.

81. Cross-Board Linking

Definition: Link one board to another.
How It Works: Hyperlink object → open target.
How Miro Achieves It: Deep link routing.
UX/UI: Hover link preview.

82. Bookmark Cards

Definition: Rich link previews.
How It Works: Paste URL → auto preview.
How Miro Achieves It: Metadata scraping service.
UX/UI: Card with image/title.

83. Dark Mode

Definition: Alternative low-light UI.
How It Works: Toggle theme.
How Miro Achieves It: CSS theme switch.
UX/UI: Dark background; contrast adjustments.

84. Custom Keyboard Shortcuts

Definition: User-defined hotkeys.
How It Works: Assign actions to keys.
How Miro Achieves It: Shortcut mapping config.
UX/UI: Shortcut settings page.

85. Board Duplication with History

Definition: Clone including metadata.
How It Works: Duplicate command.
How Miro Achieves It: Deep object copy with references.
UX/UI: Duplicate confirmation dialog.

86. Focus Mode

Definition: Hide UI chrome.
How It Works: Enter distraction-free mode.
How Miro Achieves It: Minimal UI overlay.
UX/UI: Full canvas immersive view.

88. Board Archiving

Definition: Deactivate board without deletion.
How It Works: Archive state change.
How Miro Achieves It: Status flag + restricted access.
UX/UI: Archived badge.

89. API Webhooks

Definition: Trigger external systems.
How It Works: Event → webhook call.
How Miro Achieves It: Event dispatcher.
UX/UI: Developer webhook dashboard.

92. Smart Sticky Aggregation

Definition: Combine duplicates.
How It Works: Detect duplicates → suggest merge.
How Miro Achieves It: Text similarity detection.
UX/UI: Merge suggestion overlay.

95. Multi-Select Transform Tools

Definition: Resize/rotate multiple items at once.
How It Works: Select group → transform.
How Miro Achieves It: Unified bounding box transformation.
UX/UI: Transform handles.

96. Object Metadata

Definition: Add custom properties to objects.
How It Works: Edit metadata panel.
How Miro Achieves It: Object schema extension.
UX/UI: Side inspector panel.

99. Board Performance Optimization (Lazy Loading)

Definition: Maintain speed on massive boards.
How It Works: Load only visible objects.
How Miro Achieves It: Canvas tiling + virtualization.
UX/UI: Seamless pan/zoom performance.


