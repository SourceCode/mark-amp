#include <catch2/catch_test_macros.hpp>
#include <wx/defs.h>
#include <wx/stc/stc.h>

// ═══════════════════════════════════════════════════════
// Scintilla command constants — verify that all standard
// editor operations are available via wxStyledTextCtrl.
// The editor (EditorPanel) wraps wxStyledTextCtrl which
// handles these commands natively via Scintilla keymaps.
// ═══════════════════════════════════════════════════════

// --- Clipboard operations ---

TEST_CASE("STC clipboard commands are defined", "[editor_controls][clipboard]")
{
    SECTION("copy")
    {
        REQUIRE(wxSTC_CMD_COPY >= 0);
    }

    SECTION("cut")
    {
        REQUIRE(wxSTC_CMD_CUT >= 0);
    }

    SECTION("paste")
    {
        REQUIRE(wxSTC_CMD_PASTE >= 0);
    }
}

// --- Undo / Redo ---

TEST_CASE("STC undo/redo commands are defined", "[editor_controls][undo]")
{
    SECTION("undo")
    {
        REQUIRE(wxSTC_CMD_UNDO >= 0);
    }

    SECTION("redo")
    {
        REQUIRE(wxSTC_CMD_REDO >= 0);
    }
}

// --- Selection ---

TEST_CASE("STC selection commands are defined", "[editor_controls][selection]")
{
    SECTION("select all")
    {
        REQUIRE(wxSTC_CMD_SELECTALL >= 0);
    }
}

// --- Cursor movement: Home / End ---

TEST_CASE("STC Home/End commands are defined", "[editor_controls][navigation]")
{
    SECTION("home — move to start of line")
    {
        REQUIRE(wxSTC_CMD_HOME >= 0);
    }

    SECTION("vchome — move to first non-blank on line")
    {
        REQUIRE(wxSTC_CMD_VCHOME >= 0);
    }

    SECTION("lineend — move to end of line")
    {
        REQUIRE(wxSTC_CMD_LINEEND >= 0);
    }

    SECTION("home extend — select to start of line")
    {
        REQUIRE(wxSTC_CMD_HOMEEXTEND >= 0);
    }

    SECTION("vchome extend — select to first non-blank")
    {
        REQUIRE(wxSTC_CMD_VCHOMEEXTEND >= 0);
    }

    SECTION("lineend extend — select to end of line")
    {
        REQUIRE(wxSTC_CMD_LINEENDEXTEND >= 0);
    }

    SECTION("document start — Cmd+Home / Ctrl+Home")
    {
        REQUIRE(wxSTC_CMD_DOCUMENTSTART >= 0);
    }

    SECTION("document end — Cmd+End / Ctrl+End")
    {
        REQUIRE(wxSTC_CMD_DOCUMENTEND >= 0);
    }

    SECTION("document start extend — Cmd+Shift+Home")
    {
        REQUIRE(wxSTC_CMD_DOCUMENTSTARTEXTEND >= 0);
    }

    SECTION("document end extend — Cmd+Shift+End")
    {
        REQUIRE(wxSTC_CMD_DOCUMENTENDEXTEND >= 0);
    }
}

// --- Cursor movement: Page Up / Page Down ---

TEST_CASE("STC PageUp/PageDown commands are defined", "[editor_controls][navigation]")
{
    SECTION("page up")
    {
        REQUIRE(wxSTC_CMD_PAGEUP >= 0);
    }

    SECTION("page down")
    {
        REQUIRE(wxSTC_CMD_PAGEDOWN >= 0);
    }

    SECTION("page up extend — Shift+PageUp")
    {
        REQUIRE(wxSTC_CMD_PAGEUPRECTEXTEND >= 0);
    }

    SECTION("page down extend — Shift+PageDown")
    {
        REQUIRE(wxSTC_CMD_PAGEDOWNRECTEXTEND >= 0);
    }
}

// --- Delete operations ---

TEST_CASE("STC delete commands are defined", "[editor_controls][delete]")
{
    SECTION("clear — Delete key")
    {
        REQUIRE(wxSTC_CMD_CLEAR >= 0);
    }

    SECTION("delete back — Backspace")
    {
        REQUIRE(wxSTC_CMD_DELETEBACK >= 0);
    }

    SECTION("delete back not line — Backspace but not past line start")
    {
        REQUIRE(wxSTC_CMD_DELETEBACKNOTLINE >= 0);
    }

    SECTION("delete word left — Option+Backspace / Ctrl+Backspace")
    {
        REQUIRE(wxSTC_CMD_DELWORDLEFT >= 0);
    }

    SECTION("delete word right — Option+Delete / Ctrl+Delete")
    {
        REQUIRE(wxSTC_CMD_DELWORDRIGHT >= 0);
    }

    SECTION("delete line — Cmd+Shift+K / Ctrl+Shift+K")
    {
        REQUIRE(wxSTC_CMD_LINEDELETE >= 0);
    }

    SECTION("line cut — cut entire line")
    {
        REQUIRE(wxSTC_CMD_LINECUT >= 0);
    }

    SECTION("line copy — copy entire line")
    {
        REQUIRE(wxSTC_CMD_LINECOPY >= 0);
    }
}

// --- Word navigation ---

TEST_CASE("STC word navigation commands are defined", "[editor_controls][navigation]")
{
    SECTION("word left — Option+Left / Ctrl+Left")
    {
        REQUIRE(wxSTC_CMD_WORDLEFT >= 0);
    }

    SECTION("word right — Option+Right / Ctrl+Right")
    {
        REQUIRE(wxSTC_CMD_WORDRIGHT >= 0);
    }

    SECTION("word left extend — Option+Shift+Left")
    {
        REQUIRE(wxSTC_CMD_WORDLEFTEXTEND >= 0);
    }

    SECTION("word right extend — Option+Shift+Right")
    {
        REQUIRE(wxSTC_CMD_WORDRIGHTEXTEND >= 0);
    }
}

// --- Line operations ---

TEST_CASE("STC line manipulation commands are defined", "[editor_controls][lines]")
{
    SECTION("line duplicate")
    {
        REQUIRE(wxSTC_CMD_LINEDUPLICATE >= 0);
    }

    SECTION("line transpose — swap with line above")
    {
        REQUIRE(wxSTC_CMD_LINETRANSPOSE >= 0);
    }

    SECTION("line up — move cursor up one line")
    {
        REQUIRE(wxSTC_CMD_LINEUP >= 0);
    }

    SECTION("line down — move cursor down one line")
    {
        REQUIRE(wxSTC_CMD_LINEDOWN >= 0);
    }

    SECTION("line up extend — Shift+Up")
    {
        REQUIRE(wxSTC_CMD_LINEUPEXTEND >= 0);
    }

    SECTION("line down extend — Shift+Down")
    {
        REQUIRE(wxSTC_CMD_LINEDOWNEXTEND >= 0);
    }

    SECTION("char left")
    {
        REQUIRE(wxSTC_CMD_CHARLEFT >= 0);
    }

    SECTION("char right")
    {
        REQUIRE(wxSTC_CMD_CHARRIGHT >= 0);
    }

    SECTION("char left extend — Shift+Left")
    {
        REQUIRE(wxSTC_CMD_CHARLEFTEXTEND >= 0);
    }

    SECTION("char right extend — Shift+Right")
    {
        REQUIRE(wxSTC_CMD_CHARRIGHTEXTEND >= 0);
    }
}

// --- Tab / Indent ---

TEST_CASE("STC indent commands are defined", "[editor_controls][indent]")
{
    SECTION("tab — insert tab / indent")
    {
        REQUIRE(wxSTC_CMD_TAB >= 0);
    }

    SECTION("backtab — Shift+Tab / outdent")
    {
        REQUIRE(wxSTC_CMD_BACKTAB >= 0);
    }
}

// --- Newline ---

TEST_CASE("STC newline command is defined", "[editor_controls][newline]")
{
    REQUIRE(wxSTC_CMD_NEWLINE >= 0);
}

// ═══════════════════════════════════════════════════════
// wxWidgets key constants — verify the platform provides
// all key codes needed for standard editor navigation.
// ═══════════════════════════════════════════════════════

TEST_CASE("wxWidgets key constants for editor navigation", "[editor_controls][keys]")
{
    SECTION("Home key")
    {
        REQUIRE(WXK_HOME > 0);
    }

    SECTION("End key")
    {
        REQUIRE(WXK_END > 0);
    }

    SECTION("Delete key")
    {
        REQUIRE(WXK_DELETE > 0);
    }

    SECTION("Insert key")
    {
        REQUIRE(WXK_INSERT > 0);
    }

    SECTION("Page Up key")
    {
        REQUIRE(WXK_PAGEUP > 0);
    }

    SECTION("Page Down key")
    {
        REQUIRE(WXK_PAGEDOWN > 0);
    }

    SECTION("Backspace key")
    {
        REQUIRE(WXK_BACK > 0);
    }

    SECTION("Tab key")
    {
        REQUIRE(WXK_TAB > 0);
    }

    SECTION("Return / Enter key")
    {
        REQUIRE(WXK_RETURN > 0);
    }

    SECTION("Escape key")
    {
        REQUIRE(WXK_ESCAPE > 0);
    }

    SECTION("Arrow keys")
    {
        REQUIRE(WXK_LEFT > 0);
        REQUIRE(WXK_RIGHT > 0);
        REQUIRE(WXK_UP > 0);
        REQUIRE(WXK_DOWN > 0);
    }
}

// ═══════════════════════════════════════════════════════
// Modifier key constants
// ═══════════════════════════════════════════════════════

TEST_CASE("wxWidgets modifier constants for key combos",
          "[editor_controls][keys]"){SECTION("Shift modifier"){REQUIRE(wxMOD_SHIFT > 0);
}

SECTION("Control modifier")
{
    REQUIRE(wxMOD_CONTROL > 0);
}

SECTION("Alt / Option modifier")
{
    REQUIRE(wxMOD_ALT > 0);
}

#ifdef __WXMAC__
SECTION("Command modifier (macOS)")
{
    REQUIRE(wxMOD_RAW_CONTROL > 0);
}
#endif
}

// ═══════════════════════════════════════════════════════
// Scintilla overtype (Insert mode toggle)
// ═══════════════════════════════════════════════════════

TEST_CASE("STC overtype (insert mode) toggle is defined", "[editor_controls][insert]")
{
    // Toggle between insert and overtype mode (Insert key behaviour)
    REQUIRE(wxSTC_CMD_EDITTOGGLEOVERTYPE >= 0);
}

// ═══════════════════════════════════════════════════════
// Zoom / font size commands
// ═══════════════════════════════════════════════════════

TEST_CASE("STC zoom commands are defined", "[editor_controls][zoom]")
{
    SECTION("zoom in — Cmd+Plus / Ctrl+Plus")
    {
        REQUIRE(wxSTC_CMD_ZOOMIN >= 0);
    }

    SECTION("zoom out — Cmd+Minus / Ctrl+Minus")
    {
        REQUIRE(wxSTC_CMD_ZOOMOUT >= 0);
    }
}

// ═══════════════════════════════════════════════════════
// Scroll commands (without moving cursor)
// ═══════════════════════════════════════════════════════

TEST_CASE("STC scroll commands are defined", "[editor_controls][scroll]")
{
    SECTION("scroll up — keep cursor, scroll viewport up")
    {
        REQUIRE(wxSTC_CMD_LINESCROLLUP >= 0);
    }

    SECTION("scroll down — keep cursor, scroll viewport down")
    {
        REQUIRE(wxSTC_CMD_LINESCROLLDOWN >= 0);
    }
}

// ═══════════════════════════════════════════════════════
// Command IDs are distinct (sanity check)
// ═══════════════════════════════════════════════════════

TEST_CASE("Critical STC command IDs are distinct", "[editor_controls][sanity]")
{
    // The most important operations must have unique IDs
    REQUIRE(wxSTC_CMD_COPY != wxSTC_CMD_CUT);
    REQUIRE(wxSTC_CMD_CUT != wxSTC_CMD_PASTE);
    REQUIRE(wxSTC_CMD_UNDO != wxSTC_CMD_REDO);
    REQUIRE(wxSTC_CMD_HOME != wxSTC_CMD_LINEEND);
    REQUIRE(wxSTC_CMD_PAGEUP != wxSTC_CMD_PAGEDOWN);
    REQUIRE(wxSTC_CMD_CLEAR != wxSTC_CMD_DELETEBACK);
    REQUIRE(wxSTC_CMD_WORDLEFT != wxSTC_CMD_WORDRIGHT);
}
