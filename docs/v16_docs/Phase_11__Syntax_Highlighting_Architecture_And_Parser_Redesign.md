# Phase 11 -- Syntax Highlighting Architecture And Parser Redesign

## Objective
Redesign syntax-highlighting architecture for extensibility, accuracy, and broad language coverage.

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
1. Audit current current tokenizer correctness audit implementation and document gaps against v16 targets.
2. Define contract/schema for current tokenizer correctness audit with migration notes and edge cases.
3. Implement current tokenizer correctness audit core refactor with backward-compatible adapter paths.
4. Add automated tests for current tokenizer correctness audit including failure-path coverage.
5. Publish current tokenizer correctness audit documentation and phase acceptance checklist.
6. Audit current token taxonomy redesign implementation and document gaps against v16 targets.
7. Define contract/schema for token taxonomy redesign with migration notes and edge cases.
8. Implement token taxonomy redesign core refactor with backward-compatible adapter paths.
9. Add automated tests for token taxonomy redesign including failure-path coverage.
10. Publish token taxonomy redesign documentation and phase acceptance checklist.
11. Audit current incremental tokenization strategy implementation and document gaps against v16 targets.
12. Define contract/schema for incremental tokenization strategy with migration notes and edge cases.
13. Implement incremental tokenization strategy core refactor with backward-compatible adapter paths.
14. Add automated tests for incremental tokenization strategy including failure-path coverage.
15. Publish incremental tokenization strategy documentation and phase acceptance checklist.
16. Audit current language abstraction contract implementation and document gaps against v16 targets.
17. Define contract/schema for language abstraction contract with migration notes and edge cases.
18. Implement language abstraction contract core refactor with backward-compatible adapter paths.
19. Add automated tests for language abstraction contract including failure-path coverage.
20. Publish language abstraction contract documentation and phase acceptance checklist.
21. Audit current scope assignment consistency rules implementation and document gaps against v16 targets.
22. Define contract/schema for scope assignment consistency rules with migration notes and edge cases.
23. Implement scope assignment consistency rules core refactor with backward-compatible adapter paths.
24. Add automated tests for scope assignment consistency rules including failure-path coverage.
25. Publish scope assignment consistency rules documentation and phase acceptance checklist.
26. Audit current error-tolerant parsing behavior implementation and document gaps against v16 targets.
27. Define contract/schema for error-tolerant parsing behavior with migration notes and edge cases.
28. Implement error-tolerant parsing behavior core refactor with backward-compatible adapter paths.
29. Add automated tests for error-tolerant parsing behavior including failure-path coverage.
30. Publish error-tolerant parsing behavior documentation and phase acceptance checklist.
31. Audit current embedded-language parsing strategy implementation and document gaps against v16 targets.
32. Define contract/schema for embedded-language parsing strategy with migration notes and edge cases.
33. Implement embedded-language parsing strategy core refactor with backward-compatible adapter paths.
34. Add automated tests for embedded-language parsing strategy including failure-path coverage.
35. Publish embedded-language parsing strategy documentation and phase acceptance checklist.
36. Audit current lexer performance constraints implementation and document gaps against v16 targets.
37. Define contract/schema for lexer performance constraints with migration notes and edge cases.
38. Implement lexer performance constraints core refactor with backward-compatible adapter paths.
39. Add automated tests for lexer performance constraints including failure-path coverage.
40. Publish lexer performance constraints documentation and phase acceptance checklist.
41. Audit current html rendering token class alignment implementation and document gaps against v16 targets.
42. Define contract/schema for html rendering token class alignment with migration notes and edge cases.
43. Implement html rendering token class alignment core refactor with backward-compatible adapter paths.
44. Add automated tests for html rendering token class alignment including failure-path coverage.
45. Publish html rendering token class alignment documentation and phase acceptance checklist.
46. Audit current migration plan from legacy token classes implementation and document gaps against v16 targets.
47. Define contract/schema for migration plan from legacy token classes with migration notes and edge cases.
48. Implement migration plan from legacy token classes core refactor with backward-compatible adapter paths.
49. Add automated tests for migration plan from legacy token classes including failure-path coverage.
50. Publish migration plan from legacy token classes documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
