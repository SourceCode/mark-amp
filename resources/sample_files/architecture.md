# Architecture

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
