import { Theme, FileNode } from './types';

export const THEMES: Theme[] = [
  {
    id: 'midnight-neon',
    name: 'Midnight Neon',
    colors: {
      '--bg-app': '#050510',
      '--bg-panel': '#0a0a1f',
      '--bg-header': '#0f0f2d',
      '--bg-input': '#050510',
      '--text-main': '#00ffea',
      '--text-muted': '#5e6ad2',
      '--accent-primary': '#ff0055',
      '--accent-secondary': '#00ffea',
      '--border-light': 'rgba(255, 0, 85, 0.2)',
      '--border-dark': '#000000',
    },
  },
  {
    id: 'cyber-night',
    name: 'Cyber Night',
    colors: {
      '--bg-app': '#1a1a2e',
      '--bg-panel': '#16213e',
      '--bg-header': '#0f3460',
      '--bg-input': '#0f3460',
      '--text-main': '#e94560',
      '--text-muted': '#533483',
      '--accent-primary': '#e94560',
      '--accent-secondary': '#533483',
      '--border-light': 'rgba(255, 255, 255, 0.1)',
      '--border-dark': 'rgba(0, 0, 0, 0.5)',
    },
  },
  {
    id: 'solarized-dark',
    name: 'Solarized Dark',
    colors: {
      '--bg-app': '#002b36',
      '--bg-panel': '#073642',
      '--bg-header': '#002b36',
      '--bg-input': '#002b36',
      '--text-main': '#839496',
      '--text-muted': '#586e75',
      '--accent-primary': '#b58900',
      '--accent-secondary': '#2aa198',
      '--border-light': '#586e75',
      '--border-dark': '#001e26',
    },
  },
  {
    id: 'classic-mono',
    name: 'Classic Mono',
    colors: {
      '--bg-app': '#ffffff',
      '--bg-panel': '#f3f3f3',
      '--bg-header': '#e5e5e5',
      '--bg-input': '#ffffff',
      '--text-main': '#111111',
      '--text-muted': '#666666',
      '--accent-primary': '#000000',
      '--accent-secondary': '#444444',
      '--border-light': '#cccccc',
      '--border-dark': '#999999',
    },
  },
  {
    id: 'high-contrast-blue',
    name: 'High Contrast Blue',
    colors: {
      '--bg-app': '#000080',
      '--bg-panel': '#0000a0',
      '--bg-header': '#000080',
      '--bg-input': '#000000',
      '--text-main': '#ffffff',
      '--text-muted': '#00ffff',
      '--accent-primary': '#ffff00',
      '--accent-secondary': '#00ff00',
      '--border-light': '#ffffff',
      '--border-dark': '#000040',
    },
  },
  {
    id: 'matrix-core',
    name: 'Matrix Core',
    colors: {
      '--bg-app': '#0d1117',
      '--bg-panel': '#000000',
      '--bg-header': '#161b22',
      '--bg-input': '#0d1117',
      '--text-main': '#00ff41',
      '--text-muted': '#008f11',
      '--accent-primary': '#00ff41',
      '--accent-secondary': '#003b00',
      '--border-light': '#222',
      '--border-dark': '#000',
    },
  },
  {
    id: 'winamp-classic',
    name: 'Classic Amp',
    colors: {
      '--bg-app': '#282828',
      '--bg-panel': '#191919',
      '--bg-header': '#333333',
      '--bg-input': '#000000',
      '--text-main': '#00FF00',
      '--text-muted': '#aaaaaa',
      '--accent-primary': '#d4af37',
      '--accent-secondary': '#888888',
      '--border-light': '#555555',
      '--border-dark': '#111111',
    },
  },
   {
    id: 'vapor-wave',
    name: 'Vapor Wave',
    colors: {
      '--bg-app': '#2b1055',
      '--bg-panel': '#3d2c8d',
      '--bg-header': '#7562e0',
      '--bg-input': '#1c0c3f',
      '--text-main': '#00ffff',
      '--text-muted': '#b8c0ff',
      '--accent-primary': '#ff00ff',
      '--accent-secondary': '#916bbf',
      '--border-light': 'rgba(255, 255, 255, 0.2)',
      '--border-dark': 'rgba(0, 0, 0, 0.6)',
    },
  },
];

export const INITIAL_FILES: FileNode[] = [
  {
    id: 'root',
    name: 'My Project',
    type: 'folder',
    isOpen: true,
    children: [
      {
        id: 'readme',
        name: 'README.md',
        type: 'file',
        content: `# MarkAmp v1.0

> The ultimate retro-futuristic Markdown editor.

## Features
- **Split View Editing**: Code on the left, preview on the right.
- **Mermaid Support**: Native diagram rendering.
- **Theming**: Hot-swappable themes.

## Syntax Highlighting

\`\`\`javascript
function helloWorld() {
  console.log("Hello, MarkAmp!");
}
\`\`\`

## Tables

| Feature | Status |
| :--- | :--- |
| Markdown | ✅ |
| Mermaid | ✅ |
| Themes | ✅ |

## Checklist
- [x] Launch MarkAmp
- [ ] Create a cool theme
- [ ] Write documentation
`,
      },
      {
        id: 'folder-diagrams',
        name: 'Diagrams',
        type: 'folder',
        isOpen: true,
        children: [
          {
            id: 'arch-diagram',
            name: 'architecture.md',
            type: 'file',
            content: `# System Architecture

Here is the current system flow:

\`\`\`mermaid
graph TD
    A[Client] -->|HTTP| B(Load Balancer)
    B --> C{Service}
    C -->|Get| D[Database]
    C -->|Cache| E[Redis]
    style A fill:#f9f,stroke:#333,stroke-width:4px
    style D fill:#bbf,stroke:#333,stroke-width:2px
\`\`\`

> Note: The load balancer handles SSL termination.
`,
          },
          {
            id: 'sequence',
            name: 'sequence.md',
            type: 'file',
            content: `# Sequence Diagram Example

\`\`\`mermaid
sequenceDiagram
    participant Alice
    participant Bob
    Alice->>John: Hello John, how are you?
    loop Healthcheck
        John->>John: Fight against hypochondria
    end
    Note right of John: Rational thoughts <br/>prevail!
    John-->>Alice: Great!
    John->>Bob: How about you?
    Bob-->>John: Jolly good!
\`\`\`
`
          }
        ],
      },
      {
        id: 'todo',
        name: 'TODO.md',
        type: 'file',
        content: `- [ ] Refactor UI components
- [x] Add Mermaid support
- [ ] Fix scroll sync
`
      }
    ],
  },
];