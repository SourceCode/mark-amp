#include "MathRenderer.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Constructor: populate symbol table
// ═══════════════════════════════════════════════════════

MathRenderer::MathRenderer()
{
    // Greek lowercase
    symbols_["\\alpha"] = "\xCE\xB1";      // α
    symbols_["\\beta"] = "\xCE\xB2";       // β
    symbols_["\\gamma"] = "\xCE\xB3";      // γ
    symbols_["\\delta"] = "\xCE\xB4";      // δ
    symbols_["\\epsilon"] = "\xCE\xB5";    // ε
    symbols_["\\varepsilon"] = "\xCE\xB5"; // ε
    symbols_["\\zeta"] = "\xCE\xB6";       // ζ
    symbols_["\\eta"] = "\xCE\xB7";        // η
    symbols_["\\theta"] = "\xCE\xB8";      // θ
    symbols_["\\iota"] = "\xCE\xB9";       // ι
    symbols_["\\kappa"] = "\xCE\xBA";      // κ
    symbols_["\\lambda"] = "\xCE\xBB";     // λ
    symbols_["\\mu"] = "\xCE\xBC";         // μ
    symbols_["\\nu"] = "\xCE\xBD";         // ν
    symbols_["\\xi"] = "\xCE\xBE";         // ξ
    symbols_["\\pi"] = "\xCF\x80";         // π
    symbols_["\\rho"] = "\xCF\x81";        // ρ
    symbols_["\\sigma"] = "\xCF\x83";      // σ
    symbols_["\\tau"] = "\xCF\x84";        // τ
    symbols_["\\upsilon"] = "\xCF\x85";    // υ
    symbols_["\\phi"] = "\xCF\x86";        // φ
    symbols_["\\varphi"] = "\xCF\x95";     // ϕ
    symbols_["\\chi"] = "\xCF\x87";        // χ
    symbols_["\\psi"] = "\xCF\x88";        // ψ
    symbols_["\\omega"] = "\xCF\x89";      // ω

    // Greek uppercase
    symbols_["\\Gamma"] = "\xCE\x93";  // Γ
    symbols_["\\Delta"] = "\xCE\x94";  // Δ
    symbols_["\\Theta"] = "\xCE\x98";  // Θ
    symbols_["\\Lambda"] = "\xCE\x9B"; // Λ
    symbols_["\\Xi"] = "\xCE\x9E";     // Ξ
    symbols_["\\Pi"] = "\xCE\xA0";     // Π
    symbols_["\\Sigma"] = "\xCE\xA3";  // Σ
    symbols_["\\Phi"] = "\xCE\xA6";    // Φ
    symbols_["\\Psi"] = "\xCE\xA8";    // Ψ
    symbols_["\\Omega"] = "\xCE\xA9";  // Ω

    // Operators
    symbols_["\\sum"] = "\xE2\x88\x91";     // ∑
    symbols_["\\prod"] = "\xE2\x88\x8F";    // ∏
    symbols_["\\int"] = "\xE2\x88\xAB";     // ∫
    symbols_["\\iint"] = "\xE2\x88\xAC";    // ∬
    symbols_["\\oint"] = "\xE2\x88\xAE";    // ∮
    symbols_["\\partial"] = "\xE2\x88\x82"; // ∂
    symbols_["\\nabla"] = "\xE2\x88\x87";   // ∇
    symbols_["\\sqrt"] = "\xE2\x88\x9A";    // √
    symbols_["\\cdot"] = "\xC2\xB7";        // ·
    symbols_["\\times"] = "\xC3\x97";       // ×
    symbols_["\\div"] = "\xC3\xB7";         // ÷
    symbols_["\\pm"] = "\xC2\xB1";          // ±
    symbols_["\\mp"] = "\xE2\x88\x93";      // ∓
    symbols_["\\circ"] = "\xE2\x88\x98";    // ∘

    // Relations
    symbols_["\\leq"] = "\xE2\x89\xA4";      // ≤
    symbols_["\\le"] = "\xE2\x89\xA4";       // ≤
    symbols_["\\geq"] = "\xE2\x89\xA5";      // ≥
    symbols_["\\ge"] = "\xE2\x89\xA5";       // ≥
    symbols_["\\neq"] = "\xE2\x89\xA0";      // ≠
    symbols_["\\ne"] = "\xE2\x89\xA0";       // ≠
    symbols_["\\approx"] = "\xE2\x89\x88";   // ≈
    symbols_["\\equiv"] = "\xE2\x89\xA1";    // ≡
    symbols_["\\sim"] = "\xE2\x88\xBC";      // ∼
    symbols_["\\propto"] = "\xE2\x88\x9D";   // ∝
    symbols_["\\prec"] = "\xE2\x89\xBA";     // ≺
    symbols_["\\succ"] = "\xE2\x89\xBB";     // ≻
    symbols_["\\subset"] = "\xE2\x8A\x82";   // ⊂
    symbols_["\\supset"] = "\xE2\x8A\x83";   // ⊃
    symbols_["\\subseteq"] = "\xE2\x8A\x86"; // ⊆
    symbols_["\\supseteq"] = "\xE2\x8A\x87"; // ⊇
    symbols_["\\in"] = "\xE2\x88\x88";       // ∈
    symbols_["\\notin"] = "\xE2\x88\x89";    // ∉
    symbols_["\\ni"] = "\xE2\x88\x8B";       // ∋

    // Arrows
    symbols_["\\rightarrow"] = "\xE2\x86\x92";     // →
    symbols_["\\to"] = "\xE2\x86\x92";             // →
    symbols_["\\leftarrow"] = "\xE2\x86\x90";      // ←
    symbols_["\\leftrightarrow"] = "\xE2\x86\x94"; // ↔
    symbols_["\\Rightarrow"] = "\xE2\x87\x92";     // ⇒
    symbols_["\\Leftarrow"] = "\xE2\x87\x90";      // ⇐
    symbols_["\\Leftrightarrow"] = "\xE2\x87\x94"; // ⇔
    symbols_["\\uparrow"] = "\xE2\x86\x91";        // ↑
    symbols_["\\downarrow"] = "\xE2\x86\x93";      // ↓
    symbols_["\\mapsto"] = "\xE2\x86\xA6";         // ↦

    // Logic & set theory
    symbols_["\\forall"] = "\xE2\x88\x80";     // ∀
    symbols_["\\exists"] = "\xE2\x88\x83";     // ∃
    symbols_["\\nexists"] = "\xE2\x88\x84";    // ∄
    symbols_["\\land"] = "\xE2\x88\xA7";       // ∧
    symbols_["\\lor"] = "\xE2\x88\xA8";        // ∨
    symbols_["\\neg"] = "\xC2\xAC";            // ¬
    symbols_["\\lnot"] = "\xC2\xAC";           // ¬
    symbols_["\\cup"] = "\xE2\x88\xAA";        // ∪
    symbols_["\\cap"] = "\xE2\x88\xA9";        // ∩
    symbols_["\\emptyset"] = "\xE2\x88\x85";   // ∅
    symbols_["\\varnothing"] = "\xE2\x88\x85"; // ∅

    // Misc symbols
    symbols_["\\infty"] = "\xE2\x88\x9E";    // ∞
    symbols_["\\aleph"] = "\xD7\x90";        // א
    symbols_["\\hbar"] = "\xE2\x84\x8F";     // ℏ
    symbols_["\\ell"] = "\xE2\x84\x93";      // ℓ
    symbols_["\\Re"] = "\xE2\x84\x9C";       // ℜ
    symbols_["\\Im"] = "\xE2\x84\x91";       // ℑ
    symbols_["\\wp"] = "\xE2\x84\x98";       // ℘
    symbols_["\\angle"] = "\xE2\x88\xA0";    // ∠
    symbols_["\\triangle"] = "\xE2\x96\xB3"; // △
    symbols_["\\star"] = "\xE2\x98\x86";     // ☆
    symbols_["\\dagger"] = "\xE2\x80\xA0";   // †
    symbols_["\\ddagger"] = "\xE2\x80\xA1";  // ‡

    // Dots
    symbols_["\\cdots"] = "\xE2\x8B\xAF"; // ⋯
    symbols_["\\ldots"] = "\xE2\x80\xA6"; // …
    symbols_["\\vdots"] = "\xE2\x8B\xAE"; // ⋮
    symbols_["\\ddots"] = "\xE2\x8B\xB1"; // ⋱

    // Brackets
    symbols_["\\langle"] = "\xE2\x9F\xA8"; // ⟨
    symbols_["\\rangle"] = "\xE2\x9F\xA9"; // ⟩
    symbols_["\\lceil"] = "\xE2\x8C\x88";  // ⌈
    symbols_["\\rceil"] = "\xE2\x8C\x89";  // ⌉
    symbols_["\\lfloor"] = "\xE2\x8C\x8A"; // ⌊
    symbols_["\\rfloor"] = "\xE2\x8C\x8B"; // ⌋

    // Spacing commands (render as appropriate whitespace)
    symbols_["\\quad"] = "\xE2\x80\x83";              // em space
    symbols_["\\qquad"] = "\xE2\x80\x83\xE2\x80\x83"; // 2× em space
    symbols_["\\,"] = "\xE2\x80\x89";                 // thin space
    symbols_["\\;"] = "\xE2\x80\x82";                 // en space
    symbols_["\\!"] = "";                             // negative thin space (absorb)

    // Text formatting commands (pass through content)
    symbols_["\\text"] = "";   // handled specially in render_tokens
    symbols_["\\mathrm"] = ""; // handled specially
    symbols_["\\mathbf"] = ""; // handled specially
    symbols_["\\mathit"] = ""; // handled specially
}

// ═══════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════

auto MathRenderer::render(std::string_view latex, bool is_display) -> std::string
{
    auto tokens = tokenize(latex);
    auto rendered = render_tokens(tokens);

    if (is_display)
    {
        return "<div class=\"math-display\">" + rendered + "</div>\n";
    }
    return "<span class=\"math-inline\">" + rendered + "</span>";
}

auto MathRenderer::is_available() const -> bool
{
    return true; // Pure C++ — always available
}

// ═══════════════════════════════════════════════════════
// Tokenizer
// ═══════════════════════════════════════════════════════

auto MathRenderer::tokenize(std::string_view latex) const -> std::vector<Token>
{
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < latex.size())
    {
        char ch = latex[pos];

        if (ch == '\\')
        {
            // LaTeX command: \commandname
            size_t start = pos;
            pos++; // skip backslash

            if (pos < latex.size() && !std::isalpha(static_cast<unsigned char>(latex[pos])))
            {
                // Single-character command like \, \; \! \{ \}
                std::string cmd(latex.substr(start, 2));
                pos++;

                // Special: \{ and \} are literal braces
                if (cmd == "\\{")
                {
                    tokens.push_back({Token::Kind::kText, "{"});
                }
                else if (cmd == "\\}")
                {
                    tokens.push_back({Token::Kind::kText, "}"});
                }
                else
                {
                    tokens.push_back({Token::Kind::kCommand, cmd});
                }
            }
            else
            {
                // Multi-character command
                while (pos < latex.size() && std::isalpha(static_cast<unsigned char>(latex[pos])))
                {
                    pos++;
                }
                tokens.push_back(
                    {Token::Kind::kCommand, std::string(latex.substr(start, pos - start))});
            }
        }
        else if (ch == '^')
        {
            tokens.push_back({Token::Kind::kSuperscript, "^"});
            pos++;
        }
        else if (ch == '_')
        {
            tokens.push_back({Token::Kind::kSubscript, "_"});
            pos++;
        }
        else if (ch == '{')
        {
            tokens.push_back({Token::Kind::kOpenBrace, "{"});
            pos++;
        }
        else if (ch == '}')
        {
            tokens.push_back({Token::Kind::kCloseBrace, "}"});
            pos++;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            // Whitespace — collapse into single space
            while (pos < latex.size() && (latex[pos] == ' ' || latex[pos] == '\t' ||
                                          latex[pos] == '\n' || latex[pos] == '\r'))
            {
                pos++;
            }
            tokens.push_back({Token::Kind::kText, " "});
        }
        else
        {
            // Plain text — accumulate consecutive non-special characters
            size_t start = pos;
            while (pos < latex.size() && latex[pos] != '\\' && latex[pos] != '^' &&
                   latex[pos] != '_' && latex[pos] != '{' && latex[pos] != '}' &&
                   latex[pos] != ' ' && latex[pos] != '\t' && latex[pos] != '\n' &&
                   latex[pos] != '\r')
            {
                pos++;
            }
            tokens.push_back({Token::Kind::kText, std::string(latex.substr(start, pos - start))});
        }
    }

    return tokens;
}

// ═══════════════════════════════════════════════════════
// Token renderer
// ═══════════════════════════════════════════════════════

auto MathRenderer::render_tokens(const std::vector<Token>& tokens) const -> std::string
{
    std::string result;
    size_t i = 0;

    while (i < tokens.size())
    {
        const auto& token = tokens[i];

        switch (token.kind)
        {
            case Token::Kind::kCommand:
            {
                // Check for \frac{a}{b} pattern
                if (token.value == "\\frac" && i + 4 < tokens.size() &&
                    tokens[i + 1].kind == Token::Kind::kOpenBrace)
                {
                    // Collect numerator
                    std::string numerator;
                    size_t j = i + 2;
                    int brace_depth = 1;
                    while (j < tokens.size() && brace_depth > 0)
                    {
                        if (tokens[j].kind == Token::Kind::kOpenBrace)
                        {
                            brace_depth++;
                        }
                        else if (tokens[j].kind == Token::Kind::kCloseBrace)
                        {
                            brace_depth--;
                            if (brace_depth == 0)
                            {
                                break;
                            }
                        }
                        numerator += tokens[j].value;
                        j++;
                    }
                    j++; // skip closing brace

                    // Collect denominator
                    std::string denominator;
                    if (j < tokens.size() && tokens[j].kind == Token::Kind::kOpenBrace)
                    {
                        j++; // skip opening brace
                        brace_depth = 1;
                        while (j < tokens.size() && brace_depth > 0)
                        {
                            if (tokens[j].kind == Token::Kind::kOpenBrace)
                            {
                                brace_depth++;
                            }
                            else if (tokens[j].kind == Token::Kind::kCloseBrace)
                            {
                                brace_depth--;
                                if (brace_depth == 0)
                                {
                                    break;
                                }
                            }
                            denominator += tokens[j].value;
                            j++;
                        }
                        j++; // skip closing brace
                    }

                    result += numerator + "/" + denominator;
                    i = j;
                    continue;
                }

                // Check for \sqrt{x} pattern
                if (token.value == "\\sqrt" && i + 2 < tokens.size() &&
                    tokens[i + 1].kind == Token::Kind::kOpenBrace)
                {
                    std::string content;
                    size_t j = i + 2;
                    int brace_depth = 1;
                    while (j < tokens.size() && brace_depth > 0)
                    {
                        if (tokens[j].kind == Token::Kind::kOpenBrace)
                        {
                            brace_depth++;
                        }
                        else if (tokens[j].kind == Token::Kind::kCloseBrace)
                        {
                            brace_depth--;
                            if (brace_depth == 0)
                            {
                                break;
                            }
                        }
                        content += tokens[j].value;
                        j++;
                    }
                    j++; // skip closing brace

                    result += "\xE2\x88\x9A(" + content + ")"; // √(content)
                    i = j;
                    continue;
                }

                // Check for \text{...}, \mathrm{...}, etc. (pass through content)
                if ((token.value == "\\text" || token.value == "\\mathrm" ||
                     token.value == "\\mathbf" || token.value == "\\mathit") &&
                    i + 2 < tokens.size() && tokens[i + 1].kind == Token::Kind::kOpenBrace)
                {
                    std::string content;
                    size_t j = i + 2;
                    int brace_depth = 1;
                    while (j < tokens.size() && brace_depth > 0)
                    {
                        if (tokens[j].kind == Token::Kind::kOpenBrace)
                        {
                            brace_depth++;
                        }
                        else if (tokens[j].kind == Token::Kind::kCloseBrace)
                        {
                            brace_depth--;
                            if (brace_depth == 0)
                            {
                                break;
                            }
                        }
                        content += tokens[j].value;
                        j++;
                    }
                    j++; // skip closing brace

                    result += content;
                    i = j;
                    continue;
                }

                // Look up symbol
                auto it = symbols_.find(token.value);
                if (it != symbols_.end())
                {
                    result += it->second;
                }
                else
                {
                    // Common function names: render upright
                    static const std::vector<std::string> kFunctions = {
                        "\\sin",    "\\cos",    "\\tan",  "\\sec",  "\\csc",  "\\cot", "\\arcsin",
                        "\\arccos", "\\arctan", "\\sinh", "\\cosh", "\\tanh", "\\log", "\\ln",
                        "\\exp",    "\\lim",    "\\sup",  "\\inf",  "\\max",  "\\min", "\\det",
                        "\\dim",    "\\ker",    "\\arg",  "\\deg",  "\\gcd",  "\\hom", "\\mod",
                    };
                    auto func_it = std::find(kFunctions.begin(), kFunctions.end(), token.value);
                    if (func_it != kFunctions.end())
                    {
                        // Strip backslash and render as text
                        result += token.value.substr(1);
                    }
                    else
                    {
                        // Unknown command — render as-is
                        result += token.value;
                    }
                }
                i++;
                break;
            }

            case Token::Kind::kSuperscript:
            {
                // Next token is the superscript content
                i++;
                if (i < tokens.size())
                {
                    if (tokens[i].kind == Token::Kind::kOpenBrace)
                    {
                        // Collect braced content
                        std::string content;
                        i++; // skip open brace
                        int brace_depth = 1;
                        while (i < tokens.size() && brace_depth > 0)
                        {
                            if (tokens[i].kind == Token::Kind::kOpenBrace)
                            {
                                brace_depth++;
                            }
                            else if (tokens[i].kind == Token::Kind::kCloseBrace)
                            {
                                brace_depth--;
                                if (brace_depth == 0)
                                {
                                    break;
                                }
                            }
                            content += tokens[i].value;
                            i++;
                        }
                        i++; // skip close brace
                        result += to_superscript(content);
                    }
                    else
                    {
                        result += to_superscript(tokens[i].value);
                        i++;
                    }
                }
                break;
            }

            case Token::Kind::kSubscript:
            {
                // Next token is the subscript content
                i++;
                if (i < tokens.size())
                {
                    if (tokens[i].kind == Token::Kind::kOpenBrace)
                    {
                        // Collect braced content
                        std::string content;
                        i++; // skip open brace
                        int brace_depth = 1;
                        while (i < tokens.size() && brace_depth > 0)
                        {
                            if (tokens[i].kind == Token::Kind::kOpenBrace)
                            {
                                brace_depth++;
                            }
                            else if (tokens[i].kind == Token::Kind::kCloseBrace)
                            {
                                brace_depth--;
                                if (brace_depth == 0)
                                {
                                    break;
                                }
                            }
                            content += tokens[i].value;
                            i++;
                        }
                        i++; // skip close brace
                        result += to_subscript(content);
                    }
                    else
                    {
                        result += to_subscript(tokens[i].value);
                        i++;
                    }
                }
                break;
            }

            case Token::Kind::kText:
                result += token.value;
                i++;
                break;

            case Token::Kind::kOpenBrace:
            case Token::Kind::kCloseBrace:
                // Standalone braces — skip (grouping only)
                i++;
                break;

            default:
                i++;
                break;
        }
    }

    return result;
}

// ═══════════════════════════════════════════════════════
// Unicode superscript/subscript conversion
// ═══════════════════════════════════════════════════════

auto MathRenderer::to_superscript(std::string_view text) -> std::string
{
    // Unicode superscript digits and common characters
    static const std::unordered_map<char, std::string> kSuperMap = {
        {'0', "\xE2\x81\xB0"}, // ⁰
        {'1', "\xC2\xB9"},     // ¹
        {'2', "\xC2\xB2"},     // ²
        {'3', "\xC2\xB3"},     // ³
        {'4', "\xE2\x81\xB4"}, // ⁴
        {'5', "\xE2\x81\xB5"}, // ⁵
        {'6', "\xE2\x81\xB6"}, // ⁶
        {'7', "\xE2\x81\xB7"}, // ⁷
        {'8', "\xE2\x81\xB8"}, // ⁸
        {'9', "\xE2\x81\xB9"}, // ⁹
        {'+', "\xE2\x81\xBA"}, // ⁺
        {'-', "\xE2\x81\xBB"}, // ⁻
        {'=', "\xE2\x81\xBC"}, // ⁼
        {'(', "\xE2\x81\xBD"}, // ⁽
        {')', "\xE2\x81\xBE"}, // ⁾
        {'n', "\xE2\x81\xBF"}, // ⁿ
        {'i', "\xE2\x81\xB1"}, // ⁱ
    };

    std::string result;
    for (char ch : text)
    {
        auto it = kSuperMap.find(ch);
        if (it != kSuperMap.end())
        {
            result += it->second;
        }
        else
        {
            // Fall back to HTML superscript for unsupported chars
            result += "<sup>" + std::string(1, ch) + "</sup>";
        }
    }
    return result;
}

auto MathRenderer::to_subscript(std::string_view text) -> std::string
{
    // Unicode subscript digits and common characters
    static const std::unordered_map<char, std::string> kSubMap = {
        {'0', "\xE2\x82\x80"}, // ₀
        {'1', "\xE2\x82\x81"}, // ₁
        {'2', "\xE2\x82\x82"}, // ₂
        {'3', "\xE2\x82\x83"}, // ₃
        {'4', "\xE2\x82\x84"}, // ₄
        {'5', "\xE2\x82\x85"}, // ₅
        {'6', "\xE2\x82\x86"}, // ₆
        {'7', "\xE2\x82\x87"}, // ₇
        {'8', "\xE2\x82\x88"}, // ₈
        {'9', "\xE2\x82\x89"}, // ₉
        {'+', "\xE2\x82\x8A"}, // ₊
        {'-', "\xE2\x82\x8B"}, // ₋
        {'=', "\xE2\x82\x8C"}, // ₌
        {'(', "\xE2\x82\x8D"}, // ₍
        {')', "\xE2\x82\x8E"}, // ₎
        {'a', "\xE2\x82\x90"}, // ₐ
        {'e', "\xE2\x82\x91"}, // ₑ
        {'i', "\xE1\xB5\xA2"}, // ᵢ
        {'o', "\xE2\x82\x92"}, // ₒ
        {'r', "\xE1\xB5\xA3"}, // ᵣ
        {'u', "\xE1\xB5\xA4"}, // ᵤ
        {'v', "\xE1\xB5\xA5"}, // ᵥ
        {'x', "\xE2\x82\x93"}, // ₓ
    };

    std::string result;
    for (char ch : text)
    {
        auto it = kSubMap.find(ch);
        if (it != kSubMap.end())
        {
            result += it->second;
        }
        else
        {
            // Fall back to HTML subscript for unsupported chars
            result += "<sub>" + std::string(1, ch) + "</sub>";
        }
    }
    return result;
}

} // namespace markamp::core
