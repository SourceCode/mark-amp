# Phase 02 -- Material Icon Set Licensing And Architecture

## Objective
Establish legal, architectural, and operational foundations for adopting a VS Code Material Icon Theme-style icon system.

## Context
This phase is part of the v16 UI/UX overhaul focused on replacing all icons with a VS Code Material Icon Theme-style system and advancing syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- Canonical Icon ID: Stable string key used in code (example: icon.file.ts).
- Icon Manifest: Versioned metadata file mapping file/folder/context keys to icon assets.
- Scope Token: TextMate-like token scope such as source.ts or keyword.control used for syntax theme mapping.
- Language Pack: Bundle containing grammar, injection rules, and language metadata.
- Quality Gate: A test or metric threshold that must pass before merge or release.

## Reference Files (Full Paths)
- /Users/ryanrentfro/code/markamp/resources/icons/lucide/folder.svg
- /Users/ryanrentfro/code/markamp/src/ui/IconRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconCache.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

## Implementation Details For AI Coding Agents
- Preserve backward compatibility by introducing adapter layers before removing legacy icon IDs or syntax classes.
- When replacing icons, prefer manifest-driven lookup over inline SVG literals or emoji.
- Add deterministic tests for mapping completeness, missing asset handling, and fallback behavior.
- Keep all newly introduced styling values theme-tokenized; avoid hardcoded colors.
- For cross-platform work, explicitly test font fallback and rendering parity on macOS, Windows, and Linux runners.

## Example
Old: command icon resolves to emoji from /Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp.
New: command icon resolves via canonical ID -> manifest entry -> SVG asset -> themed render pipeline.

## Tasks
1. Audit current third-party icon license verification and attribution implementation and document gaps against v16 targets.
2. Define contract/schema for third-party icon license verification and attribution with migration notes and edge cases.
3. Implement third-party icon license verification and attribution core refactor with backward-compatible adapter paths.
4. Add automated tests for third-party icon license verification and attribution including failure-path coverage.
5. Publish third-party icon license verification and attribution documentation and phase acceptance checklist.
6. Audit current icon pack ingestion architecture implementation and document gaps against v16 targets.
7. Define contract/schema for icon pack ingestion architecture with migration notes and edge cases.
8. Implement icon pack ingestion architecture core refactor with backward-compatible adapter paths.
9. Add automated tests for icon pack ingestion architecture including failure-path coverage.
10. Publish icon pack ingestion architecture documentation and phase acceptance checklist.
11. Audit current canonical icon id naming standard implementation and document gaps against v16 targets.
12. Define contract/schema for canonical icon id naming standard with migration notes and edge cases.
13. Implement canonical icon id naming standard core refactor with backward-compatible adapter paths.
14. Add automated tests for canonical icon id naming standard including failure-path coverage.
15. Publish canonical icon id naming standard documentation and phase acceptance checklist.
16. Audit current icon manifest format and schema implementation and document gaps against v16 targets.
17. Define contract/schema for icon manifest format and schema with migration notes and edge cases.
18. Implement icon manifest format and schema core refactor with backward-compatible adapter paths.
19. Add automated tests for icon manifest format and schema including failure-path coverage.
20. Publish icon manifest format and schema documentation and phase acceptance checklist.
21. Audit current theme-aware icon color policy implementation and document gaps against v16 targets.
22. Define contract/schema for theme-aware icon color policy with migration notes and edge cases.
23. Implement theme-aware icon color policy core refactor with backward-compatible adapter paths.
24. Add automated tests for theme-aware icon color policy including failure-path coverage.
25. Publish theme-aware icon color policy documentation and phase acceptance checklist.
26. Audit current high-DPI icon rendering policy implementation and document gaps against v16 targets.
27. Define contract/schema for high-DPI icon rendering policy with migration notes and edge cases.
28. Implement high-DPI icon rendering policy core refactor with backward-compatible adapter paths.
29. Add automated tests for high-DPI icon rendering policy including failure-path coverage.
30. Publish high-DPI icon rendering policy documentation and phase acceptance checklist.
31. Audit current fallback/missing-icon behavior implementation and document gaps against v16 targets.
32. Define contract/schema for fallback/missing-icon behavior with migration notes and edge cases.
33. Implement fallback/missing-icon behavior core refactor with backward-compatible adapter paths.
34. Add automated tests for fallback/missing-icon behavior including failure-path coverage.
35. Publish fallback/missing-icon behavior documentation and phase acceptance checklist.
36. Audit current migration strategy from lucide and emoji icons implementation and document gaps against v16 targets.
37. Define contract/schema for migration strategy from lucide and emoji icons with migration notes and edge cases.
38. Implement migration strategy from lucide and emoji icons core refactor with backward-compatible adapter paths.
39. Add automated tests for migration strategy from lucide and emoji icons including failure-path coverage.
40. Publish migration strategy from lucide and emoji icons documentation and phase acceptance checklist.
41. Audit current asset versioning and change control implementation and document gaps against v16 targets.
42. Define contract/schema for asset versioning and change control with migration notes and edge cases.
43. Implement asset versioning and change control core refactor with backward-compatible adapter paths.
44. Add automated tests for asset versioning and change control including failure-path coverage.
45. Publish asset versioning and change control documentation and phase acceptance checklist.
46. Audit current security review for SVG inputs implementation and document gaps against v16 targets.
47. Define contract/schema for security review for SVG inputs with migration notes and edge cases.
48. Implement security review for SVG inputs core refactor with backward-compatible adapter paths.
49. Add automated tests for security review for SVG inputs including failure-path coverage.
50. Publish security review for SVG inputs documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
