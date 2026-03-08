# Phase 05 -- File And Folder Icons VSCode Parity

## Objective
Achieve high-fidelity file/folder icon coverage comparable to VS Code Material Icon Theme behavior.

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
1. Audit current file extension to icon mapping breadth implementation and document gaps against v16 targets.
2. Define contract/schema for file extension to icon mapping breadth with migration notes and edge cases.
3. Implement file extension to icon mapping breadth core refactor with backward-compatible adapter paths.
4. Add automated tests for file extension to icon mapping breadth including failure-path coverage.
5. Publish file extension to icon mapping breadth documentation and phase acceptance checklist.
6. Audit current special filename to icon mapping implementation and document gaps against v16 targets.
7. Define contract/schema for special filename to icon mapping with migration notes and edge cases.
8. Implement special filename to icon mapping core refactor with backward-compatible adapter paths.
9. Add automated tests for special filename to icon mapping including failure-path coverage.
10. Publish special filename to icon mapping documentation and phase acceptance checklist.
11. Audit current folder-name semantic icon mapping implementation and document gaps against v16 targets.
12. Define contract/schema for folder-name semantic icon mapping with migration notes and edge cases.
13. Implement folder-name semantic icon mapping core refactor with backward-compatible adapter paths.
14. Add automated tests for folder-name semantic icon mapping including failure-path coverage.
15. Publish folder-name semantic icon mapping documentation and phase acceptance checklist.
16. Audit current open/closed folder state icon mapping implementation and document gaps against v16 targets.
17. Define contract/schema for open/closed folder state icon mapping with migration notes and edge cases.
18. Implement open/closed folder state icon mapping core refactor with backward-compatible adapter paths.
19. Add automated tests for open/closed folder state icon mapping including failure-path coverage.
20. Publish open/closed folder state icon mapping documentation and phase acceptance checklist.
21. Audit current workspace root and project icon mapping implementation and document gaps against v16 targets.
22. Define contract/schema for workspace root and project icon mapping with migration notes and edge cases.
23. Implement workspace root and project icon mapping core refactor with backward-compatible adapter paths.
24. Add automated tests for workspace root and project icon mapping including failure-path coverage.
25. Publish workspace root and project icon mapping documentation and phase acceptance checklist.
26. Audit current language-specific icon nuance rules implementation and document gaps against v16 targets.
27. Define contract/schema for language-specific icon nuance rules with migration notes and edge cases.
28. Implement language-specific icon nuance rules core refactor with backward-compatible adapter paths.
29. Add automated tests for language-specific icon nuance rules including failure-path coverage.
30. Publish language-specific icon nuance rules documentation and phase acceptance checklist.
31. Audit current hidden/config file icon rules implementation and document gaps against v16 targets.
32. Define contract/schema for hidden/config file icon rules with migration notes and edge cases.
33. Implement hidden/config file icon rules core refactor with backward-compatible adapter paths.
34. Add automated tests for hidden/config file icon rules including failure-path coverage.
35. Publish hidden/config file icon rules documentation and phase acceptance checklist.
36. Audit current test fixtures for extension coverage implementation and document gaps against v16 targets.
37. Define contract/schema for test fixtures for extension coverage with migration notes and edge cases.
38. Implement test fixtures for extension coverage core refactor with backward-compatible adapter paths.
39. Add automated tests for test fixtures for extension coverage including failure-path coverage.
40. Publish test fixtures for extension coverage documentation and phase acceptance checklist.
41. Audit current ui rendering updates in file tree implementation and document gaps against v16 targets.
42. Define contract/schema for ui rendering updates in file tree with migration notes and edge cases.
43. Implement ui rendering updates in file tree core refactor with backward-compatible adapter paths.
44. Add automated tests for ui rendering updates in file tree including failure-path coverage.
45. Publish ui rendering updates in file tree documentation and phase acceptance checklist.
46. Audit current user override hooks for icon associations implementation and document gaps against v16 targets.
47. Define contract/schema for user override hooks for icon associations with migration notes and edge cases.
48. Implement user override hooks for icon associations core refactor with backward-compatible adapter paths.
49. Add automated tests for user override hooks for icon associations including failure-path coverage.
50. Publish user override hooks for icon associations documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
