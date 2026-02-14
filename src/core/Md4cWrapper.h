#pragma once

#include "Types.h"

#include <expected>
#include <md4c.h>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// C++ wrapper around the md4c C library.
/// Parses GitHub Flavored Markdown into a MarkdownDocument AST.
class Md4cParser
{
public:
    Md4cParser();

    // Builder: configure parser flags
    auto enable_tables(bool on = true) -> Md4cParser&;
    auto enable_task_lists(bool on = true) -> Md4cParser&;
    auto enable_strikethrough(bool on = true) -> Md4cParser&;
    auto enable_autolinks(bool on = true) -> Md4cParser&;
    auto enable_no_html(bool on = true) -> Md4cParser&;
    auto enable_latex_math(bool on = true) -> Md4cParser&;

    // Parse markdown to AST
    [[nodiscard]] auto parse(std::string_view markdown)
        -> std::expected<MarkdownDocument, std::string>;

    // Parser state (used during a single parse call)
    struct ParseState
    {
        MarkdownDocument document;
        std::vector<MdNode*> node_stack; // Stack for building the tree
        std::string current_code_block;
        std::string current_code_language;
        bool in_code_block{false};
    };

private:
    unsigned parser_flags_{0};

    // Instance methods called from callbacks
    void on_enter_block(ParseState& state, MD_BLOCKTYPE block_type, void* detail);
    void on_leave_block(ParseState& state, MD_BLOCKTYPE block_type, void* detail);
    void on_enter_span(ParseState& state, MD_SPANTYPE span_type, void* detail);
    void on_leave_span(ParseState& state, MD_SPANTYPE span_type);
    void on_text(ParseState& state, MD_TEXTTYPE text_type, const MD_CHAR* text, MD_SIZE size);

    // Static callback trampolines
    static int enter_block_cb(MD_BLOCKTYPE type, void* detail, void* userdata);
    static int leave_block_cb(MD_BLOCKTYPE type, void* detail, void* userdata);
    static int enter_span_cb(MD_SPANTYPE type, void* detail, void* userdata);
    static int leave_span_cb(MD_SPANTYPE type, void* detail, void* userdata);
    static int text_cb(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);
};

} // namespace markamp::core
