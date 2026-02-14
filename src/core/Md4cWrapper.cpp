#include "Md4cWrapper.h"

#include <algorithm>
#include <cstring>
#include <md4c.h>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Helper: extract MD_ATTRIBUTE text
// ═══════════════════════════════════════════════════════

namespace
{

auto attr_to_string(const MD_ATTRIBUTE& attr) -> std::string
{
    if (attr.text == nullptr || attr.size == 0)
    {
        return {};
    }
    return std::string(attr.text, attr.size);
}

} // anonymous namespace

// ═══════════════════════════════════════════════════════
// Construction & builder
// ═══════════════════════════════════════════════════════

Md4cParser::Md4cParser()
{
    // Default: GFM dialect
    parser_flags_ = MD_DIALECT_GITHUB | MD_FLAG_NOHTML | MD_FLAG_LATEXMATHSPANS;
}

auto Md4cParser::enable_tables(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_TABLES;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_TABLES);
    }
    return *this;
}

auto Md4cParser::enable_task_lists(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_TASKLISTS;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_TASKLISTS);
    }
    return *this;
}

auto Md4cParser::enable_strikethrough(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_STRIKETHROUGH;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_STRIKETHROUGH);
    }
    return *this;
}

auto Md4cParser::enable_autolinks(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_PERMISSIVEAUTOLINKS;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_PERMISSIVEAUTOLINKS);
    }
    return *this;
}

auto Md4cParser::enable_no_html(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_NOHTML;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_NOHTML);
    }
    return *this;
}

auto Md4cParser::enable_latex_math(bool on) -> Md4cParser&
{
    if (on)
    {
        parser_flags_ |= MD_FLAG_LATEXMATHSPANS;
    }
    else
    {
        parser_flags_ &= ~static_cast<unsigned>(MD_FLAG_LATEXMATHSPANS);
    }
    return *this;
}

// ═══════════════════════════════════════════════════════
// Push/pop helpers
// ═══════════════════════════════════════════════════════

namespace
{

void push_node(Md4cParser::ParseState& state, MdNode node)
{
    if (state.node_stack.empty())
    {
        return;
    }
    auto* parent = state.node_stack.back();
    parent->children.push_back(std::move(node));
    state.node_stack.push_back(&parent->children.back());
}

void pop_node(Md4cParser::ParseState& state)
{
    if (state.node_stack.size() > 1)
    {
        state.node_stack.pop_back();
    }
}

} // anonymous namespace

// ═══════════════════════════════════════════════════════
// Block callbacks
// ═══════════════════════════════════════════════════════

void Md4cParser::on_enter_block(ParseState& state, MD_BLOCKTYPE block_type, void* detail)
{
    MdNode node;

    switch (block_type)
    {
        case MD_BLOCK_DOC:
            // Already have root, just push it
            state.node_stack.push_back(&state.document.root);
            return;

        case MD_BLOCK_P:
            node.type = MdNodeType::Paragraph;
            break;

        case MD_BLOCK_QUOTE:
            node.type = MdNodeType::BlockQuote;
            break;

        case MD_BLOCK_H:
        {
            auto* h_detail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
            node.type = MdNodeType::Heading;
            node.heading_level = static_cast<int>(h_detail->level);
            break;
        }

        case MD_BLOCK_UL:
        {
            auto* ul_detail = static_cast<MD_BLOCK_UL_DETAIL*>(detail);
            node.type = MdNodeType::UnorderedList;
            node.is_tight = ul_detail->is_tight != 0;
            break;
        }

        case MD_BLOCK_OL:
        {
            auto* ol_detail = static_cast<MD_BLOCK_OL_DETAIL*>(detail);
            node.type = MdNodeType::OrderedList;
            node.start_number = static_cast<int>(ol_detail->start);
            node.is_tight = ol_detail->is_tight != 0;
            break;
        }

        case MD_BLOCK_LI:
        {
            auto* li_detail = static_cast<MD_BLOCK_LI_DETAIL*>(detail);
            node.type = MdNodeType::ListItem;
            if (li_detail->is_task != 0)
            {
                // We'll add a TaskListMarker child
                MdNode marker;
                marker.type = MdNodeType::TaskListMarker;
                marker.is_checked = (li_detail->task_mark == 'x' || li_detail->task_mark == 'X');
                node.children.push_back(std::move(marker));
            }
            break;
        }

        case MD_BLOCK_HR:
            node.type = MdNodeType::HorizontalRule;
            break;

        case MD_BLOCK_CODE:
        {
            auto* code_detail = static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
            auto lang = attr_to_string(code_detail->lang);
            auto info = attr_to_string(code_detail->info);

            if (lang == "mermaid")
            {
                node.type = MdNodeType::MermaidBlock;
            }
            else if (code_detail->fence_char != 0)
            {
                node.type = MdNodeType::FencedCodeBlock;
            }
            else
            {
                node.type = MdNodeType::CodeBlock;
            }

            node.language = lang;
            node.info_string = info;

            // Track code language
            if (!lang.empty())
            {
                auto& langs = state.document.code_languages;
                if (std::find(langs.begin(), langs.end(), lang) == langs.end())
                {
                    langs.push_back(lang);
                }
            }

            state.in_code_block = true;
            state.current_code_block.clear();
            state.current_code_language = lang;
            break;
        }

        case MD_BLOCK_HTML:
            node.type = MdNodeType::HtmlBlock;
            break;

        case MD_BLOCK_TABLE:
            node.type = MdNodeType::Table;
            break;

        case MD_BLOCK_THEAD:
            node.type = MdNodeType::TableHead;
            break;

        case MD_BLOCK_TBODY:
            node.type = MdNodeType::TableBody;
            break;

        case MD_BLOCK_TR:
            node.type = MdNodeType::TableRow;
            break;

        case MD_BLOCK_TH:
        {
            auto* td_detail = static_cast<MD_BLOCK_TD_DETAIL*>(detail);
            node.type = MdNodeType::TableCell;
            node.is_header = true;
            switch (td_detail->align)
            {
                case MD_ALIGN_LEFT:
                    node.alignment = MdAlignment::Left;
                    break;
                case MD_ALIGN_CENTER:
                    node.alignment = MdAlignment::Center;
                    break;
                case MD_ALIGN_RIGHT:
                    node.alignment = MdAlignment::Right;
                    break;
                default:
                    node.alignment = MdAlignment::Default;
                    break;
            }
            break;
        }

        case MD_BLOCK_TD:
        {
            auto* td_detail = static_cast<MD_BLOCK_TD_DETAIL*>(detail);
            node.type = MdNodeType::TableCell;
            node.is_header = false;
            switch (td_detail->align)
            {
                case MD_ALIGN_LEFT:
                    node.alignment = MdAlignment::Left;
                    break;
                case MD_ALIGN_CENTER:
                    node.alignment = MdAlignment::Center;
                    break;
                case MD_ALIGN_RIGHT:
                    node.alignment = MdAlignment::Right;
                    break;
                default:
                    node.alignment = MdAlignment::Default;
                    break;
            }
            break;
        }
    }

    push_node(state, std::move(node));
}

void Md4cParser::on_leave_block(ParseState& state, MD_BLOCKTYPE block_type, void* /*detail*/)
{
    if (block_type == MD_BLOCK_DOC)
    {
        state.node_stack.clear();
        return;
    }

    if (block_type == MD_BLOCK_CODE)
    {
        // Finalize code block: store accumulated text
        if (!state.node_stack.empty())
        {
            auto* current = state.node_stack.back();
            current->text_content = state.current_code_block;

            if (current->type == MdNodeType::MermaidBlock)
            {
                state.document.mermaid_blocks.push_back(state.current_code_block);
            }
        }
        state.in_code_block = false;
        state.current_code_block.clear();
        state.current_code_language.clear();
    }

    pop_node(state);
}

// ═══════════════════════════════════════════════════════
// Span callbacks
// ═══════════════════════════════════════════════════════

void Md4cParser::on_enter_span(ParseState& state, MD_SPANTYPE span_type, void* detail)
{
    MdNode node;

    switch (span_type)
    {
        case MD_SPAN_EM:
            node.type = MdNodeType::Emphasis;
            break;

        case MD_SPAN_STRONG:
            node.type = MdNodeType::Strong;
            break;

        case MD_SPAN_CODE:
            node.type = MdNodeType::Code;
            break;

        case MD_SPAN_A:
        {
            auto* a_detail = static_cast<MD_SPAN_A_DETAIL*>(detail);
            node.type = MdNodeType::Link;
            node.url = attr_to_string(a_detail->href);
            node.title = attr_to_string(a_detail->title);
            break;
        }

        case MD_SPAN_IMG:
        {
            auto* img_detail = static_cast<MD_SPAN_IMG_DETAIL*>(detail);
            node.type = MdNodeType::Image;
            node.url = attr_to_string(img_detail->src);
            node.title = attr_to_string(img_detail->title);
            break;
        }

        case MD_SPAN_DEL:
            node.type = MdNodeType::Strikethrough;
            break;

        case MD_SPAN_LATEXMATH:
            node.type = MdNodeType::MathInline;
            break;

        case MD_SPAN_LATEXMATH_DISPLAY:
            node.type = MdNodeType::MathDisplay;
            node.is_display = true;
            break;

        default:
            // Unsupported span types (WIKILINK, UNDERLINE)
            node.type = MdNodeType::Text;
            break;
    }

    push_node(state, std::move(node));
}

void Md4cParser::on_leave_span(ParseState& state, MD_SPANTYPE /*span_type*/)
{
    pop_node(state);
}

// ═══════════════════════════════════════════════════════
// Text callback
// ═══════════════════════════════════════════════════════

void Md4cParser::on_text(ParseState& state,
                         MD_TEXTTYPE text_type,
                         const MD_CHAR* text,
                         MD_SIZE size)
{
    if (state.in_code_block)
    {
        state.current_code_block.append(text, size);
        return;
    }

    switch (text_type)
    {
        case MD_TEXT_NORMAL:
        case MD_TEXT_CODE:
        case MD_TEXT_ENTITY:
        case MD_TEXT_HTML:
        {
            MdNode text_node;
            text_node.type = MdNodeType::Text;
            text_node.text_content = std::string(text, size);

            if (!state.node_stack.empty())
            {
                state.node_stack.back()->children.push_back(std::move(text_node));
            }
            break;
        }

        case MD_TEXT_BR:
        {
            MdNode br_node;
            br_node.type = MdNodeType::LineBreak;
            if (!state.node_stack.empty())
            {
                state.node_stack.back()->children.push_back(std::move(br_node));
            }
            break;
        }

        case MD_TEXT_SOFTBR:
        {
            MdNode soft_node;
            soft_node.type = MdNodeType::SoftBreak;
            if (!state.node_stack.empty())
            {
                state.node_stack.back()->children.push_back(std::move(soft_node));
            }
            break;
        }

        case MD_TEXT_NULLCHAR:
        {
            // Replace null with U+FFFD
            MdNode text_node;
            text_node.type = MdNodeType::Text;
            text_node.text_content = "\xEF\xBF\xBD"; // UTF-8 for U+FFFD
            if (!state.node_stack.empty())
            {
                state.node_stack.back()->children.push_back(std::move(text_node));
            }
            break;
        }

        case MD_TEXT_LATEXMATH:
        {
            // LaTeX math text — store as text content in current node
            MdNode text_node;
            text_node.type = MdNodeType::Text;
            text_node.text_content = std::string(text, size);
            if (!state.node_stack.empty())
            {
                state.node_stack.back()->children.push_back(std::move(text_node));
            }
            break;
        }
    }
}

// ═══════════════════════════════════════════════════════
// Static callback trampolines
// ═══════════════════════════════════════════════════════

struct CallbackData
{
    Md4cParser* parser;
    Md4cParser::ParseState* state;
};

int Md4cParser::enter_block_cb(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    auto* data = static_cast<CallbackData*>(userdata);
    data->parser->on_enter_block(*data->state, type, detail);
    return 0;
}

int Md4cParser::leave_block_cb(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    auto* data = static_cast<CallbackData*>(userdata);
    data->parser->on_leave_block(*data->state, type, detail);
    return 0;
}

int Md4cParser::enter_span_cb(MD_SPANTYPE type, void* detail, void* userdata)
{
    auto* data = static_cast<CallbackData*>(userdata);
    data->parser->on_enter_span(*data->state, type, detail);
    return 0;
}

int Md4cParser::leave_span_cb(MD_SPANTYPE type, void* /*detail*/, void* userdata)
{
    auto* data = static_cast<CallbackData*>(userdata);
    data->parser->on_leave_span(*data->state, type);
    return 0;
}

int Md4cParser::text_cb(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    auto* data = static_cast<CallbackData*>(userdata);
    data->parser->on_text(*data->state, type, text, size);
    return 0;
}

// ═══════════════════════════════════════════════════════
// Parse
// ═══════════════════════════════════════════════════════

auto Md4cParser::parse(std::string_view markdown) -> std::expected<MarkdownDocument, std::string>
{
    ParseState state;
    state.document.root.type = MdNodeType::Document;

    CallbackData cb_data{this, &state};

    MD_PARSER md_parser{};
    md_parser.abi_version = 0;
    md_parser.flags = parser_flags_;
    md_parser.enter_block = &Md4cParser::enter_block_cb;
    md_parser.leave_block = &Md4cParser::leave_block_cb;
    md_parser.enter_span = &Md4cParser::enter_span_cb;
    md_parser.leave_span = &Md4cParser::leave_span_cb;
    md_parser.text = &Md4cParser::text_cb;
    md_parser.debug_log = nullptr;
    md_parser.syntax = nullptr;

    int result =
        md_parse(markdown.data(), static_cast<MD_SIZE>(markdown.size()), &md_parser, &cb_data);

    if (result != 0)
    {
        return std::unexpected("md4c parse error (code " + std::to_string(result) + ")");
    }

    return std::move(state.document);
}

} // namespace markamp::core
