#pragma once

/// @file SemanticTokenProvider.h
/// @brief V9 Phase 19 — Semantic token provider for markdown-aware highlighting.
///
/// Maps SyntaxHighlighter TokenType to LSP-style SemanticToken with modifiers,
/// and adds markdown-specific structural classification.

#include "CodeIntelligenceTypes.h"

#include <sstream>
#include <string>
#include <vector>

namespace markamp::core
{

/// Header-only provider for semantic tokens in markdown documents.
///
/// Produces SemanticToken entries for headings, links, emphasis, code blocks,
/// footnotes, frontmatter, and other structural elements.
///
/// Usage:
/// ```cpp
/// SemanticTokenProvider provider;
/// auto tokens = provider.provide_tokens("# Hello\n\n**bold** text");
/// // tokens contain heading, strong emphasis, etc.
/// ```
class SemanticTokenProvider
{
public:
    SemanticTokenProvider() = default;

    /// Provide semantic tokens for the entire document.
    /// @param content Full document content
    /// @return Vector of SemanticToken entries
    [[nodiscard]] static auto provide_tokens(const std::string& content)
        -> std::vector<SemanticToken>
    {
        std::vector<SemanticToken> tokens;
        std::istringstream stream(content);
        std::string line_text;
        int line_num = 0;
        bool in_frontmatter = false;
        bool frontmatter_started = false;
        bool in_code_fence = false;

        while (std::getline(stream, line_text))
        {
            // YAML frontmatter detection
            if (line_num == 0 && line_text == "---")
            {
                in_frontmatter = true;
                frontmatter_started = true;
                tokens.push_back({line_num,
                                  0,
                                  static_cast<int>(line_text.size()),
                                  SemanticTokenType::kFrontmatter,
                                  SemanticTokenModifier::kDeclaration});
                ++line_num;
                continue;
            }

            if (in_frontmatter)
            {
                if (line_text == "---")
                {
                    in_frontmatter = false;
                    tokens.push_back({line_num,
                                      0,
                                      static_cast<int>(line_text.size()),
                                      SemanticTokenType::kFrontmatter,
                                      SemanticTokenModifier::kNone});
                }
                else
                {
                    tokens.push_back({line_num,
                                      0,
                                      static_cast<int>(line_text.size()),
                                      SemanticTokenType::kFrontmatter,
                                      SemanticTokenModifier::kNone});
                }
                ++line_num;
                continue;
            }

            // Code fence detection
            if (line_text.size() >= 3 && line_text.substr(0, 3) == "```")
            {
                in_code_fence = !in_code_fence;
                tokens.push_back({line_num,
                                  0,
                                  static_cast<int>(line_text.size()),
                                  SemanticTokenType::kCode,
                                  SemanticTokenModifier::kDeclaration});
                ++line_num;
                continue;
            }

            if (in_code_fence)
            {
                tokens.push_back({line_num,
                                  0,
                                  static_cast<int>(line_text.size()),
                                  SemanticTokenType::kCode,
                                  SemanticTokenModifier::kReadonly});
                ++line_num;
                continue;
            }

            // Heading detection
            if (!line_text.empty() && line_text[0] == '#')
            {
                int level = 0;
                while (level < static_cast<int>(line_text.size()) &&
                       line_text[static_cast<std::string::size_type>(level)] == '#')
                {
                    ++level;
                }
                if (level <= 6 && level < static_cast<int>(line_text.size()) &&
                    line_text[static_cast<std::string::size_type>(level)] == ' ')
                {
                    tokens.push_back({line_num,
                                      0,
                                      static_cast<int>(line_text.size()),
                                      SemanticTokenType::kHeading,
                                      SemanticTokenModifier::kDeclaration});
                    ++line_num;
                    continue;
                }
            }

            // Blockquote detection
            if (!line_text.empty() && line_text[0] == '>')
            {
                tokens.push_back({line_num,
                                  0,
                                  static_cast<int>(line_text.size()),
                                  SemanticTokenType::kBlockquote,
                                  SemanticTokenModifier::kNone});
            }

            // List item detection
            if (line_text.size() >= 2)
            {
                auto trimmed = line_text;
                auto first_non_space = trimmed.find_first_not_of(" \t");
                if (first_non_space != std::string::npos)
                {
                    const char marker = trimmed[first_non_space];
                    if (marker == '-' || marker == '*' || marker == '+')
                    {
                        if (first_non_space + 1 < trimmed.size() &&
                            trimmed[first_non_space + 1] == ' ')
                        {
                            tokens.push_back({line_num,
                                              static_cast<int>(first_non_space),
                                              2,
                                              SemanticTokenType::kListItem,
                                              SemanticTokenModifier::kNone});
                        }
                    }
                }
            }

            // Inline element detection within the line
            tokenize_inline(line_text, line_num, tokens);

            ++line_num;
        }

        // Suppress unused variable warning
        (void)frontmatter_started;

        return tokens;
    }

private:
    /// Tokenize inline elements within a line (emphasis, links, code, footnotes, etc.)
    static void
    tokenize_inline(const std::string& line_text, int line_num, std::vector<SemanticToken>& tokens)
    {
        std::string::size_type pos = 0;
        while (pos < line_text.size())
        {
            // Strong emphasis: **text**
            if (pos + 1 < line_text.size() && line_text[pos] == '*' && line_text[pos + 1] == '*')
            {
                auto close = line_text.find("**", pos + 2);
                if (close != std::string::npos)
                {
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 2),
                                      SemanticTokenType::kStrong,
                                      SemanticTokenModifier::kNone});
                    pos = close + 2;
                    continue;
                }
            }

            // Emphasis: *text* (but not **)
            if (line_text[pos] == '*' && (pos + 1 >= line_text.size() || line_text[pos + 1] != '*'))
            {
                auto close = line_text.find('*', pos + 1);
                if (close != std::string::npos && close > pos + 1)
                {
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 1),
                                      SemanticTokenType::kEmphasis,
                                      SemanticTokenModifier::kNone});
                    pos = close + 1;
                    continue;
                }
            }

            // Inline code: `text`
            if (line_text[pos] == '`' && (pos + 1 >= line_text.size() || line_text[pos + 1] != '`'))
            {
                auto close = line_text.find('`', pos + 1);
                if (close != std::string::npos)
                {
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 1),
                                      SemanticTokenType::kCode,
                                      SemanticTokenModifier::kReadonly});
                    pos = close + 1;
                    continue;
                }
            }

            // Image: ![alt](url)
            if (pos + 1 < line_text.size() && line_text[pos] == '!' && line_text[pos + 1] == '[')
            {
                auto close_bracket = line_text.find(']', pos + 2);
                if (close_bracket != std::string::npos && close_bracket + 1 < line_text.size() &&
                    line_text[close_bracket + 1] == '(')
                {
                    auto close_paren = line_text.find(')', close_bracket + 2);
                    if (close_paren != std::string::npos)
                    {
                        tokens.push_back({line_num,
                                          static_cast<int>(pos),
                                          static_cast<int>(close_paren - pos + 1),
                                          SemanticTokenType::kImage,
                                          SemanticTokenModifier::kNone});
                        pos = close_paren + 1;
                        continue;
                    }
                }
            }

            // Wiki-link: [[target]]
            if (pos + 1 < line_text.size() && line_text[pos] == '[' && line_text[pos + 1] == '[')
            {
                auto close = line_text.find("]]", pos + 2);
                if (close != std::string::npos)
                {
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 2),
                                      SemanticTokenType::kLink,
                                      SemanticTokenModifier::kReference});
                    pos = close + 2;
                    continue;
                }
            }

            // Link: [text](url)
            if (line_text[pos] == '[' && (pos == 0 || line_text[pos - 1] != '!'))
            {
                auto close_bracket = line_text.find(']', pos + 1);
                if (close_bracket != std::string::npos && close_bracket + 1 < line_text.size() &&
                    line_text[close_bracket + 1] == '(')
                {
                    auto close_paren = line_text.find(')', close_bracket + 2);
                    if (close_paren != std::string::npos)
                    {
                        tokens.push_back({line_num,
                                          static_cast<int>(pos),
                                          static_cast<int>(close_paren - pos + 1),
                                          SemanticTokenType::kLink,
                                          SemanticTokenModifier::kNone});
                        pos = close_paren + 1;
                        continue;
                    }
                }
            }

            // Footnote reference: [^id]
            if (pos + 1 < line_text.size() && line_text[pos] == '[' && line_text[pos + 1] == '^')
            {
                auto close = line_text.find(']', pos + 2);
                if (close != std::string::npos)
                {
                    // Check if it's a definition or reference
                    auto modifier = (close + 1 < line_text.size() && line_text[close + 1] == ':')
                                        ? SemanticTokenModifier::kDeclaration
                                        : SemanticTokenModifier::kReference;
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 1),
                                      SemanticTokenType::kFootnoteRef,
                                      modifier});
                    pos = close + 1;
                    continue;
                }
            }

            // HTML tag: <tag> or </tag>
            if (line_text[pos] == '<' && pos + 1 < line_text.size() &&
                (std::isalpha(static_cast<unsigned char>(line_text[pos + 1])) != 0 ||
                 line_text[pos + 1] == '/'))
            {
                auto close = line_text.find('>', pos + 1);
                if (close != std::string::npos)
                {
                    tokens.push_back({line_num,
                                      static_cast<int>(pos),
                                      static_cast<int>(close - pos + 1),
                                      SemanticTokenType::kHtmlTag,
                                      SemanticTokenModifier::kNone});
                    pos = close + 1;
                    continue;
                }
            }

            ++pos;
        }
    }
};

} // namespace markamp::core
