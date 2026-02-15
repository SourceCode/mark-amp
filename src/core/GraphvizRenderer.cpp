// ============================================================================
// File: src/core/GraphvizRenderer.cpp
// Phase 30: Diagram Rendering Extended — Graphviz renderer implementation
// ============================================================================
#include "GraphvizRenderer.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>

namespace markamp::core
{

void GraphvizRenderer::set_dot_path(const std::string& path)
{
    dot_path_ = path;
}

auto GraphvizRenderer::render(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    const auto hash = compute_hash(source);
    if (const auto cached = cache_.find(hash); cached != cache_.end())
    {
        DiagramResult result;
        result.svg = cached->second;
        result.render_time_ms = 0;
        return result;
    }

    const auto start = std::chrono::steady_clock::now();

    // Write source to temp file
    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_input = temp_dir / "markamp_graphviz_input.dot";
    auto temp_output = temp_dir / "markamp_graphviz_output.svg";

    {
        std::ofstream ofs(temp_input);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to create temp file for Graphviz");
        }
        ofs << source;
    }

    // Execute dot
    const auto cmd =
        dot_path_ + " -Tsvg \"" + temp_input.string() + "\" -o \"" + temp_output.string() + "\"";
    const int exit_code = std::system(cmd.c_str());

    DiagramResult result;

    if (exit_code != 0)
    {
        result.diagnostics.push_back({DiagramDiagnosticLevel::kError,
                                      "Graphviz dot exited with code " + std::to_string(exit_code),
                                      -1});
    }

    // Read SVG output
    std::error_code error_code;
    if (std::filesystem::exists(temp_output, error_code) && !error_code)
    {
        std::ifstream ifs(temp_output);
        std::ostringstream oss;
        oss << ifs.rdbuf();
        result.svg = oss.str();

        cache_[hash] = result.svg;
        std::filesystem::remove(temp_output, error_code);
    }

    std::filesystem::remove(temp_input, error_code);

    const auto end = std::chrono::steady_clock::now();
    result.render_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

auto GraphvizRenderer::is_available() const -> bool
{
    // Check if dot is on PATH
    const auto cmd = dot_path_ + " -V 2>/dev/null";
    return std::system(cmd.c_str()) == 0;
}

auto GraphvizRenderer::language_id() const -> std::string
{
    return "graphviz";
}

auto GraphvizRenderer::display_name() const -> std::string
{
    return "Graphviz (DOT)";
}

auto GraphvizRenderer::compute_hash(const std::string& source) -> std::string
{
    auto hash = std::hash<std::string>{}(source);
    return std::to_string(hash);
}

} // namespace markamp::core
