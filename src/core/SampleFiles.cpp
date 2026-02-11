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

} // namespace markamp::core
