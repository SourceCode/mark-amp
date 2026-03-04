#pragma once

#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 42: Scroll track decoration (search matches, git changes, diagnostics).
struct ScrollDecoration
{
    enum class Kind
    {
        kSearchMatch,
        kGitAdded,
        kGitModified,
        kGitDeleted,
        kDiagnosticError,
        kDiagnosticWarning,
        kDiagnosticInfo,
    };

    Kind kind{Kind::kSearchMatch};
    int line{0};
    int span{1}; ///< Number of lines this decoration spans.
};

/// Interface for providing scroll track decorations.
class IScrollDecorationProvider
{
public:
    virtual ~IScrollDecorationProvider() = default;

    [[nodiscard]] virtual auto decorations() const -> std::vector<ScrollDecoration> = 0;
    [[nodiscard]] virtual auto provider_id() const -> std::string = 0;
};

/// Concrete: search match decorations.
class SearchMatchDecorationProvider : public IScrollDecorationProvider
{
public:
    void set_matches(std::vector<int> match_lines)
    {
        match_lines_ = std::move(match_lines);
    }

    [[nodiscard]] auto decorations() const -> std::vector<ScrollDecoration> override
    {
        std::vector<ScrollDecoration> result;
        result.reserve(match_lines_.size());
        for (int line : match_lines_)
        {
            result.push_back({.kind = ScrollDecoration::Kind::kSearchMatch, .line = line});
        }
        return result;
    }

    [[nodiscard]] auto provider_id() const -> std::string override
    {
        return "search";
    }

private:
    std::vector<int> match_lines_;
};

/// Concrete: git change decorations.
class GitChangeDecorationProvider : public IScrollDecorationProvider
{
public:
    struct GitChange
    {
        ScrollDecoration::Kind kind;
        int start_line;
        int line_count;
    };

    void set_changes(std::vector<GitChange> changes)
    {
        changes_ = std::move(changes);
    }

    [[nodiscard]] auto decorations() const -> std::vector<ScrollDecoration> override
    {
        std::vector<ScrollDecoration> result;
        result.reserve(changes_.size());
        for (const auto& change : changes_)
        {
            result.push_back(
                {.kind = change.kind, .line = change.start_line, .span = change.line_count});
        }
        return result;
    }

    [[nodiscard]] auto provider_id() const -> std::string override
    {
        return "git";
    }

private:
    std::vector<GitChange> changes_;
};

/// Concrete: diagnostic (error/warning) decorations.
class DiagnosticDecorationProvider : public IScrollDecorationProvider
{
public:
    struct Diagnostic
    {
        ScrollDecoration::Kind kind;
        int line;
    };

    void set_diagnostics(std::vector<Diagnostic> diagnostics)
    {
        diagnostics_ = std::move(diagnostics);
    }

    [[nodiscard]] auto decorations() const -> std::vector<ScrollDecoration> override
    {
        std::vector<ScrollDecoration> result;
        result.reserve(diagnostics_.size());
        for (const auto& diag : diagnostics_)
        {
            result.push_back({.kind = diag.kind, .line = diag.line});
        }
        return result;
    }

    [[nodiscard]] auto provider_id() const -> std::string override
    {
        return "diagnostics";
    }

private:
    std::vector<Diagnostic> diagnostics_;
};

} // namespace markamp::ui
