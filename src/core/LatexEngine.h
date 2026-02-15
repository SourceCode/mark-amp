/// @file LatexEngine.h
/// @brief V4 Phase 18 – Inline LaTeX Rendering Enhancement.

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// A user-defined LaTeX macro.
struct LatexMacro
{
    std::string name;      ///< e.g. "\\R"
    std::string expansion; ///< e.g. "\\mathbb{R}"
    int arg_count{0};      ///< Number of arguments (#1, #2, …)
};

/// Result of rendering LaTeX.
struct LatexRenderResult
{
    std::string output;
    bool success{true};
    std::string error;
    int equation_number{0};

    enum class Format : uint8_t
    {
        kUnicode,
        kHtml,
        kSvg
    } format{Format::kUnicode};
};

/// A parsed LaTeX environment (align, matrix, etc.).
struct LatexEnvironment
{
    std::string name; ///< "align", "matrix", "cases"
    std::string content;
    bool numbered{true};
};

/// Engine wrapping MathRenderer with macro expansion, equation numbering,
/// and environment support.
class LatexEngine
{
public:
    LatexEngine(EventBus& event_bus, Config& config);

    /// Render inline math ($...$).
    [[nodiscard]] auto render_inline(const std::string& latex) const -> LatexRenderResult;

    /// Render display math ($$...$$).
    [[nodiscard]] auto render_display(const std::string& latex) -> LatexRenderResult;

    /// Render a specific environment (\begin{...}...\end{...}).
    [[nodiscard]] auto render_environment(const LatexEnvironment& env) -> LatexRenderResult;

    /// Scan a full markdown document, replace $...$ and $$...$$ with rendered output.
    [[nodiscard]] auto process_document(const std::string& markdown) -> std::string;

    /// Register a custom macro (\newcommand style).
    auto register_macro(const LatexMacro& macro) -> void;

    /// Load macros from a preamble string (\newcommand{} definitions).
    auto load_preamble(const std::string& preamble) -> int;

    /// Get the next equation number and increment counter.
    [[nodiscard]] auto next_equation_number() -> int;

    /// Reset equation numbering to 1.
    auto reset_numbering() -> void;

    /// Get all registered macros.
    [[nodiscard]] auto macros() const -> const std::unordered_map<std::string, LatexMacro>&;

    /// Expand macros in a LaTeX expression.
    [[nodiscard]] auto expand_macros(const std::string& latex) const -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;

    std::unordered_map<std::string, LatexMacro> macros_;
    int equation_counter_{1};

    /// Render core LaTeX (after macro expansion) using basic symbol table.
    [[nodiscard]] static auto render_core(const std::string& latex, bool is_display) -> std::string;
};

} // namespace markamp::core
