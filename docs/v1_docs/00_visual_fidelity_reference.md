# MarkAmp Visual Fidelity Reference

## CRITICAL REQUIREMENT

The C++26/wxWidgets implementation MUST produce a UI that is **pixel-perfect identical** to the React/TypeScript reference implementation. Every measurement, color, font weight, border style, hover effect, and spacing value documented here is derived directly from the reference source code and MUST be reproduced exactly.

This document serves as the authoritative visual specification for all UI phases.

---

## 1. Window Chrome (App.tsx lines 102-130)

### Layout
```
[●] MARKAMP v1.0.0     [filename.md]     [☰] | [_] [□] [×]
```

### Specifications
| Property | Value |
|---|---|
| Height | 40px (`h-10`) |
| Background | `--bg-header` |
| Bottom border | 2px `--border-dark` |
| Top border | 1px `--border-light` |
| Padding X | 12px (`px-3`) |
| Select | `user-select: none` |
| Draggable | Entire bar except buttons |

### Left Section: App Identity
| Property | Value |
|---|---|
| Logo dot size | 16x16px (`w-4 h-4`) |
| Logo dot style | `rounded-full` (perfect circle) |
| Logo dot fill | `bg-gradient-to-br from-[accent_primary] to-purple-600` |
| Logo dot glow | `box-shadow: 0 0 10px var(--accent-primary)` |
| App name font | Bold, tracking-wider, 14px (`text-sm`), uppercase |
| App name color | `--accent-primary` |
| App name style | `drop-shadow-sm` |
| Version text | 12px (`text-xs`), `--text-muted`, hidden on small screens |
| Spacing | 12px gap (`space-x-3`) |

### Center Section: Filename
| Property | Value |
|---|---|
| Font | Monospace, 12px (`text-xs font-mono`) |
| Color | `--text-muted` |
| Background | `rgba(0,0,0,0.2)` |
| Padding | 12px horizontal, 4px vertical (`px-3 py-1`) |
| Border radius | 4px (`rounded`) |
| Border | 1px `--border-light` at 20% opacity |
| Visibility | Hidden on screens < md |

### Right Section: Window Controls
| Property | Value |
|---|---|
| Button padding | 6px (`p-1.5`) |
| Button border radius | 4px (`rounded`) |
| Button icon size | Minimize/Maximize: 12px, Close: 12px, Menu: 14px |
| Default color | `--text-muted` |
| Hover background | `rgba(255,255,255,0.1)` (except close) |
| Hover text | `--text-main` (except close) |
| Close hover bg | `rgba(239,68,68,0.8)` (red-500/80) |
| Close hover text | `#ffffff` |
| Separator | 1px wide, 16px tall (`h-4 w-[1px]`), `--border-light` at 30%, 4px margin X |

---

## 2. Toolbar (App.tsx lines 150-192)

### Layout
```
[SRC] [SPLIT] [VIEW]                    [💾] [🎨 THEMES] [⚙]
```

### Specifications
| Property | Value |
|---|---|
| Height | 40px (`h-10`) |
| Background | `--bg-panel` |
| Bottom border | 1px `--border-dark` |
| Padding X | 16px (`px-4`) |
| Justify | `space-between` |

### View Mode Buttons
| Property | Value |
|---|---|
| Padding | 6px (`p-1.5`) |
| Border radius | 4px (`rounded`) |
| Font | 12px (`text-xs`), bold (`font-semibold`) |
| Icon size | 14px |
| Gap icon-text | 6px (`space-x-1.5`) |
| Inactive color | `--text-muted` |
| Inactive hover | `--text-main` |
| Active background | `--accent-primary` at 20% |
| Active color | `--accent-primary` |
| Active shadow | `shadow-sm` |
| Transition | `transition-all` |

### Right Toolbar Icons
| Property | Value |
|---|---|
| Save icon size | 16px |
| Palette icon size | 16px |
| Settings icon size | 16px |
| Default color | `--text-muted` |
| Hover color | `--accent-primary` |
| THEMES text | 12px, semibold, hidden on < md |

---

## 3. Sidebar (Sidebar.tsx)

### Container
| Property | Value |
|---|---|
| Width | 256px (`w-64`) |
| Background | `--bg-panel` |
| Right border | 2px `--border-dark` |
| Left border | 1px `--border-light` |
| Shadow | `shadow-xl` |
| Z-index | 10 |

### Sidebar Header
| Property | Value |
|---|---|
| Padding | 16px horizontal, 12px vertical (`px-4 py-3`) |
| Bottom border | 1px `--border-dark` |
| Background | `--bg-header` at 50% |
| Title font | 12px, bold, uppercase, tracking-widest |
| Title color | `--accent-primary` |
| Title family | Sans-serif (Rajdhani) |

### File Tree Items
| Property | Value |
|---|---|
| Padding Y | 4px (`py-1`) |
| Padding X | 8px (`px-2`) + depth * 16px indent |
| Font | Monospace, 14px (`text-sm font-mono`) |
| Default color | `--text-muted` |
| Hover color | `--text-main` |
| Hover background | `--accent-primary` at 10% |
| Selected color | `--accent-primary` |
| Selected background | `--accent-primary` at 20% |
| Icon size | 14px |
| Icon opacity | 70% |
| Icon margin right | 8px (`mr-2`) |
| Chevron size | 12px |
| Chevron opacity | 50% |
| Text overflow | `truncate` (ellipsis) |
| Transition | `transition-colors duration-100` |

### Sidebar Footer (Stats)
| Property | Value |
|---|---|
| Padding | 12px (`p-3`) |
| Top border | 1px `--border-light` |
| Background | `--bg-app` at 50% |
| Font | 10px monospace, uppercase (`text-[10px] font-mono uppercase`) |
| Color | `--text-muted` |
| Layout | Two rows, each `justify-between` |

---

## 4. Editor (App.tsx lines 21-31)

### Specifications
| Property | Value |
|---|---|
| Background | `--bg-input` |
| Text color | `--text-main` |
| Font | Monospace (JetBrains Mono), 14px (`text-sm`) |
| Padding | 24px (`p-6`) |
| Resize | None |
| Border | None |
| Outline on focus | None |
| Spell check | Disabled |
| Placeholder | "Start typing..." |
| Placeholder color | `--text-muted` at 50% |

### Bevel Overlay (lines 200-202)
| Property | Value |
|---|---|
| Position | Absolute inset-0, pointer-events-none |
| Border width | 2px |
| Top border | `rgba(0,0,0,0.2)` |
| Left border | `rgba(0,0,0,0.2)` |
| Bottom border | `rgba(255,255,255,0.05)` |
| Right border | `rgba(255,255,255,0.05)` |

---

## 5. Preview Pane (Preview.tsx)

### Container
| Property | Value |
|---|---|
| Background | `--bg-app` |
| Overflow Y | Auto with custom scrollbar |
| Padding content | 24px (`p-6`) |
| Font family | Sans-serif (Rajdhani) |
| Max width | None |

### Rendered Element Styles (Preview.tsx lines 42-53)

#### Headings
| Element | Font Size | Weight | Color | Extra |
|---|---|---|---|---|
| h1 | 30px (`text-3xl`) | Bold | `--accent-primary` | border-bottom 1px `--border-light`, pb-2, mb-6 |
| h2 | 24px (`text-2xl`) | Semibold | `--accent-primary` | mt-8, mb-4 |
| h3 | 20px (`text-xl`) | Medium | `--text-main` | mt-6, mb-3 |

#### Text Elements
| Element | Style |
|---|---|
| p | mb-4, leading-relaxed, `--text-main` at 90% opacity |
| a | `--accent-primary`, hover: underline, underline-offset-4, decoration `--accent-primary` at 50% |
| blockquote | border-left 4px `--accent-primary`, pl-4, py-1, italic, bg `--accent-primary` at 5%, my-4 |
| ul | disc markers, pl-6, mb-4, marker color `--accent-primary` |
| ol | decimal markers, pl-6, mb-4, marker color `--accent-primary` |
| hr | `--border-light` color, my-8 |

#### Table
| Element | Style |
|---|---|
| wrapper | overflow-x-auto, my-6, border 1px `--border-light`, rounded |
| table | w-full, text-left, border-collapse |
| th | bg `--bg-panel`, p-3, border-bottom 1px `--border-light`, semibold, `--accent-primary` |
| td | p-3, border-bottom 1px `--border-light` at 30% |

#### Code
| Element | Style |
|---|---|
| inline code | bg `--accent-primary` at 20%, text `--accent-primary`, px-1.5 py-0.5, rounded, text-sm, monospace |
| code block wrapper | relative, group |
| language label | absolute right-2 top-2, 12px, `--text-muted`, opacity-50, monospace |
| pre | bg rgba(0,0,0,0.3), p-4, rounded, border 1px `--border-light`, overflow-x-auto |

### Bevel Overlay (lines 211-213)
Same as editor bevel overlay.

---

## 6. Status Bar (App.tsx lines 218-228)

### Layout
```
[Ready] [Ln 12, Col 42] [UTF-8]          [Mermaid: Active] [Theme Name]
```

### Specifications
| Property | Value |
|---|---|
| Height | 24px (`h-6`) |
| Background | `--bg-panel` |
| Top border | 1px `--border-light` |
| Padding X | 12px (`px-3`) |
| Font | 10px monospace uppercase (`text-[10px] font-mono uppercase`) |
| Color | `--text-muted` |
| Select | `user-select: none` |
| Left group spacing | 16px (`space-x-4`) |
| Right group spacing | 16px (`space-x-4`) |
| Mermaid status color | `--accent-primary` |

---

## 7. Theme Gallery (ThemeGallery.tsx)

### Modal Backdrop
| Property | Value |
|---|---|
| Position | Fixed, inset-0 |
| Z-index | 50 |
| Background | `rgba(0,0,0,0.8)` |
| Backdrop filter | `blur(4px)` (sm) |
| Content alignment | Centered |

### Dialog Container
| Property | Value |
|---|---|
| Width | max 896px (`max-w-4xl`) |
| Height | 80vh (`h-[80vh]`) |
| Background | `--bg-app` |
| Border | 2px `--border-light` |
| Border radius | 8px (`rounded-lg`) |
| Shadow | `shadow-2xl` |
| Overflow | Hidden |

### Gallery Header (lines 136-152)
| Property | Value |
|---|---|
| Background | `--bg-header` |
| Bottom border | 1px `--border-dark` |
| Padding | 16px |
| Icon container | p-2, bg `--accent-primary` at 20%, rounded |
| Icon size | 20px |
| Title font | 20px (`text-xl`), bold, `--text-main` |
| Subtitle font | 12px (`text-xs`), uppercase, tracking-wider, `--text-muted` |
| Close button | 24px icon, `--text-muted`, hover: red-500 at 20% bg, red-400 text |

### Gallery Toolbar (lines 155-182)
| Property | Value |
|---|---|
| Background | `--bg-panel` |
| Bottom border | 1px `--border-light` |
| Padding | 16px |
| Count text | 14px (`text-sm`), `--text-muted`, number in `--accent-primary` monospace |
| Import button | px-4 py-2, bg `--accent-primary` at 10%, border 1px `--accent-primary` at 30%, hover bg 20%, text `--accent-primary`, 14px semibold uppercase tracking-wide |

### Theme Preview Cards (lines 17-71)
| Property | Value |
|---|---|
| Border | 2px |
| Border inactive | `--border-dark` |
| Border hover | `--border-light` |
| Border active | `--accent-primary`, ring-2 `--accent-primary` at 30%, scale 105%, z-10 |
| Border radius | 8px |
| Cursor | pointer |
| Transition | `duration-200` |

### Card Mini Preview (lines 26-49)
| Property | Value |
|---|---|
| Height | 112px (`h-28`) |
| Background | card's `--bg-app` |
| Header height | 24px (`h-6`) |
| Header bg | card's `--bg-header` |
| Header bottom border | card's `--border-dark` |
| Dot 1 | 8x8px circle, card's `--accent-primary` |
| Dot 2 | 8x8px circle, card's `--accent-secondary` at 50% |
| Text bar | 40px wide, 6px tall, card's `--text-muted` at 20% |
| Sidebar | 25% width, card's `--bg-panel`, border card's `--border-light` |
| Sidebar text 1 | 75% width, 6px tall, card's `--text-muted` at 40% |
| Sidebar text 2 | 50% width, 6px tall, card's `--text-muted` at 30% |
| Heading bar | 66% width, 8px tall, card's `--accent-primary` |
| Body bars | 100%/83%/80% width, 6px tall, card's `--text-main` at 80% |

### Card Footer (lines 52-63)
| Property | Value |
|---|---|
| Padding | 12px (`p-3`) |
| Background | `--bg-panel` (active theme, not card theme) |
| Top border | `--border-light` |
| Name font | 14px (`text-sm`), semibold |
| Name active color | `--accent-primary` |
| Name inactive color | `--text-main` |
| Export button | p-1.5, rounded, hover bg `--bg-header`, `--text-muted` -> `--accent-primary` |
| Export icon | 14px |

### Active Indicator (lines 65-69)
| Property | Value |
|---|---|
| Position | Absolute, top-2 right-2 |
| Background | `--accent-primary` |
| Text color | `--bg-app` |
| Padding | 4px (`p-1`) |
| Border radius | Full circle |
| Shadow | `shadow-lg` |
| Checkmark | 12px, stroke-width 4 |

### Grid (line 186)
| Property | Value |
|---|---|
| Columns | 1 / sm:2 / md:3 |
| Gap | 24px (`gap-6`) |
| Padding | 24px (`p-6`) |
| Scrollable | Yes (overflow-y-auto, custom scrollbar) |

---

## 8. Mermaid Error Overlay (MermaidBlock.tsx lines 43-47)

| Property | Value |
|---|---|
| Padding | 16px (`p-4`) |
| Border | 1px `rgba(239,68,68,0.5)` (red-500/50) |
| Background | `rgba(127,29,29,0.2)` (red-900/20) |
| Text color | `rgb(248,113,113)` (red-400) |
| Font | Monospace, 14px (`text-sm`) |
| Border radius | 4px (`rounded`) |

---

## 9. Mermaid Container (MermaidBlock.tsx lines 51-55)

| Property | Value |
|---|---|
| Margin Y | 16px (`my-4`) |
| Display | Flex, justify-center |
| Background | `--bg-panel` at 50% |
| Padding | 16px (`p-4`) |
| Border radius | 4px (`rounded`) |
| Border | 1px `--border-light` |

---

## 10. Fonts

| Usage | Family | Weights |
|---|---|---|
| UI text (chrome, toolbar, sidebar header, status bar, preview body) | Rajdhani | 400 (Regular), 600 (SemiBold), 700 (Bold) |
| Code, editor, file tree, filename display | JetBrains Mono | 400 (Regular), 700 (Bold) |

### Font Files Required
- `JetBrainsMono-Regular.ttf`
- `JetBrainsMono-Bold.ttf`
- `Rajdhani-Regular.ttf`
- `Rajdhani-SemiBold.ttf`
- `Rajdhani-Bold.ttf`

---

## 11. Custom Scrollbar

| Property | Value |
|---|---|
| Width | 8px |
| Height | 8px |
| Track background | `--bg-panel` |
| Thumb background | `--accent-secondary` |
| Thumb border radius | 4px |
| Thumb hover | `--accent-primary` |

---

## 12. Theme Color Variables (10 tokens)

All 8 built-in themes use these exact hex values (from constants.ts):

### Midnight Neon
| Token | Value |
|---|---|
| `--bg-app` | `#050510` |
| `--bg-panel` | `#0a0a1f` |
| `--bg-header` | `#0f0f2d` |
| `--bg-input` | `#050510` |
| `--text-main` | `#00ffea` |
| `--text-muted` | `#5e6ad2` |
| `--accent-primary` | `#ff0055` |
| `--accent-secondary` | `#00ffea` |
| `--border-light` | `rgba(255, 0, 85, 0.2)` |
| `--border-dark` | `#000000` |

### Cyber Night
| Token | Value |
|---|---|
| `--bg-app` | `#1a1a2e` |
| `--bg-panel` | `#16213e` |
| `--bg-header` | `#0f3460` |
| `--bg-input` | `#0f3460` |
| `--text-main` | `#e94560` |
| `--text-muted` | `#533483` |
| `--accent-primary` | `#e94560` |
| `--accent-secondary` | `#533483` |
| `--border-light` | `rgba(255, 255, 255, 0.1)` |
| `--border-dark` | `rgba(0, 0, 0, 0.5)` |

### Solarized Dark
| Token | Value |
|---|---|
| `--bg-app` | `#002b36` |
| `--bg-panel` | `#073642` |
| `--bg-header` | `#002b36` |
| `--bg-input` | `#002b36` |
| `--text-main` | `#839496` |
| `--text-muted` | `#586e75` |
| `--accent-primary` | `#b58900` |
| `--accent-secondary` | `#2aa198` |
| `--border-light` | `#586e75` |
| `--border-dark` | `#001e26` |

### Classic Mono
| Token | Value |
|---|---|
| `--bg-app` | `#ffffff` |
| `--bg-panel` | `#f3f3f3` |
| `--bg-header` | `#e5e5e5` |
| `--bg-input` | `#ffffff` |
| `--text-main` | `#111111` |
| `--text-muted` | `#666666` |
| `--accent-primary` | `#000000` |
| `--accent-secondary` | `#444444` |
| `--border-light` | `#cccccc` |
| `--border-dark` | `#999999` |

### High Contrast Blue
| Token | Value |
|---|---|
| `--bg-app` | `#000080` |
| `--bg-panel` | `#0000a0` |
| `--bg-header` | `#000080` |
| `--bg-input` | `#000000` |
| `--text-main` | `#ffffff` |
| `--text-muted` | `#00ffff` |
| `--accent-primary` | `#ffff00` |
| `--accent-secondary` | `#00ff00` |
| `--border-light` | `#ffffff` |
| `--border-dark` | `#000040` |

### Matrix Core
| Token | Value |
|---|---|
| `--bg-app` | `#0d1117` |
| `--bg-panel` | `#000000` |
| `--bg-header` | `#161b22` |
| `--bg-input` | `#0d1117` |
| `--text-main` | `#00ff41` |
| `--text-muted` | `#008f11` |
| `--accent-primary` | `#00ff41` |
| `--accent-secondary` | `#003b00` |
| `--border-light` | `#222222` |
| `--border-dark` | `#000000` |

### Classic Amp
| Token | Value |
|---|---|
| `--bg-app` | `#282828` |
| `--bg-panel` | `#191919` |
| `--bg-header` | `#333333` |
| `--bg-input` | `#000000` |
| `--text-main` | `#00FF00` |
| `--text-muted` | `#aaaaaa` |
| `--accent-primary` | `#d4af37` |
| `--accent-secondary` | `#888888` |
| `--border-light` | `#555555` |
| `--border-dark` | `#111111` |

### Vapor Wave
| Token | Value |
|---|---|
| `--bg-app` | `#2b1055` |
| `--bg-panel` | `#3d2c8d` |
| `--bg-header` | `#7562e0` |
| `--bg-input` | `#1c0c3f` |
| `--text-main` | `#00ffff` |
| `--text-muted` | `#b8c0ff` |
| `--accent-primary` | `#ff00ff` |
| `--accent-secondary` | `#916bbf` |
| `--border-light` | `rgba(255, 255, 255, 0.2)` |
| `--border-dark` | `rgba(0, 0, 0, 0.6)` |

---

## 13. Visual Verification Process

For every phase that involves UI rendering, the acceptance criteria MUST include:

1. **Side-by-side screenshot comparison** with the React reference
2. **Color value verification** using a color picker on rendered output
3. **Measurement verification** of key dimensions (chrome height, sidebar width, padding values)
4. **Theme cycle test** - visually verify the element across ALL 8 built-in themes
5. **Font rendering check** - verify JetBrains Mono and Rajdhani render correctly

If wxWidgets cannot achieve an exact match for a specific visual effect (e.g., backdrop blur, CSS gradients), the closest possible approximation MUST be used and the deviation documented.
