# Phase 13: Markdown Parsing Engine Integration

## Objective

Integrate the md4c library for parsing GitHub Flavored Markdown (GFM) into a structured document model. Create a C++ wrapper around the C library that provides a clean, type-safe API for converting markdown text into a renderable document tree.

## Prerequisites

- Phase 03 (Dependency Management with vcpkg)
- Phase 05 (Application Architecture and Module Design)

## Deliverables

1. C++ wrapper around md4c with RAII and type-safe callbacks
2. MarkdownDocument AST (abstract syntax tree) data model
3. IMarkdownParser implementation
4. Support for all GFM features: headings, emphasis, lists, blockquotes, tables, code blocks, task lists, links, images, horizontal rules
5. Mermaid code block detection and extraction
6. Unit tests for all markdown elements

## Tasks

### Task 13.1: Create the MarkdownDocument data model

Create `/Users/ryanrentfro/code/markamp/src/core/MarkdownDocument.h`:

An AST representing the parsed markdown:

```cpp
namespace markamp::core {

enum class MdNodeType {
    // Block-level
    Document, Paragraph, Heading, BlockQuote,
    UnorderedList, OrderedList, ListItem,
    CodeBlock, FencedCodeBlock,
    HorizontalRule, Table, TableRow, TableCell,
    HtmlBlock,

    // Inline-level
    Text, Emphasis, Strong, StrongEmphasis,
    Code, Link, Image, LineBreak, SoftBreak,
    HtmlInline, TaskListMarker, Strikethrough,

    // Custom
    MermaidBlock  // Fenced code block with language "mermaid"
};

enum class MdAlignment { Default, Left, Center, Right };

struct MdNode {
    MdNodeType type;
    std::string text_content;  // For leaf nodes (Text, Code, etc.)

    // Heading
    int heading_level{0};     // 1-6

    // Code block
    std::string language;      // "javascript", "mermaid", "cpp", etc.
    std::string info_string;   // Full info string after ```

    // Link / Image
    std::string url;
    std::string title;

    // List
    bool is_tight{false};
    int start_number{1};       // For ordered lists

    // Table cell
    MdAlignment alignment{MdAlignment::Default};
    bool is_header{false};

    // Task list
    bool is_checked{false};

    // Children
    std::vector<MdNode> children;

    // Helpers
    [[nodiscard]] auto is_block() const -> bool;
    [[nodiscard]] auto is_inline() const -> bool;
    [[nodiscard]] auto plain_text() const -> std::string;  // Recursive text extraction
    [[nodiscard]] auto find_all(MdNodeType type) const -> std::vector<const MdNode*>;
};

struct MarkdownDocument {
    MdNode root;
    std::vector<std::string> mermaid_blocks;   // Extracted mermaid sources
    std::vector<std::string> code_languages;   // Unique languages used

    [[nodiscard]] auto heading_count() const -> size_t;
    [[nodiscard]] auto word_count() const -> size_t;
    [[nodiscard]] auto has_mermaid() const -> bool;
    [[nodiscard]] auto has_tables() const -> bool;
    [[nodiscard]] auto has_task_lists() const -> bool;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- All markdown element types are represented
- AST can be traversed recursively
- Helper functions work correctly
- Mermaid blocks are separately tracked

### Task 13.2: Create the md4c C++ wrapper

Create `/Users/ryanrentfro/code/markamp/src/core/Md4cWrapper.h` and `Md4cWrapper.cpp`:

Wrap the md4c C callback API in a C++ builder pattern:

```cpp
namespace markamp::core {

class Md4cParser
{
public:
    Md4cParser();

    // Configure parser flags
    Md4cParser& enable_tables(bool on = true);
    Md4cParser& enable_task_lists(bool on = true);
    Md4cParser& enable_strikethrough(bool on = true);
    Md4cParser& enable_autolinks(bool on = true);
    Md4cParser& enable_permissive_url_autolinks(bool on = true);

    // Parse markdown to AST
    auto parse(std::string_view markdown) -> std::expected<MarkdownDocument, std::string>;

private:
    unsigned parser_flags_{0};

    // md4c callback trampolines (static functions that forward to instance methods)
    static int enter_block_callback(MD_BLOCKTYPE type, void* detail, void* userdata);
    static int leave_block_callback(MD_BLOCKTYPE type, void* detail, void* userdata);
    static int enter_span_callback(MD_SPANTYPE type, void* detail, void* userdata);
    static int leave_span_callback(MD_SPANTYPE type, void* detail, void* userdata);
    static int text_callback(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);

    // Parser state (used during a single parse call)
    struct ParseState {
        MarkdownDocument document;
        std::vector<MdNode*> node_stack;  // Stack for building the tree
        std::string current_code_block;
        std::string current_code_language;
        bool in_code_block{false};
    };

    void on_enter_block(ParseState& state, MD_BLOCKTYPE type, void* detail);
    void on_leave_block(ParseState& state, MD_BLOCKTYPE type, void* detail);
    void on_enter_span(ParseState& state, MD_SPANTYPE type, void* detail);
    void on_leave_span(ParseState& state, MD_SPANTYPE type, void* detail);
    void on_text(ParseState& state, MD_TEXTTYPE type, const char* text, size_t size);
};

} // namespace markamp::core
```

**md4c flags to enable:**
- `MD_FLAG_TABLES` -- GFM tables
- `MD_FLAG_TASKLISTS` -- `- [ ]` / `- [x]` task lists
- `MD_FLAG_STRIKETHROUGH` -- `~~text~~`
- `MD_FLAG_PERMISSIVEAUTOLINKS` -- auto-link URLs
- `MD_FLAG_NOHTML` -- disable raw HTML (security, can be toggled)

**Acceptance criteria:**
- md4c is called correctly via its C API
- Callbacks build a correct AST
- All GFM features are parsed
- Parser returns errors for invalid input instead of crashing
- Memory safety: no buffer overflows, no dangling pointers in callbacks

### Task 13.3: Implement the IMarkdownParser interface

Create `/Users/ryanrentfro/code/markamp/src/core/MarkdownParser.h` and `MarkdownParser.cpp`:

```cpp
namespace markamp::core {

class MarkdownParser : public IMarkdownParser
{
public:
    MarkdownParser();

    auto parse(std::string_view markdown) -> std::expected<MarkdownDocument, std::string> override;
    auto render_html(const MarkdownDocument& doc) -> std::string override;

private:
    Md4cParser parser_;
};

} // namespace markamp::core
```

The `render_html()` method converts the AST back to HTML for the preview pane. However, this will be more fully developed in Phase 14. For now, implement a basic HTML renderer that handles the core elements.

**Acceptance criteria:**
- Parser implements the IMarkdownParser interface
- Parse + render_html round-trip produces valid HTML
- Register in ServiceRegistry in MarkAmpApp

### Task 13.4: Implement mermaid block detection

During parsing, detect fenced code blocks with language "mermaid":

1. When a fenced code block is entered with info string "mermaid":
   - Create an `MdNode` with type `MermaidBlock`
   - Collect all text content into the node
2. When the code block is complete:
   - Add the mermaid source to `MarkdownDocument::mermaid_blocks`
   - The node remains in the AST at its position

This matches the reference Preview.tsx behavior where mermaid code blocks are detected by `language-mermaid` class and routed to `MermaidBlock` component.

**Acceptance criteria:**
- Mermaid code blocks are correctly identified
- Mermaid source is extracted into `mermaid_blocks` vector
- Non-mermaid code blocks are unaffected
- Multiple mermaid blocks in one document are all detected

### Task 13.5: Implement basic HTML rendering from AST

Create `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.h` and `HtmlRenderer.cpp`:

Traverse the AST and emit HTML:

```cpp
namespace markamp::rendering {

class HtmlRenderer
{
public:
    auto render(const core::MarkdownDocument& doc) -> std::string;

private:
    void render_node(const core::MdNode& node, std::string& output);
    void render_children(const core::MdNode& node, std::string& output);

    // Helpers
    static auto escape_html(std::string_view text) -> std::string;
    static auto alignment_style(core::MdAlignment align) -> std::string;
};

} // namespace markamp::rendering
```

**HTML output mapping:**
| MdNodeType | HTML Output |
|---|---|
| Heading(1-6) | `<h1>` through `<h6>` |
| Paragraph | `<p>` |
| Emphasis | `<em>` |
| Strong | `<strong>` |
| Code | `<code>` |
| CodeBlock | `<pre><code class="language-xxx">` |
| MermaidBlock | `<div class="mermaid-block" data-chart="...">` |
| Link | `<a href="..." title="...">` |
| Image | `<img src="..." alt="..." title="...">` |
| UnorderedList | `<ul>` |
| OrderedList | `<ol start="...">` |
| ListItem | `<li>` |
| TaskListMarker | `<input type="checkbox" checked/disabled>` |
| BlockQuote | `<blockquote>` |
| Table | `<table>` |
| TableRow | `<tr>` |
| TableCell | `<th>` or `<td>` with alignment |
| HorizontalRule | `<hr>` |
| Strikethrough | `<del>` |
| LineBreak | `<br>` |

**HTML escaping:** Escape `<`, `>`, `&`, `"`, `'` in text content.

**Acceptance criteria:**
- All node types produce correct HTML
- HTML is well-formed (all tags properly opened and closed)
- Special characters are escaped
- Code block language is preserved in class attribute
- Table alignment is applied as inline styles

### Task 13.6: Handle edge cases and special content

Test and handle these markdown edge cases:

1. **Empty document**: produces empty HTML
2. **Only whitespace**: produces empty HTML
3. **Deeply nested lists** (5+ levels): correctly nested HTML
4. **Mixed list types**: ordered inside unordered and vice versa
5. **Code blocks with backticks in content**: correctly escaped
6. **Links with special characters in URLs**: URL-encoded
7. **Images with no alt text**: empty alt attribute
8. **Tables with varying column counts**: handled gracefully
9. **Task list items with markdown content**: task checkbox + formatted text
10. **Multiple consecutive blank lines**: collapsed to single paragraph break

**Acceptance criteria:**
- All edge cases produce correct, non-crashing output
- Malformed markdown degrades gracefully (no crashes)

### Task 13.7: Write comprehensive parser tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_markdown_parser.cpp`:

Test cases for every markdown element:
1. Plain paragraph text
2. Headings (H1 through H6, ATX style)
3. Emphasis (italic with `*` and `_`)
4. Strong (bold with `**` and `__`)
5. Strong emphasis (bold italic)
6. Inline code
7. Fenced code block (with and without language)
8. Mermaid code block detection
9. Unordered list (single level)
10. Ordered list (with start number)
11. Nested lists
12. Task list
13. Link (inline, reference, autolink)
14. Image
15. Block quote (single and nested)
16. Table (with alignment)
17. Horizontal rule
18. Strikethrough
19. Line break (trailing spaces, `<br>`)
20. HTML rendering of each element
21. Full document from reference sample files

**Acceptance criteria:**
- All 21+ test cases pass
- Each test verifies both the AST structure and the HTML output
- Reference sample files (README.md, architecture.md, sequence.md, TODO.md) parse correctly
- At least 40 test assertions total

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/MarkdownDocument.h` | Created |
| `src/core/Md4cWrapper.h` | Created |
| `src/core/Md4cWrapper.cpp` | Created |
| `src/core/MarkdownParser.h` | Created |
| `src/core/MarkdownParser.cpp` | Created |
| `src/rendering/HtmlRenderer.h` | Created |
| `src/rendering/HtmlRenderer.cpp` | Created |
| `src/app/MarkAmpApp.cpp` | Modified (register parser service) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_markdown_parser.cpp` | Created |

## Dependencies

- md4c (via vcpkg)
- Phase 05 IMarkdownParser interface
- std::expected

## Estimated Complexity

**High** -- The md4c C callback API is non-trivial to wrap safely in C++. Building a correct AST from nested enter/leave callbacks requires careful stack management. Handling all GFM edge cases is extensive work.
