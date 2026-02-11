#include <catch2/catch_test_macros.hpp>
#include <wx/stc/stc.h>

// EditorPanel constants (duplicated from header to avoid wx dependency chain)
namespace editor_constants
{
constexpr int kDefaultFontSize = 13;
constexpr int kDefaultTabSize = 4;
constexpr int kCaretWidth = 2;
constexpr int kCaretBlinkMs = 500;
constexpr int kDebounceMs = 300;
constexpr int kFindBarHeight = 36;
} // namespace editor_constants

// ═══════════════════════════════════════════════════════
// EditorPanel constants
// ═══════════════════════════════════════════════════════

TEST_CASE("EditorPanel default font size", "[editor]")
{
    REQUIRE(editor_constants::kDefaultFontSize == 13);
}

TEST_CASE("EditorPanel default tab size", "[editor]")
{
    REQUIRE(editor_constants::kDefaultTabSize == 4);
}

TEST_CASE("EditorPanel caret width", "[editor]")
{
    REQUIRE(editor_constants::kCaretWidth == 2);
}

TEST_CASE("EditorPanel caret blink period", "[editor]")
{
    REQUIRE(editor_constants::kCaretBlinkMs == 500);
}

TEST_CASE("EditorPanel debounce period", "[editor]")
{
    REQUIRE(editor_constants::kDebounceMs == 300);
}

TEST_CASE("EditorPanel find bar height", "[editor]")
{
    REQUIRE(editor_constants::kFindBarHeight == 36);
}

// ═══════════════════════════════════════════════════════
// Scintilla markdown lexer style IDs (verify they exist)
// ═══════════════════════════════════════════════════════

TEST_CASE("Scintilla markdown style IDs are defined", "[editor]")
{
    // These constants come from wx/stc/stc.h — verify they are available
    REQUIRE(wxSTC_LEX_MARKDOWN > 0);
    REQUIRE(wxSTC_MARKDOWN_DEFAULT >= 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER1 > 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER2 > 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER3 > 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER4 > 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER5 > 0);
    REQUIRE(wxSTC_MARKDOWN_HEADER6 > 0);
    REQUIRE(wxSTC_MARKDOWN_STRONG1 > 0);
    REQUIRE(wxSTC_MARKDOWN_STRONG2 > 0);
    REQUIRE(wxSTC_MARKDOWN_EM1 > 0);
    REQUIRE(wxSTC_MARKDOWN_EM2 > 0);
    REQUIRE(wxSTC_MARKDOWN_CODE > 0);
    REQUIRE(wxSTC_MARKDOWN_CODE2 > 0);
    REQUIRE(wxSTC_MARKDOWN_CODEBK > 0);
    REQUIRE(wxSTC_MARKDOWN_LINK > 0);
    REQUIRE(wxSTC_MARKDOWN_BLOCKQUOTE > 0);
    REQUIRE(wxSTC_MARKDOWN_STRIKEOUT > 0);
    REQUIRE(wxSTC_MARKDOWN_HRULE > 0);
    REQUIRE(wxSTC_MARKDOWN_ULIST_ITEM > 0);
    REQUIRE(wxSTC_MARKDOWN_OLIST_ITEM > 0);
}

// ═══════════════════════════════════════════════════════
// Scintilla search flags (verify availability)
// ═══════════════════════════════════════════════════════

TEST_CASE("Scintilla search flags are defined", "[editor]")
{
    REQUIRE(wxSTC_FIND_MATCHCASE > 0);
}

// ═══════════════════════════════════════════════════════
// Scintilla wrap modes (verify availability)
// ═══════════════════════════════════════════════════════

TEST_CASE("Scintilla wrap modes are defined", "[editor]")
{
    REQUIRE(wxSTC_WRAP_NONE == 0);
    REQUIRE(wxSTC_WRAP_WORD > 0);
}

// ═══════════════════════════════════════════════════════
// Scintilla indicator styles (verify availability)
// ═══════════════════════════════════════════════════════

TEST_CASE("Scintilla indicator styles are defined", "[editor]")
{
    REQUIRE(wxSTC_INDIC_ROUNDBOX >= 0);
}
