# Phase 17 -- Advanced Code Styling And Theme Tokens

## Objective
Upgrade code styling to advanced quality with richer token classes, typography controls, and nuanced visual hierarchy.

## Context
This phase is part of the v16 UI/UX overhaul program focused on replacing all icons with a VS Code Material Icon Theme-style system and elevating syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- **Canonical Icon ID:** Stable string key used in code (for example, ).
- **Icon Manifest:** Versioned metadata file mapping file/folder/context keys to icon assets.
- **Scope Token:** TextMate-like token scope such as  or  used for syntax theme mapping.
- **Language Pack:** Bundle containing grammar, injection rules, and language metadata.
- **Quality Gate:** A test or metric threshold that must pass before merge/release.

## Reference Files (Full Paths)
- \
- \
- \
- \
- \

## Implementation Details For AI Coding Agents
- Preserve backward compatibility by introducing adapter layers before removing legacy icon IDs or syntax classes.
- When replacing icons, prefer manifest-driven lookup over inline SVG literals or emoji.
- Add deterministic tests for mapping completeness, missing asset handling, and fallback behavior.
- Keep all newly introduced color/styling values theme-tokenized; avoid hardcoded colors.
- For cross-platform work, explicitly test font fallback and rendering parity on macOS/Windows/Linux runners.

## Example
\: command icon resolves to emoji from \.
\: command icon resolves via canonical ID -> manifest entry -> SVG asset -> themed render pipeline.

## Tasks
1. Audit current token color ramp design system implementation and document gaps against v16 targets.
2. Define contract/schema for token color ramp design system with migration notes and edge cases.
3. Implement token color ramp design system core refactor with backward-compatible adapter paths.
4. Add automated tests for token color ramp design system including failure-path coverage.
5. Publish token color ramp design system documentation and phase acceptance checklist.
6. Audit current font weight/style policy by token type implementation and document gaps against v16 targets.
7. Define contract/schema for font weight/style policy by token type with migration notes and edge cases.
8. Implement font weight/style policy by token type core refactor with backward-compatible adapter paths.
9. Add automated tests for font weight/style policy by token type including failure-path coverage.
10. Publish font weight/style policy by token type documentation and phase acceptance checklist.
11. Audit current italic/bold/underline semantic usage implementation and document gaps against v16 targets.
12. Define contract/schema for italic/bold/underline semantic usage with migration notes and edge cases.
13. Implement italic/bold/underline semantic usage core refactor with backward-compatible adapter paths.
14. Add automated tests for italic/bold/underline semantic usage including failure-path coverage.
15. Publish italic/bold/underline semantic usage documentation and phase acceptance checklist.
16. Audit current operator and punctuation styling nuance implementation and document gaps against v16 targets.
17. Define contract/schema for operator and punctuation styling nuance with migration notes and edge cases.
18. Implement operator and punctuation styling nuance core refactor with backward-compatible adapter paths.
19. Add automated tests for operator and punctuation styling nuance including failure-path coverage.
20. Publish operator and punctuation styling nuance documentation and phase acceptance checklist.
21. Audit current comment legibility and contrast policy implementation and document gaps against v16 targets.
22. Define contract/schema for comment legibility and contrast policy with migration notes and edge cases.
23. Implement comment legibility and contrast policy core refactor with backward-compatible adapter paths.
24. Add automated tests for comment legibility and contrast policy including failure-path coverage.
25. Publish comment legibility and contrast policy documentation and phase acceptance checklist.
26. Audit current invalid/deprecated token styling implementation and document gaps against v16 targets.
27. Define contract/schema for invalid/deprecated token styling with migration notes and edge cases.
28. Implement invalid/deprecated token styling core refactor with backward-compatible adapter paths.
29. Add automated tests for invalid/deprecated token styling including failure-path coverage.
30. Publish invalid/deprecated token styling documentation and phase acceptance checklist.
31. Audit current rainbow delimiters option implementation and document gaps against v16 targets.
32. Define contract/schema for rainbow delimiters option with migration notes and edge cases.
33. Implement rainbow delimiters option core refactor with backward-compatible adapter paths.
34. Add automated tests for rainbow delimiters option including failure-path coverage.
35. Publish rainbow delimiters option documentation and phase acceptance checklist.
36. Audit current bracket pair colorization option implementation and document gaps against v16 targets.
37. Define contract/schema for bracket pair colorization option with migration notes and edge cases.
38. Implement bracket pair colorization option core refactor with backward-compatible adapter paths.
39. Add automated tests for bracket pair colorization option including failure-path coverage.
40. Publish bracket pair colorization option documentation and phase acceptance checklist.
41. Audit current inline diagnostics styling integration implementation and document gaps against v16 targets.
42. Define contract/schema for inline diagnostics styling integration with migration notes and edge cases.
43. Implement inline diagnostics styling integration core refactor with backward-compatible adapter paths.
44. Add automated tests for inline diagnostics styling integration including failure-path coverage.
45. Publish inline diagnostics styling integration documentation and phase acceptance checklist.
46. Audit current user-customizable code style presets implementation and document gaps against v16 targets.
47. Define contract/schema for user-customizable code style presets with migration notes and edge cases.
48. Implement user-customizable code style presets core refactor with backward-compatible adapter paths.
49. Add automated tests for user-customizable code style presets including failure-path coverage.
50. Publish user-customizable code style presets documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
