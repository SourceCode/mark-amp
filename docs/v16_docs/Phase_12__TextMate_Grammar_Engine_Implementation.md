# Phase 12 -- TextMate Grammar Engine Implementation

## Objective
Implement full TextMate grammar support to replace stub behavior and enable modern language highlighting.

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
1. Audit current grammar loader for tmLanguage json implementation and document gaps against v16 targets.
2. Define contract/schema for grammar loader for tmLanguage json with migration notes and edge cases.
3. Implement grammar loader for tmLanguage json core refactor with backward-compatible adapter paths.
4. Add automated tests for grammar loader for tmLanguage json including failure-path coverage.
5. Publish grammar loader for tmLanguage json documentation and phase acceptance checklist.
6. Audit current scope stack and state machine implementation and document gaps against v16 targets.
7. Define contract/schema for scope stack and state machine with migration notes and edge cases.
8. Implement scope stack and state machine core refactor with backward-compatible adapter paths.
9. Add automated tests for scope stack and state machine including failure-path coverage.
10. Publish scope stack and state machine documentation and phase acceptance checklist.
11. Audit current injection grammar support implementation and document gaps against v16 targets.
12. Define contract/schema for injection grammar support with migration notes and edge cases.
13. Implement injection grammar support core refactor with backward-compatible adapter paths.
14. Add automated tests for injection grammar support including failure-path coverage.
15. Publish injection grammar support documentation and phase acceptance checklist.
16. Audit current multi-line rule support implementation and document gaps against v16 targets.
17. Define contract/schema for multi-line rule support with migration notes and edge cases.
18. Implement multi-line rule support core refactor with backward-compatible adapter paths.
19. Add automated tests for multi-line rule support including failure-path coverage.
20. Publish multi-line rule support documentation and phase acceptance checklist.
21. Audit current backtracking and catastrophic regex safeguards implementation and document gaps against v16 targets.
22. Define contract/schema for backtracking and catastrophic regex safeguards with migration notes and edge cases.
23. Implement backtracking and catastrophic regex safeguards core refactor with backward-compatible adapter paths.
24. Add automated tests for backtracking and catastrophic regex safeguards including failure-path coverage.
25. Publish backtracking and catastrophic regex safeguards documentation and phase acceptance checklist.
26. Audit current grammar caching and invalidation implementation and document gaps against v16 targets.
27. Define contract/schema for grammar caching and invalidation with migration notes and edge cases.
28. Implement grammar caching and invalidation core refactor with backward-compatible adapter paths.
29. Add automated tests for grammar caching and invalidation including failure-path coverage.
30. Publish grammar caching and invalidation documentation and phase acceptance checklist.
31. Audit current language-to-scope resolver implementation and document gaps against v16 targets.
32. Define contract/schema for language-to-scope resolver with migration notes and edge cases.
33. Implement language-to-scope resolver core refactor with backward-compatible adapter paths.
34. Add automated tests for language-to-scope resolver including failure-path coverage.
35. Publish language-to-scope resolver documentation and phase acceptance checklist.
36. Audit current theme scope matching pipeline implementation and document gaps against v16 targets.
37. Define contract/schema for theme scope matching pipeline with migration notes and edge cases.
38. Implement theme scope matching pipeline core refactor with backward-compatible adapter paths.
39. Add automated tests for theme scope matching pipeline including failure-path coverage.
40. Publish theme scope matching pipeline documentation and phase acceptance checklist.
41. Audit current diagnostics for grammar parse failures implementation and document gaps against v16 targets.
42. Define contract/schema for diagnostics for grammar parse failures with migration notes and edge cases.
43. Implement diagnostics for grammar parse failures core refactor with backward-compatible adapter paths.
44. Add automated tests for diagnostics for grammar parse failures including failure-path coverage.
45. Publish diagnostics for grammar parse failures documentation and phase acceptance checklist.
46. Audit current tests for grammar correctness and performance implementation and document gaps against v16 targets.
47. Define contract/schema for tests for grammar correctness and performance with migration notes and edge cases.
48. Implement tests for grammar correctness and performance core refactor with backward-compatible adapter paths.
49. Add automated tests for tests for grammar correctness and performance including failure-path coverage.
50. Publish tests for grammar correctness and performance documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
