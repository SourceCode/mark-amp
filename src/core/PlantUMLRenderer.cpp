// ============================================================================
// File: src/core/PlantUMLRenderer.cpp
// Phase 30: Diagram Rendering Extended — PlantUML renderer implementation
// ============================================================================
#include "PlantUMLRenderer.h"

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

void PlantUMLRenderer::set_jar_path(const std::filesystem::path& jar_path)
{
    jar_path_ = jar_path;
}

void PlantUMLRenderer::set_server_url(const std::string& url)
{
    server_url_ = url;
}

auto PlantUMLRenderer::render(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    // Check cache
    const auto hash = compute_hash(source);
    if (const auto cached = cache_.find(hash); cached != cache_.end())
    {
        DiagramResult result;
        result.svg = cached->second;
        result.render_time_ms = 0;
        return result;
    }

    // Prefer jar, fall back to server
    auto result_or_error =
        (!jar_path_.empty()) ? render_via_jar(source) : render_via_server(source);

    if (result_or_error.has_value() && !result_or_error->svg.empty())
    {
        cache_[hash] = result_or_error->svg;
    }

    return result_or_error;
}

auto PlantUMLRenderer::is_available() const -> bool
{
    if (!jar_path_.empty())
    {
        std::error_code error_code;
        return std::filesystem::exists(jar_path_, error_code) && !error_code;
    }
    return !server_url_.empty();
}

auto PlantUMLRenderer::language_id() const -> std::string
{
    return "plantuml";
}

auto PlantUMLRenderer::display_name() const -> std::string
{
    return "PlantUML";
}

auto PlantUMLRenderer::render_via_jar(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    const auto start = std::chrono::steady_clock::now();

    // Write source to temp file
    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_input = temp_dir / "markamp_plantuml_input.puml";
    auto temp_output = temp_dir / "markamp_plantuml_input.svg";

    {
        std::ofstream ofs(temp_input);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to create temp file for PlantUML input");
        }
        ofs << source;
    }

    // Execute PlantUML jar
    const auto cmd =
        "java -jar \"" + jar_path_.string() + "\" -tsvg \"" + temp_input.string() + "\"";
    const int exit_code = std::system(cmd.c_str());

    DiagramResult result;

    if (exit_code != 0)
    {
        result.diagnostics.push_back(
            {DiagramDiagnosticLevel::kError,
             "PlantUML process exited with code " + std::to_string(exit_code),
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

        // Cleanup
        std::filesystem::remove(temp_output, error_code);
    }

    std::filesystem::remove(temp_input, error_code);

    const auto end = std::chrono::steady_clock::now();
    result.render_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

auto PlantUMLRenderer::render_via_server(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    if (server_url_.empty())
    {
        return std::unexpected("PlantUML server URL not configured");
    }

    const auto start = std::chrono::steady_clock::now();

    // Construct HTTP POST to PlantUML server.
    // Server endpoint: POST <server_url>/svg with Content-Type: text/plain
    // Request body: the PlantUML diagram source.
    // Response: SVG content.
    const std::string endpoint = server_url_ + "/svg";

    // libcurl integration point for HTTP POST:
    // curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    // curl_easy_setopt(curl, CURLOPT_POST, 1L);
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, source.c_str());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, source.size());
    // headers: "Content-Type: text/plain"
    (void)source;
    (void)endpoint;

    const auto end = std::chrono::steady_clock::now();

    DiagramResult result;
    result.render_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    result.diagnostics.push_back(
        {DiagramDiagnosticLevel::kInfo,
         "Server rendering requires libcurl runtime (endpoint: " + endpoint + ")",
         -1});

    return result;
}

auto PlantUMLRenderer::compute_hash(const std::string& source) -> std::string
{
    // Simple hash for caching
    auto hash = std::hash<std::string>{}(source);
    return std::to_string(hash);
}

} // namespace markamp::core
