# Phase 19 -- Theme Marketplace Compatibility And Import Export

## Objective
Improve marketplace/import/export flows so third-party themes correctly style advanced syntax and icon layers.

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
1. Audit current vscode theme import fidelity implementation and document gaps against v16 targets.
2. Define contract/schema for vscode theme import fidelity with migration notes and edge cases.
3. Implement vscode theme import fidelity core refactor with backward-compatible adapter paths.
4. Add automated tests for vscode theme import fidelity including failure-path coverage.
5. Publish vscode theme import fidelity documentation and phase acceptance checklist.
6. Audit current token scope conversion completeness implementation and document gaps against v16 targets.
7. Define contract/schema for token scope conversion completeness with migration notes and edge cases.
8. Implement token scope conversion completeness core refactor with backward-compatible adapter paths.
9. Add automated tests for token scope conversion completeness including failure-path coverage.
10. Publish token scope conversion completeness documentation and phase acceptance checklist.
11. Audit current semantic token conversion support implementation and document gaps against v16 targets.
12. Define contract/schema for semantic token conversion support with migration notes and edge cases.
13. Implement semantic token conversion support core refactor with backward-compatible adapter paths.
14. Add automated tests for semantic token conversion support including failure-path coverage.
15. Publish semantic token conversion support documentation and phase acceptance checklist.
16. Audit current icon tint and ui token conversion implementation and document gaps against v16 targets.
17. Define contract/schema for icon tint and ui token conversion with migration notes and edge cases.
18. Implement icon tint and ui token conversion core refactor with backward-compatible adapter paths.
19. Add automated tests for icon tint and ui token conversion including failure-path coverage.
20. Publish icon tint and ui token conversion documentation and phase acceptance checklist.
21. Audit current theme export fidelity implementation and document gaps against v16 targets.
22. Define contract/schema for theme export fidelity with migration notes and edge cases.
23. Implement theme export fidelity core refactor with backward-compatible adapter paths.
24. Add automated tests for theme export fidelity including failure-path coverage.
25. Publish theme export fidelity documentation and phase acceptance checklist.
26. Audit current marketplace metadata validation implementation and document gaps against v16 targets.
27. Define contract/schema for marketplace metadata validation with migration notes and edge cases.
28. Implement marketplace metadata validation core refactor with backward-compatible adapter paths.
29. Add automated tests for marketplace metadata validation including failure-path coverage.
30. Publish marketplace metadata validation documentation and phase acceptance checklist.
31. Audit current compatibility diagnostics messaging implementation and document gaps against v16 targets.
32. Define contract/schema for compatibility diagnostics messaging with migration notes and edge cases.
33. Implement compatibility diagnostics messaging core refactor with backward-compatible adapter paths.
34. Add automated tests for compatibility diagnostics messaging including failure-path coverage.
35. Publish compatibility diagnostics messaging documentation and phase acceptance checklist.
36. Audit current theme pack signing and trust checks implementation and document gaps against v16 targets.
37. Define contract/schema for theme pack signing and trust checks with migration notes and edge cases.
38. Implement theme pack signing and trust checks core refactor with backward-compatible adapter paths.
39. Add automated tests for theme pack signing and trust checks including failure-path coverage.
40. Publish theme pack signing and trust checks documentation and phase acceptance checklist.
41. Audit current user override merge behavior implementation and document gaps against v16 targets.
42. Define contract/schema for user override merge behavior with migration notes and edge cases.
43. Implement user override merge behavior core refactor with backward-compatible adapter paths.
44. Add automated tests for user override merge behavior including failure-path coverage.
45. Publish user override merge behavior documentation and phase acceptance checklist.
46. Audit current round-trip import/export regression tests implementation and document gaps against v16 targets.
47. Define contract/schema for round-trip import/export regression tests with migration notes and edge cases.
48. Implement round-trip import/export regression tests core refactor with backward-compatible adapter paths.
49. Add automated tests for round-trip import/export regression tests including failure-path coverage.
50. Publish round-trip import/export regression tests documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
