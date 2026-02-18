#include "SampleFiles.h"

namespace markamp::core
{

auto get_sample_file_tree() -> FileNode
{
    FileNode root;
    root.id = "root";
    root.name = "My Project";
    root.type = FileNodeType::Folder;
    root.is_open = true;

    // README.md
    FileNode readme;
    readme.id = "readme";
    readme.name = "README.md";
    readme.type = FileNodeType::File;
    readme.content = R"(# MarkAmp v1.0

Welcome to **MarkAmp** — a modern Markdown editor with live Mermaid diagram rendering.

## Features

- 📝 Live Markdown editing with syntax highlighting
- 📊 Real-time Mermaid diagram rendering
- 🎨 Multiple color themes
- 📁 File tree navigation
- ⚡ Split view: Editor + Preview

## Getting Started

1. Select a file from the sidebar
2. Edit markdown in the left pane
3. See rendered output in the right pane
4. Use mermaid code blocks for diagrams

## Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Save | Ctrl+S |
| Toggle Sidebar | Ctrl+B |
| Toggle Preview | Ctrl+P |
| New File | Ctrl+N |
)";

    // Diagrams/ folder
    FileNode diagrams;
    diagrams.id = "diagrams";
    diagrams.name = "Diagrams";
    diagrams.type = FileNodeType::Folder;
    diagrams.is_open = true;

    // architecture.md
    FileNode architecture;
    architecture.id = "architecture";
    architecture.name = "architecture.md";
    architecture.type = FileNodeType::File;
    architecture.content = R"(# Architecture

```mermaid
graph TD
    A[User Interface] --> B[Markdown Parser]
    A --> C[Mermaid Renderer]
    B --> D[HTML Output]
    C --> E[SVG Output]
    D --> F[Preview Panel]
    E --> F
    A --> G[File System]
    G --> H[Local Storage]
```

## Overview

The architecture follows a clean separation between the editor,
parser, and rendering components. The UI layer orchestrates the
flow of data from user input through parsing to visual output.
)";

    // sequence.md
    FileNode sequence;
    sequence.id = "sequence";
    sequence.name = "sequence.md";
    sequence.type = FileNodeType::File;
    sequence.content = R"(# File Open Sequence

```mermaid
sequenceDiagram
    participant User
    participant Sidebar
    participant Editor
    participant Parser
    participant Preview

    User->>Sidebar: Click file
    Sidebar->>Editor: Load content
    Editor->>Parser: Parse markdown
    Parser->>Preview: Render HTML
    Preview->>User: Display output
```

## Description

This sequence diagram shows the flow when a user opens a file
from the sidebar. The content is loaded into the editor, parsed
into HTML, and rendered in the preview panel.
)";

    diagrams.children.push_back(std::move(architecture));
    diagrams.children.push_back(std::move(sequence));

    // TODO.md
    FileNode todo;
    todo.id = "todo";
    todo.name = "TODO.md";
    todo.type = FileNodeType::File;
    todo.content = R"(# TODO

## In Progress
- [ ] Add syntax highlighting for code blocks
- [ ] Implement file save functionality

## Completed
- [x] Basic markdown rendering
- [x] Mermaid diagram support
- [x] File tree navigation
- [x] Theme switching

## Future
- [ ] Export to PDF
- [ ] Collaborative editing
- [ ] Plugin system
- [ ] Vim keybindings
)";

    root.children.push_back(std::move(readme));
    root.children.push_back(std::move(diagrams));
    root.children.push_back(std::move(todo));

    return root;
}

// Phase 35: Extended sample content generators

auto get_sample_markdown_showcase() -> std::string
{
    return R"(# Markdown Showcase

## Headings

# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6

## Emphasis

*italic text*, **bold text**, ***bold italic***, ~~strikethrough~~

## Lists

### Unordered
- Item 1
- Item 2
  - Nested item
  - Another nested item
- Item 3

### Ordered
1. First
2. Second
3. Third

### Task List
- [x] Complete task
- [ ] Incomplete task
- [x] Another done task

## Links & Images

[MarkAmp Website](https://markamp.dev)
[[internal-note]] — Wiki-link
[[internal-note|Custom Display]] — Wiki-link with alias

## Code

Inline: `const x = 42;`

```javascript
function greet(name) {
    return `Hello, ${name}!`;
}
```

```python
def fibonacci(n):
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return a
```

## Tables

| Feature | Status | Priority |
|---------|--------|----------|
| Editor  | ✅ Done | High    |
| Canvas  | ✅ Done | High    |
| Graph   | ✅ Done | Medium  |
| Export  | 🔄 WIP | Low     |

## Blockquotes

> "The best way to predict the future is to invent it."
> — Alan Kay

## Math (LaTeX)

$$E = mc^2$$

## Mermaid Diagram

```mermaid
graph LR
    A[Write] --> B[Preview]
    B --> C[Publish]
    A --> D[Canvas]
    D --> C
```

## Footnotes

Here is a sentence with a footnote[^1].

[^1]: This is the footnote content.

## Horizontal Rule

---

*This document showcases all supported Markdown features.*
)";
}

auto get_sample_canvas_board() -> std::string
{
    return R"({
  "version": 1,
  "objects": [
    {
      "type": "sticky_note",
      "id": "note_1",
      "x": 100, "y": 100, "width": 200, "height": 150,
      "text": "Main Idea",
      "color": "#FFE066"
    },
    {
      "type": "sticky_note",
      "id": "note_2",
      "x": 400, "y": 100, "width": 200, "height": 150,
      "text": "Supporting Detail A",
      "color": "#A3D9FF"
    },
    {
      "type": "sticky_note",
      "id": "note_3",
      "x": 400, "y": 300, "width": 200, "height": 150,
      "text": "Supporting Detail B",
      "color": "#B5EAD7"
    },
    {
      "type": "text_box",
      "id": "text_1",
      "x": 100, "y": 350, "width": 250, "height": 80,
      "text": "# Canvas Demo\nThis board shows diverse object types."
    },
    {
      "type": "connector",
      "id": "conn_1",
      "from": "note_1", "to": "note_2"
    },
    {
      "type": "connector",
      "id": "conn_2",
      "from": "note_1", "to": "note_3"
    }
  ]
})";
}

auto get_sample_notebook() -> std::string
{
    return R"(# Data Analysis Notebook

## Setup

```python
import random
data = [random.randint(1, 100) for _ in range(20)]
print(f"Generated {len(data)} data points")
```

## Analysis

The mean of our dataset:

```python
mean = sum(data) / len(data)
print(f"Mean: {mean:.2f}")
```

## Visualisation

```python
for value in sorted(data):
    bar = "#" * (value // 5)
    print(f"{value:3d} | {bar}")
```

## Conclusion

This notebook demonstrates code cells with Python execution,
intermixed with Markdown prose for explanation.
)";
}

auto get_sample_knowledge_base() -> std::vector<std::pair<std::string, std::string>>
{
    return {
        {"index.md",
         "# Knowledge Base\n\nWelcome to the sample knowledge base.\n\n"
         "- [[getting-started]]\n- [[markdown-guide]]\n- [[canvas-guide]]\n"
         "- [[notebook-guide]]\n- [[graph-guide]]\n- [[shortcuts]]\n"
         "- [[themes]]\n- [[plugins]]\n- [[export]]\n- [[faq]]\n"},
        {"getting-started.md",
         "# Getting Started\n\nSee [[markdown-guide]] for Markdown syntax.\n"
         "See [[shortcuts]] for keyboard shortcuts.\n"},
        {"markdown-guide.md",
         "# Markdown Guide\n\nUse `#` for headings, `**` for bold, `*` for italic.\n"
         "See [[canvas-guide]] for visual editing.\n"},
        {"canvas-guide.md",
         "# Canvas Guide\n\nThe canvas is an infinite 2D workspace.\n"
         "Related: [[getting-started]], [[themes]].\n"},
        {"notebook-guide.md",
         "# Notebook Guide\n\nNotebooks combine prose and code.\n"
         "Related: [[markdown-guide]], [[plugins]].\n"},
        {"graph-guide.md",
         "# Graph Guide\n\nThe knowledge graph shows connections.\n"
         "Related: [[getting-started]], [[canvas-guide]].\n"},
        {"shortcuts.md",
         "# Keyboard Shortcuts\n\n| Key | Action |\n|-----|--------|\n"
         "| ⌘+S | Save |\n| ⌘+P | Quick Open |\n"
         "Related: [[getting-started]].\n"},
        {"themes.md",
         "# Themes\n\nCustomise the look and feel.\n"
         "Related: [[canvas-guide]], [[plugins]].\n"},
        {"plugins.md",
         "# Plugins\n\nExtend MarkAmp with plugins.\n"
         "Related: [[getting-started]], [[themes]].\n"},
        {"export.md",
         "# Export\n\nExport to PDF, HTML, or Markdown.\n"
         "Related: [[markdown-guide]].\n"},
        {"faq.md",
         "# FAQ\n\n**Q: How do I get started?**\nA: See [[getting-started]].\n"
         "**Q: Can I customise themes?**\nA: See [[themes]].\n"},
        {"changelog.md",
         "# Changelog\n\n## v2.0\n- Canvas mode\n- Notebooks\n- Graph view\n"
         "## v1.0\n- Markdown editor\n- Preview\n"},
    };
}

auto get_sample_flashcard_deck() -> std::vector<std::pair<std::string, std::string>>
{
    return {
        {"What Markdown syntax creates a heading?", "Use # symbols (1-6 levels)"},
        {"How do you make text bold?", "Wrap with **double asterisks**"},
        {"How do you make text italic?", "Wrap with *single asterisks* or _underscores_"},
        {"What creates an unordered list?", "Lines starting with -, *, or +"},
        {"What creates an ordered list?", "Lines starting with numbers followed by ."},
        {"How do you create a link?", "[text](url)"},
        {"How do you create a wiki-link?", "[[page-name]] or [[page|display text]]"},
        {"What creates a code block?", "Indent 4 spaces or use ``` fences"},
        {"How do you create a task list?", "- [ ] unchecked, - [x] checked"},
        {"What creates a horizontal rule?", "Three or more ---, ***, or ___"},
    };
}

} // namespace markamp::core
