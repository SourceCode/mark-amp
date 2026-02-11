# Phase 16: Code Block Rendering with Syntax Highlighting

## Objective

Implement syntax-highlighted code block rendering in the preview pane. Code blocks must display with proper syntax coloring, language labels, and theme-aware styling matching the reference Preview.tsx code rendering.

## Prerequisites

- Phase 14 (Markdown-to-Rendered-View Pipeline)

## Deliverables

1. **Highest-quality** syntax highlighting engine using **Tree-sitter** grammars
2. Theme-aware code block styling (background, borders, language label) matching React reference EXACTLY
3. Inline code styling matching React reference EXACTLY
4. Copy-to-clipboard button on code blocks
5. Support for **30+ programming languages** at IDE-quality highlighting
6. Token-to-HTML pipeline producing output visually identical to Prism/Shiki (the JS highlighters)

## Tasks

### Task 16.1: Design the syntax highlighting architecture

Create `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.h` and `SyntaxHighlighter.cpp`:

**Approach: Tree-sitter based highlighting (HIGHEST QUALITY)**

**CRITICAL REQUIREMENT**: The syntax highlighting must be the highest quality available -- matching or exceeding VS Code, Zed, and Neovim. This means using **Tree-sitter** grammars, NOT simple regex tokenization.

**Why Tree-sitter:**
- Produces a full AST, not just regex tokens -- this means accurate scope-aware highlighting
- Used by Zed, Neovim, Helix, GitHub.com -- industry standard for quality highlighting
- C library with C++ wrapper -- perfect for our stack
- 200+ language grammars available as pre-built shared libraries
- Incremental parsing -- handles real-time editing at sub-millisecond latency
- Handles edge cases that regex tokenizers miss (nested templates, multi-line strings, heredocs)

**Integration approach:**
- Include `tree-sitter` C library via vcpkg or as a submodule
- Bundle pre-compiled grammar `.so`/`.dylib`/`.dll` files for each supported language
- At build time, embed grammar WASM or compile grammars into the binary
- At runtime, parse code blocks through Tree-sitter, walk the AST, and map node types to highlight scopes

The token classification remains the same for the rendering layer:

```cpp
namespace markamp::core {

enum class TokenType {
    Text,           // Normal text
    Keyword,        // Language keywords (if, else, return, class, etc.)
    String,         // String literals
    Number,         // Numeric literals
    Comment,        // Comments (line and block)
    Operator,       // Operators (+, -, *, etc.)
    Punctuation,    // Braces, brackets, semicolons
    Function,       // Function names
    Type,           // Type names
    Attribute,      // Attributes/decorators
    Tag,            // HTML/XML tags
    Property,       // Object properties
    Variable,       // Variables
    Constant,       // Constants (true, false, null, ALL_CAPS)
    Preprocessor,   // Preprocessor directives (#include, #define)
    Whitespace      // Whitespace (preserved but not colored)
};

struct Token {
    TokenType type;
    std::string_view text;
    size_t start;
    size_t length;
};

struct LanguageDefinition {
    std::string name;
    std::vector<std::string> aliases;       // e.g., "js" -> "javascript"
    std::vector<std::string> keywords;
    std::vector<std::string> types;
    std::vector<std::string> constants;
    std::string line_comment;              // e.g., "//"
    std::string block_comment_start;       // e.g., "/*"
    std::string block_comment_end;         // e.g., "*/"
    std::string string_delimiters;         // e.g., "\"'`"
    bool has_preprocessor{false};
    std::string preprocessor_prefix;       // e.g., "#"
};

class SyntaxHighlighter
{
public:
    SyntaxHighlighter();

    // Register language definitions
    void register_language(LanguageDefinition def);

    // Tokenize source code
    auto tokenize(std::string_view source, const std::string& language)
        -> std::vector<Token>;

    // Render to HTML with <span> tags
    auto render_html(std::string_view source, const std::string& language)
        -> std::string;

    // Check if a language is supported
    [[nodiscard]] auto is_supported(const std::string& language) const -> bool;
    [[nodiscard]] auto supported_languages() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, LanguageDefinition> languages_;
    auto resolve_language(const std::string& name_or_alias) const
        -> const LanguageDefinition*;
    auto tokenize_with_def(std::string_view source, const LanguageDefinition& def)
        -> std::vector<Token>;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- Architecture supports multiple languages
- Language aliases resolve correctly (e.g., "js" -> "javascript")
- Tokenization produces correct token types
- Unsupported languages fall back to plain text (no crash)

### Task 16.2: Bundle Tree-sitter language grammars

Bundle pre-compiled Tree-sitter grammars for **30+ languages**:

**Tier 1 (must have, most used):**
1. **JavaScript** (js, jsx)
2. **TypeScript** (ts, tsx)
3. **Python** (py, python)
4. **C** (c, h)
5. **C++** (cpp, cxx, hpp, cc)
6. **Rust** (rs, rust)
7. **Go** (go, golang)
8. **Java** (java)
9. **C#** (cs, csharp)
10. **HTML** (html, htm)
11. **CSS** (css, scss, less)
12. **JSON** (json, jsonc)
13. **YAML** (yaml, yml)
14. **SQL** (sql)
15. **Bash/Shell** (bash, sh, shell, zsh)

**Tier 2 (important, commonly encountered):**
16. **Markdown** (md, markdown)
17. **XML** (xml, svg)
18. **Ruby** (rb, ruby)
19. **PHP** (php)
20. **Swift** (swift)
21. **Kotlin** (kt, kotlin)
22. **Lua** (lua)
23. **Haskell** (hs, haskell)
24. **Elixir** (ex, exs, elixir)
25. **Zig** (zig)
26. **TOML** (toml)
27. **Dockerfile** (dockerfile)
28. **Makefile** (makefile)
29. **Protobuf** (proto)
30. **GraphQL** (graphql, gql)

**Tier 3 (bonus):**
31. **Scala** (scala)
32. **R** (r)
33. **Dart** (dart)
34. **OCaml** (ml, ocaml)
35. **Vim** (vim)

**Build integration:**
- Add Tree-sitter grammars as vcpkg ports or git submodules
- Compile each grammar into a static library at build time
- Register all grammars in a `LanguageRegistry` singleton
- Map file extensions and markdown fence info strings to grammar names

**Acceptance criteria:**
- All 30 Tier 1+2 languages are bundled and registered
- Aliases resolve correctly (e.g., "js" -> JavaScript, "py" -> Python)
- Tree-sitter successfully parses sample code for each language
- Grammar loading adds < 50ms to application startup
- Total grammar binary size is documented (target: < 20MB)

### Task 16.3: Implement the tokenizer

Implement `SyntaxHighlighter::tokenize_with_def()`:

**Tokenization algorithm:**
1. Scan character by character through the source
2. At each position, check (in priority order):
   a. Block comment start -> consume until block comment end
   b. Line comment start -> consume until end of line
   c. String delimiter -> consume until matching delimiter (handle escape `\`)
   d. Preprocessor prefix at start of line -> consume entire line
   e. Number literal start (digit, or `.` followed by digit) -> consume number
   f. Identifier start (letter or `_`) -> consume identifier, classify:
      - If in keywords list -> `Keyword`
      - If in types list -> `Type`
      - If in constants list -> `Constant`
      - If followed by `(` -> `Function`
      - Else -> `Text` (or `Variable`)
   g. Operator characters -> `Operator`
   h. Punctuation -> `Punctuation`
   i. Whitespace -> `Whitespace`
   j. Anything else -> `Text`

**Acceptance criteria:**
- Tokenizer correctly identifies all token types
- String literals handle escape sequences
- Block comments can span multiple lines
- Nested strings in comments are not tokenized
- Performance: tokenizes 10,000 lines in under 100ms

### Task 16.4: Implement HTML rendering for code blocks

Create `/Users/ryanrentfro/code/markamp/src/rendering/CodeBlockRenderer.h` and `CodeBlockRenderer.cpp`:

**Code block structure (matching reference Preview.tsx lines 24-39):**
```html
<div class="code-block-wrapper">
  <div class="code-block-header">
    <span class="language-label">{language}</span>
  </div>
  <pre class="code-block">
    <code class="language-{lang}">
      <span class="token-keyword">function</span>
      <span class="token-text"> </span>
      <span class="token-function">helloWorld</span>
      ...
    </code>
  </pre>
</div>
```

**Styling (matching reference):**
- Wrapper: `position: relative;`
- Language label: `position: absolute; right: 8px; top: 8px; font-size: 12px; color: {text_muted}; opacity: 0.5; font-family: monospace;`
- pre: `background: rgba(0,0,0,0.3); padding: 16px; border-radius: 4px; border: 1px solid {border_light}; overflow-x: auto;`
- code: `font-family: 'JetBrains Mono', monospace;`

**Token colors (theme-derived):**
| Token Type | Color |
|---|---|
| Keyword | accent_primary |
| String | accent_secondary (or a computed warm color) |
| Number | accent_secondary |
| Comment | text_muted at 60% |
| Operator | text_main |
| Function | accent_primary at 80% |
| Type | accent_secondary |
| Constant | accent_primary |
| Preprocessor | text_muted |
| Tag | accent_primary |
| Property | accent_secondary |
| Variable | text_main |
| Punctuation | text_muted |
| Text | text_main |

**Acceptance criteria:**
- Code blocks render with syntax highlighting
- Language label appears in the top-right corner
- Background is semi-transparent dark
- Border matches theme
- Token colors are theme-derived (update on theme change)

### Task 16.5: Implement inline code styling

**Inline code (matching reference line 36-38):**
```html
<code class="inline-code">text</code>
```

**Styling:**
- Background: accent_primary at 20% opacity
- Text color: accent_primary
- Padding: 2px 6px (px-1.5 py-0.5)
- Border radius: 4px
- Font: monospace, slightly smaller than body text

**Acceptance criteria:**
- Inline code is visually distinct from surrounding text
- Background and text use accent color
- Rounded corners are visible

### Task 16.6: Implement copy-to-clipboard button (optional enhancement)

Add a "copy" button to code blocks:

- Position: top-right corner, next to or replacing the language label on hover
- Icon: clipboard icon
- Behavior: copies code content to system clipboard
- Feedback: button changes to "Copied!" for 2 seconds after click

**Implementation with wxHtmlWindow:**
This may require using wxWebView instead, or implementing a custom overlay.

**Alternative:** Skip the copy button for wxHtmlWindow backend and add it if/when upgrading to wxWebView.

**Acceptance criteria:**
- If implemented: copy button appears on hover, copies code to clipboard
- If deferred: document the enhancement for future implementation

### Task 16.7: Handle code blocks without language specification

When a fenced code block has no language specified (` ``` ` with no info string):

- Render as plain text with code block styling
- No syntax highlighting
- No language label
- Same background and border treatment

Also handle indented code blocks (4 spaces or 1 tab indentation):
- Same as fenced code blocks with no language

**Acceptance criteria:**
- Language-less code blocks render without highlighting
- Indented code blocks render as code
- No errors for empty code blocks

### Task 16.8: Write syntax highlighting tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_syntax_highlighter.cpp`:

Test cases:
1. JavaScript: function declaration with keywords, strings, comments
2. Python: class definition with decorators, docstrings
3. C++: template function with preprocessor directives
4. HTML: tags with attributes
5. CSS: selectors with properties and values
6. JSON: nested object with strings and numbers
7. Bash: command with pipes and variables
8. Unknown language: falls back to plain text
9. Empty code block
10. Code with mixed token types
11. Multi-line string handling
12. Nested comments (if applicable)
13. Language alias resolution (js -> javascript)
14. Token position accuracy

**Acceptance criteria:**
- All tests pass
- Token types are verified for each test case
- HTML output is well-formed
- At least 20 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/SyntaxHighlighter.h` | Created |
| `src/core/SyntaxHighlighter.cpp` | Created |
| `src/rendering/CodeBlockRenderer.h` | Created |
| `src/rendering/CodeBlockRenderer.cpp` | Created |
| `src/rendering/HtmlRenderer.cpp` | Modified (use CodeBlockRenderer) |
| `src/ui/PreviewPanel.cpp` | Modified (CSS for code blocks) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_syntax_highlighter.cpp` | Created |

## Dependencies

- Phase 14 rendering pipeline
- Phase 08 theme engine (for token colors)

## Estimated Complexity

**High** -- Building a multi-language syntax highlighter, even a simple token-based one, is significant work. Defining correct keyword lists and handling all the edge cases of string/comment parsing across 15 languages requires careful implementation. The visual styling must also match the reference closely.
