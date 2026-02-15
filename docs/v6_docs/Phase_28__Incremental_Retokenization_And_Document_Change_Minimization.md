# Phase 28: Incremental Re-tokenization & Document Change Minimization

## Metadata

| Field | Value |
|---|---|
| Phase ID | 28 |
| Prerequisites | Phase 22 |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 5 modified |
| PRD Sections | 4.5 Rendering Performance |

---

## Objective

Ensure single-line edits only re-tokenize the affected line(s), not the entire document. Implement incremental parsing for the markdown parser as well.

---

## Background

The PRD mandates: "Never re-tokenize entire document on single-line changes." Currently, the SyntaxHighlighter may re-tokenize the entire document or large sections on every edit. For large documents (10K+ lines), this is a significant frame time contributor. Similarly, the MarkdownParser may re-parse the entire document for structural changes.

---

## Scope

### Tasks

1. **Implement incremental tokenization in SyntaxHighlighter**:
   - Add method: `tokenize_range(start_line, end_line, delta_lines)`
     - `start_line`: first modified line
     - `end_line`: last modified line
     - `delta_lines`: number of lines added (positive) or removed (negative)
   - Algorithm:
     1. Identify the tokenizer state at `start_line` (cached per-line state)
     2. Re-tokenize from `start_line` to `end_line + 1`
     3. Compare end state with cached state at `end_line + 1`
     4. If states match: done (change is contained)
     5. If states differ: extend re-tokenization forward until states converge
   - Maintain per-line tokenizer state cache for fast restart
   - Merge new tokens into existing token array, shifting offsets for lines after the edit

2. **Implement incremental parsing in MarkdownParser**:
   - For single-block edits (typing within a paragraph):
     - Re-parse only the affected block
     - Keep the AST structure for surrounding blocks
   - For structural changes (adding/removing headers, code blocks):
     - Fall back to full re-parse (these are rare during typing)
   - Detect change scope: compare edit range against block boundaries
   - Block boundaries: headers, blank lines, code fence markers, list markers

3. **Wire LineIndex change notifications**:
   - EditorPanel sends edit notification: { line_range, delta_lines }
   - SyntaxHighlighter receives notification and calls `tokenize_range()`
   - MarkdownParser receives notification and re-parses affected blocks

4. **Create `benchmarks/bench_incremental_tokenization.cpp`**:
   - `BM_Tokenize_Full_SmallDoc`: full tokenization baseline (100 lines)
   - `BM_Tokenize_Full_LargeDoc`: full tokenization baseline (10K lines)
   - `BM_Tokenize_Incremental_SingleLine`: single-line change on 10K doc
   - `BM_Tokenize_Incremental_MultiLine`: 5-line change on 10K doc
   - `BM_Parse_Full_LargeDoc`: full parse baseline
   - `BM_Parse_Incremental_SingleBlock`: single block change

5. **Create `tests/unit/test_incremental_tokenization.cpp`**:
   - Test single-character edit on line N only re-tokenizes line N
   - Test line insertion correctly shifts subsequent tokens
   - Test line deletion correctly merges token arrays
   - Test state-mismatch extension (edit changes context, e.g., opening a code block)
   - Test that incremental and full tokenization produce identical results
   - Test markdown incremental parse matches full parse

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/SyntaxHighlighter.h` |
| Modify | `src/core/SyntaxHighlighter.cpp` |
| Modify | `src/core/MarkdownParser.cpp` |
| Create | `benchmarks/bench_incremental_tokenization.cpp` |
| Create | `tests/unit/test_incremental_tokenization.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- Per-line state cache: store the tokenizer state (current scope stack, open delimiters) at the end of each line. When re-tokenizing from line N, restore state from line N-1's cache.
- State convergence: after re-tokenizing the edited region, if the end state matches the cached state of the next line, the change is contained. Otherwise, continue re-tokenizing until convergence (usually within 1-3 extra lines for most edits).
- For markdown parsing, `md4c` (the C markdown library used) may not support incremental parsing natively. In that case, re-parse the full document but only update the affected blocks in the AST.
- The key metric is: single-character edit should touch at most 3 lines of tokens (edited line + 1 neighbor on each side for context).

---

## Acceptance Criteria

- [ ] Single-character edit re-tokenizes at most 3 lines (edited + neighbors)
- [ ] `bench_incremental_tokenization` shows >10x speedup for single-line edits vs full tokenization
- [ ] Full-document tokenization still works as fallback
- [ ] `test_incremental_tokenization` validates correctness for all edge cases
- [ ] Incremental and full tokenization produce identical results
- [ ] No visual highlighting regressions (colors match)
- [ ] Markdown incremental parse correctly handles block boundary changes

---

## Testing Strategy

- Run test_incremental_tokenization with comprehensive edge cases
- Run bench_incremental_tokenization and verify speedup targets
- Visual comparison: tokenize document fully, then incrementally, compare results
- Run full test suite for regression checking
- Test with various file types: Markdown, C++, Python, plain text
