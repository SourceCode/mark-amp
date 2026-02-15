# Phase 35 -- Board Templates System

## Objective

Implement a template library for pre-built board setups: users can browse templates by category, preview them, and apply them to create new boards. Includes built-in templates (Sprint Planning, Retrospective, User Story Map, SWOT Analysis, etc.) and the ability to save any board as a template.

## Prerequisites

- Phase 05 (Board, BoardSerializer)
- Phase 07-14 (Various object types for template content)

## Feature References (PRD)

- PRD #10: Templates
- PRD #34: Board Templates (Team Library)

## Data Structures to Implement

### File: `src/canvas/BoardTemplate.h`

```cpp
#pragma once

#include "Board.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

struct TemplateInfo
{
    std::string id;
    std::string name;
    std::string description;
    std::string category;  // "Agile", "Design", "Strategy", "Education", "Custom"
    std::string author;
    std::string thumbnail_path;
    std::filesystem::path template_path;
    bool is_builtin{false};
};

class BoardTemplateLibrary
{
public:
    /// Load built-in templates from the resources directory.
    auto load_builtins(const std::filesystem::path& resources_dir) -> void;

    /// Load user-created templates from the config directory.
    auto load_user_templates(const std::filesystem::path& user_dir) -> void;

    /// Get all templates in a category.
    [[nodiscard]] auto templates_in_category(const std::string& category) const
        -> std::vector<const TemplateInfo*>;

    /// Get all categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Search templates by name.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<const TemplateInfo*>;

    /// Apply a template: creates a new Board from the template.
    [[nodiscard]] auto apply_template(const std::string& template_id) const
        -> std::unique_ptr<Board>;

    /// Save a board as a user template.
    auto save_as_template(const Board& board, const TemplateInfo& info,
                           const std::filesystem::path& user_dir) -> bool;

    /// Get all templates.
    [[nodiscard]] auto all_templates() const -> const std::vector<TemplateInfo>&;

private:
    std::vector<TemplateInfo> templates_;
    BoardSerializer serializer_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/TemplatePanel.h`

```cpp
#pragma once

#include "BoardTemplate.h"

#include <functional>

class wxGraphicsContext;

namespace markamp::canvas
{

class TemplatePanel
{
public:
    using OnApplyTemplate = std::function<void(const std::string& template_id)>;

    auto set_templates(const std::vector<TemplateInfo>& templates) -> void;
    auto set_active_category(const std::string& category) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;
    auto set_on_apply(OnApplyTemplate cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<TemplateInfo> templates_;
    std::string active_category_;
    bool visible_{false};
    OnApplyTemplate on_apply_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `BoardTemplateLibrary::load_builtins()` -- Scan resources/templates/ for .markboard files. Parse metadata from the file header.

2. `BoardTemplateLibrary::apply_template()` -- Load the template .markboard file, clone the board, assign a new ID.

3. `BoardTemplateLibrary::save_as_template()` -- Serialize the board to the user templates directory with metadata.

4. Built-in templates to include: Sprint Planning (kanban columns), Retrospective (sections), SWOT Analysis (4 quadrant frames), User Story Map (rows and columns), Mind Map Brainstorm, Project Timeline, Flowchart, Wireframe Grid.

5. `TemplatePanel::render()` -- Grid of template cards with thumbnails, names, and categories. Category filter tabs at top.

## Test Cases

File: `tests/unit/test_board_templates.cpp`

1. **Load builtins** -- Load from test resources, verify templates found.
2. **Categories** -- Verify category list.
3. **Search** -- Search "sprint", verify matches.
4. **Apply template** -- Apply, verify new board created with objects.
5. **Save as template** -- Save board, load user templates, verify found.
6. **Category filter** -- Filter by "Agile", verify only agile templates.
7. **Template info** -- Verify name, description, author on a template.

## Acceptance Criteria

- [ ] Template library loads built-in and user templates
- [ ] Templates organized by category with search
- [ ] Apply creates a new board from the template
- [ ] Save any board as a user template
- [ ] Template panel with grid of preview cards
- [ ] 8+ built-in templates across categories
- [ ] All 7 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/BoardTemplate.h` | Template library |
| CREATE | `src/canvas/BoardTemplate.cpp` | Template management |
| CREATE | `src/canvas/TemplatePanel.h` | Template picker UI |
| CREATE | `src/canvas/TemplatePanel.cpp` | Panel implementation |
| CREATE | `resources/templates/` | Built-in template .markboard files |
| MODIFY | `CMakeLists.txt` | Add new source files, bundle templates |
| CREATE | `tests/unit/test_board_templates.cpp` | 7 Catch2 tests |

## Architecture Notes

- Templates are regular .markboard files with additional metadata in their header. The BoardSerializer handles loading.
- User templates are stored in the app's config directory under templates/.
- The template panel is accessible from the start screen and from a menu item while editing.

## Estimated Complexity

**M** -- Template library, built-in template creation, panel UI, 7 tests.
