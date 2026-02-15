/// @file LatexEngine.cpp
/// @brief V4 Phase 18 – Inline LaTeX Rendering Enhancement implementation.

#include "core/LatexEngine.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <array>
#include <regex>
#include <sstream>
#include <unordered_map>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

LatexEngine::LatexEngine(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

// ============================================================================
// Core symbol table renderer
// ============================================================================

auto LatexEngine::render_core(const std::string& latex, bool is_display) -> std::string
{
    // Basic symbol replacement table
    static const std::unordered_map<std::string, std::string> symbols = {
        {"\\alpha", "α"},      {"\\beta", "β"},      {"\\gamma", "γ"},      {"\\delta", "δ"},
        {"\\epsilon", "ε"},    {"\\theta", "θ"},     {"\\lambda", "λ"},     {"\\mu", "μ"},
        {"\\pi", "π"},         {"\\sigma", "σ"},     {"\\omega", "ω"},      {"\\phi", "φ"},
        {"\\psi", "ψ"},        {"\\Gamma", "Γ"},     {"\\Delta", "Δ"},      {"\\Theta", "Θ"},
        {"\\Lambda", "Λ"},     {"\\Sigma", "Σ"},     {"\\Phi", "Φ"},        {"\\Omega", "Ω"},
        {"\\Pi", "Π"},         {"\\infty", "∞"},     {"\\sum", "∑"},        {"\\prod", "∏"},
        {"\\int", "∫"},        {"\\partial", "∂"},   {"\\nabla", "∇"},      {"\\pm", "±"},
        {"\\times", "×"},      {"\\div", "÷"},       {"\\neq", "≠"},        {"\\leq", "≤"},
        {"\\geq", "≥"},        {"\\approx", "≈"},    {"\\rightarrow", "→"}, {"\\leftarrow", "←"},
        {"\\Rightarrow", "⇒"}, {"\\Leftarrow", "⇐"}, {"\\forall", "∀"},     {"\\exists", "∃"},
        {"\\in", "∈"},         {"\\notin", "∉"},     {"\\subset", "⊂"},     {"\\supset", "⊃"},
        {"\\cup", "∪"},        {"\\cap", "∩"},       {"\\emptyset", "∅"},   {"\\cdot", "·"},
        {"\\ldots", "…"},      {"\\cdots", "⋯"},     {"\\mathbb{R}", "ℝ"},  {"\\mathbb{Z}", "ℤ"},
        {"\\mathbb{N}", "ℕ"},  {"\\mathbb{Q}", "ℚ"}, {"\\mathbb{C}", "ℂ"},
    };

    std::string result = latex;

    // Replace known symbols
    for (const auto& [cmd, symbol] : symbols)
    {
        size_t pos = 0;
        while ((pos = result.find(cmd, pos)) != std::string::npos)
        {
            result.replace(pos, cmd.size(), symbol);
            pos += symbol.size();
        }
    }

    // Handle \frac{a}{b} -> a/b
    const std::regex frac_re(R"(\\frac\{([^}]*)\}\{([^}]*)\})");
    result = std::regex_replace(result, frac_re, "$1/$2");

    // Handle \sqrt{x} -> √(x)
    const std::regex sqrt_re(R"(\\sqrt\{([^}]*)\})");
    result = std::regex_replace(result, sqrt_re, "√($1)");

    // Handle ^{n} -> superscript indicators
    const std::regex sup_re(R"(\^\{([^}]*)\})");
    result = std::regex_replace(result, sup_re, "^($1)");

    // Handle _{n} -> subscript indicators
    const std::regex sub_re(R"(_\{([^}]*)\})");
    result = std::regex_replace(result, sub_re, "_($1)");

    // Single-char super/subscript
    const std::regex sup_single(R"(\^(\w))");
    result = std::regex_replace(result, sup_single, "^$1");

    const std::regex sub_single(R"(_(\w))");
    result = std::regex_replace(result, sub_single, "_$1");

    // Strip remaining backslash commands we don't recognize
    const std::regex unknown_cmd(R"(\\[a-zA-Z]+)");
    result = std::regex_replace(result, unknown_cmd, "");

    // Strip braces
    std::string cleaned;
    for (const char character : result)
    {
        if (character != '{' && character != '}')
        {
            cleaned += character;
        }
    }

    if (is_display)
    {
        return "\n  " + cleaned + "\n";
    }
    return cleaned;
}

// ============================================================================
// Expand macros
// ============================================================================

auto LatexEngine::expand_macros(const std::string& latex) const -> std::string
{
    std::string result = latex;

    for (const auto& [name, macro] : macros_)
    {
        if (macro.arg_count == 0)
        {
            // Simple replacement
            size_t pos = 0;
            while ((pos = result.find(name, pos)) != std::string::npos)
            {
                result.replace(pos, name.size(), macro.expansion);
                pos += macro.expansion.size();
            }
        }
        else
        {
            // Macro with arguments: \name{arg1}{arg2}...
            size_t pos = 0;
            while ((pos = result.find(name, pos)) != std::string::npos)
            {
                const size_t after_name = pos + name.size();
                std::vector<std::string> args;
                size_t scan_pos = after_name;

                for (int arg_idx = 0; arg_idx < macro.arg_count; ++arg_idx)
                {
                    if (scan_pos < result.size() && result[scan_pos] == '{')
                    {
                        auto close = result.find('}', scan_pos);
                        if (close != std::string::npos)
                        {
                            args.push_back(result.substr(scan_pos + 1, close - scan_pos - 1));
                            scan_pos = close + 1;
                        }
                    }
                }

                // Build expanded text
                std::string expanded = macro.expansion;
                for (size_t arg_idx = 0; arg_idx < args.size(); ++arg_idx)
                {
                    const std::string placeholder = "#" + std::to_string(arg_idx + 1);
                    size_t placeholder_pos = 0;
                    while ((placeholder_pos = expanded.find(placeholder, placeholder_pos)) !=
                           std::string::npos)
                    {
                        expanded.replace(placeholder_pos, placeholder.size(), args[arg_idx]);
                        placeholder_pos += args[arg_idx].size();
                    }
                }

                result.replace(pos, scan_pos - pos, expanded);
                pos += expanded.size();
            }
        }
    }

    return result;
}

// ============================================================================
// Render inline math
// ============================================================================

auto LatexEngine::render_inline(const std::string& latex) const -> LatexRenderResult
{
    LatexRenderResult result;
    const std::string expanded = expand_macros(latex);
    result.output = render_core(expanded, false);
    result.success = true;
    result.format = LatexRenderResult::Format::kUnicode;
    return result;
}

// ============================================================================
// Render display math
// ============================================================================

auto LatexEngine::render_display(const std::string& latex) -> LatexRenderResult
{
    LatexRenderResult result;
    const std::string expanded = expand_macros(latex);
    result.output = render_core(expanded, true);
    result.success = true;
    result.equation_number = next_equation_number();
    result.format = LatexRenderResult::Format::kUnicode;
    return result;
}

// ============================================================================
// Render environment
// ============================================================================

auto LatexEngine::render_environment(const LatexEnvironment& env) -> LatexRenderResult
{
    LatexRenderResult result;
    const std::string expanded = expand_macros(env.content);

    if (env.name == "matrix" || env.name == "pmatrix" || env.name == "bmatrix")
    {
        // Matrix rendering: split by \\ for rows, & for columns
        std::string rendered;
        std::istringstream rows(expanded);
        std::string row;

        std::string open_bracket = "[";
        std::string close_bracket = "]";
        if (env.name == "pmatrix")
        {
            open_bracket = "(";
            close_bracket = ")";
        }

        rendered += open_bracket + " ";
        bool first_row = true;
        while (std::getline(rows, row, '\\'))
        {
            // Skip empty parts from double-backslash
            if (row.empty() || row == "\\")
            {
                continue;
            }
            if (!first_row)
            {
                rendered += "; ";
            }
            // Split by &
            std::istringstream cols(row);
            std::string col;
            bool first_col = true;
            while (std::getline(cols, col, '&'))
            {
                // Trim
                while (!col.empty() && col.front() == ' ')
                {
                    col.erase(col.begin());
                }
                while (!col.empty() && col.back() == ' ')
                {
                    col.pop_back();
                }
                if (!first_col)
                {
                    rendered += ", ";
                }
                rendered += render_core(col, false);
                first_col = false;
            }
            first_row = false;
        }
        rendered += " " + close_bracket;

        result.output = rendered;
    }
    else if (env.name == "cases")
    {
        // Cases: split by \\, format with conditions
        std::string rendered;
        std::istringstream rows(expanded);
        std::string row;

        rendered += "{ ";
        bool first_row = true;
        while (std::getline(rows, row, '\\'))
        {
            if (row.empty() || row == "\\")
            {
                continue;
            }
            if (!first_row)
            {
                rendered += ", ";
            }
            rendered += render_core(row, false);
            first_row = false;
        }
        rendered += " }";

        result.output = rendered;
    }
    else
    {
        // Default: just render content
        result.output = render_core(expanded, true);
    }

    if (env.numbered)
    {
        result.equation_number = next_equation_number();
    }

    result.success = true;
    result.format = LatexRenderResult::Format::kUnicode;
    return result;
}

// ============================================================================
// Process document
// ============================================================================

auto LatexEngine::process_document(const std::string& markdown) -> std::string
{
    std::string result = markdown;

    // Process display math ($$...$$) first
    {
        const std::regex display_re(R"(\$\$([^$]+)\$\$)");
        std::string built;
        auto search_begin = result.cbegin();
        std::sregex_iterator iter(result.cbegin(), result.cend(), display_re);
        const std::sregex_iterator end_iter;
        for (; iter != end_iter; ++iter)
        {
            built.append(search_begin, (*iter)[0].first);
            auto rendered = render_display((*iter)[1].str());
            if (rendered.success)
            {
                built += rendered.output;
            }
            else
            {
                built += (*iter)[0].str();
            }
            search_begin = (*iter)[0].second;
        }
        built.append(search_begin, result.cend());
        result = built;
    }

    // Process inline math ($...$) — avoid matching $$
    {
        const std::regex inline_re(R"(\$([^$]+)\$)");
        std::string built;
        auto search_begin = result.cbegin();
        std::sregex_iterator iter(result.cbegin(), result.cend(), inline_re);
        const std::sregex_iterator end_iter;
        for (; iter != end_iter; ++iter)
        {
            built.append(search_begin, (*iter)[0].first);
            auto rendered = render_inline((*iter)[1].str());
            if (rendered.success)
            {
                built += rendered.output;
            }
            else
            {
                built += (*iter)[0].str();
            }
            search_begin = (*iter)[0].second;
        }
        built.append(search_begin, result.cend());
        result = built;
    }

    return result;
}

// ============================================================================
// Register macro
// ============================================================================

auto LatexEngine::register_macro(const LatexMacro& macro) -> void
{
    macros_[macro.name] = macro;
}

// ============================================================================
// Load preamble
// ============================================================================

auto LatexEngine::load_preamble(const std::string& preamble) -> int
{
    int count = 0;

    // Match \newcommand{\name}{expansion} and \newcommand{\name}[n]{expansion}
    const std::regex newcmd_re(R"(\\newcommand\{(\\[a-zA-Z]+)\}(?:\[(\d+)\])?\{([^}]*)\})");

    auto begin = std::sregex_iterator(preamble.begin(), preamble.end(), newcmd_re);
    const auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        LatexMacro macro;
        macro.name = (*iter)[1].str();
        macro.arg_count = (*iter)[2].matched ? std::stoi((*iter)[2].str()) : 0;
        macro.expansion = (*iter)[3].str();
        register_macro(macro);
        ++count;
    }

    return count;
}

// ============================================================================
// Equation numbering
// ============================================================================

auto LatexEngine::next_equation_number() -> int
{
    return equation_counter_++;
}

auto LatexEngine::reset_numbering() -> void
{
    equation_counter_ = 1;
}

auto LatexEngine::macros() const -> const std::unordered_map<std::string, LatexMacro>&
{
    return macros_;
}

} // namespace markamp::core
