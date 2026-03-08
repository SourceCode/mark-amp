# Markdown Pipeline Test Cases

Test cases derived from **v1 Phases 13-17**: Markdown Parsing, GFM Extensions, Code Blocks, Mermaid Diagrams.

## TC-MD-001: Markdown Parsing

| ID      | Test Case                     | Unit                          | E2E | Regression |
| ------- | ----------------------------- | ----------------------------- | --- | ---------- |
| MD-001a | Parse headings (h1-h6)        | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001b | Parse emphasis (bold, italic) | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001c | Parse ordered/unordered lists | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001d | Parse blockquotes             | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001e | Parse links and images        | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001f | Parse inline code             | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-001g | Parse horizontal rules        | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |

## TC-MD-002: GFM Extensions

| ID      | Test Case               | Unit                          | E2E | Regression |
| ------- | ----------------------- | ----------------------------- | --- | ---------- |
| MD-002a | Tables render correctly | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-002b | Task lists (checkboxes) | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-002c | Footnotes               | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-002d | Strikethrough text      | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |
| MD-002e | Autolinks               | ✅ `test_markdown_parser.cpp` | ❌  | ✅         |

## TC-MD-003: Code Blocks with Syntax Highlighting

| ID      | Test Case                       | Unit                              | E2E | Regression |
| ------- | ------------------------------- | --------------------------------- | --- | ---------- |
| MD-003a | Fenced code block renders       | ✅ `test_code_block_renderer.cpp` | ❌  | ✅         |
| MD-003b | Language-specific highlighting  | ✅ `test_code_block_renderer.cpp` | ❌  | ✅         |
| MD-003c | No-language code block fallback | ✅ `test_code_block_renderer.cpp` | ❌  | ✅         |
| MD-003d | Copy button on code blocks      | ➖                                | ❌  | ❌         |

## TC-MD-004: Mermaid Diagrams

| ID      | Test Case                          | Unit                           | E2E | Regression |
| ------- | ---------------------------------- | ------------------------------ | --- | ---------- |
| MD-004a | Mermaid block renders diagram      | ✅ `test_mermaid_renderer.cpp` | ❌  | ❌         |
| MD-004b | Invalid mermaid syntax → error msg | ✅ `test_mermaid_renderer.cpp` | ❌  | ❌         |
| MD-004c | Theme-aware mermaid rendering      | ✅ `test_mermaid_renderer.cpp` | ❌  | ❌         |

## Coverage Summary

| Category    | Unit      | E2E      | Regression |
| ----------- | --------- | -------- | ---------- |
| Parsing     | 7/7 ✅    | 0/7 ❌   | 7/7 ✅     |
| GFM         | 5/5 ✅    | 0/5 ❌   | 5/5 ✅     |
| Code Blocks | 3/4 ✅    | 0/4 ❌   | 3/4 ✅     |
| Mermaid     | 3/3 ✅    | 0/3 ❌   | 0/3 ❌     |
| **Total**   | **18/19** | **0/19** | **15/19**  |
