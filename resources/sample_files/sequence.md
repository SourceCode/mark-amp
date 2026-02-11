# File Open Sequence

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
