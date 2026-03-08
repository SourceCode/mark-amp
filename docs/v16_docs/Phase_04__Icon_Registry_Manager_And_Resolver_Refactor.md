# Phase 04 -- Icon Registry Manager And Resolver Refactor

## Objective
Refactor icon registries/resolvers so all icon requests route through a single canonical pipeline.

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
1. Audit current icon registry API modernization implementation and document gaps against v16 targets.
2. Define contract/schema for icon registry API modernization with migration notes and edge cases.
3. Implement icon registry API modernization core refactor with backward-compatible adapter paths.
4. Add automated tests for icon registry API modernization including failure-path coverage.
5. Publish icon registry API modernization documentation and phase acceptance checklist.
6. Audit current icon manager lifecycle and thread safety implementation and document gaps against v16 targets.
7. Define contract/schema for icon manager lifecycle and thread safety with migration notes and edge cases.
8. Implement icon manager lifecycle and thread safety core refactor with backward-compatible adapter paths.
9. Add automated tests for icon manager lifecycle and thread safety including failure-path coverage.
10. Publish icon manager lifecycle and thread safety documentation and phase acceptance checklist.
11. Audit current icon provider canonical id migration implementation and document gaps against v16 targets.
12. Define contract/schema for icon provider canonical id migration with migration notes and edge cases.
13. Implement icon provider canonical id migration core refactor with backward-compatible adapter paths.
14. Add automated tests for icon provider canonical id migration including failure-path coverage.
15. Publish icon provider canonical id migration documentation and phase acceptance checklist.
16. Audit current file type resolver parity with manifest implementation and document gaps against v16 targets.
17. Define contract/schema for file type resolver parity with manifest with migration notes and edge cases.
18. Implement file type resolver parity with manifest core refactor with backward-compatible adapter paths.
19. Add automated tests for file type resolver parity with manifest including failure-path coverage.
20. Publish file type resolver parity with manifest documentation and phase acceptance checklist.
21. Audit current folder icon resolver parity with manifest implementation and document gaps against v16 targets.
22. Define contract/schema for folder icon resolver parity with manifest with migration notes and edge cases.
23. Implement folder icon resolver parity with manifest core refactor with backward-compatible adapter paths.
24. Add automated tests for folder icon resolver parity with manifest including failure-path coverage.
25. Publish folder icon resolver parity with manifest documentation and phase acceptance checklist.
26. Audit current command icon mapping normalization implementation and document gaps against v16 targets.
27. Define contract/schema for command icon mapping normalization with migration notes and edge cases.
28. Implement command icon mapping normalization core refactor with backward-compatible adapter paths.
29. Add automated tests for command icon mapping normalization including failure-path coverage.
30. Publish command icon mapping normalization documentation and phase acceptance checklist.
31. Audit current status/action icon mapping normalization implementation and document gaps against v16 targets.
32. Define contract/schema for status/action icon mapping normalization with migration notes and edge cases.
33. Implement status/action icon mapping normalization core refactor with backward-compatible adapter paths.
34. Add automated tests for status/action icon mapping normalization including failure-path coverage.
35. Publish status/action icon mapping normalization documentation and phase acceptance checklist.
36. Audit current deprecation of inline icon literals implementation and document gaps against v16 targets.
37. Define contract/schema for deprecation of inline icon literals with migration notes and edge cases.
38. Implement deprecation of inline icon literals core refactor with backward-compatible adapter paths.
39. Add automated tests for deprecation of inline icon literals including failure-path coverage.
40. Publish deprecation of inline icon literals documentation and phase acceptance checklist.
41. Audit current error handling and telemetry for resolution failures implementation and document gaps against v16 targets.
42. Define contract/schema for error handling and telemetry for resolution failures with migration notes and edge cases.
43. Implement error handling and telemetry for resolution failures core refactor with backward-compatible adapter paths.
44. Add automated tests for error handling and telemetry for resolution failures including failure-path coverage.
45. Publish error handling and telemetry for resolution failures documentation and phase acceptance checklist.
46. Audit current compatibility shim for legacy icon names implementation and document gaps against v16 targets.
47. Define contract/schema for compatibility shim for legacy icon names with migration notes and edge cases.
48. Implement compatibility shim for legacy icon names core refactor with backward-compatible adapter paths.
49. Add automated tests for compatibility shim for legacy icon names including failure-path coverage.
50. Publish compatibility shim for legacy icon names documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
