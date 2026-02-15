/// @file NoteTemplateEngine.cpp
/// @brief V4 Phase 14 – Note Templates with variable substitution implementation.

#include "core/NoteTemplateEngine.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// NoteTemplateDefinition
// ============================================================================

auto NoteTemplateDefinition::variable_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(variables.size());
    for (const auto& var : variables)
    {
        names.push_back(var.name);
    }
    return names;
}

// ============================================================================
// NoteTemplateEngine — Constructor
// ============================================================================

NoteTemplateEngine::NoteTemplateEngine(EventBus& event_bus,
                                       Config& config,
                                       VaultService& vault_service)
    : event_bus_(event_bus)
    , config_(config)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Templates folder
// ============================================================================

auto NoteTemplateEngine::templates_folder() const -> std::filesystem::path
{
    std::string folder_name = "_templates";
    const auto custom = config_.get_string("templates.folder");
    if (!custom.empty())
    {
        folder_name = custom;
    }
    return vault_service_.vault_path() / folder_name;
}

// ============================================================================
// Load templates
// ============================================================================

auto NoteTemplateEngine::load_templates() -> void
{
    templates_.clear();
    const auto folder = templates_folder();

    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder))
    {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const auto& path = entry.path();
        if (path.extension() != ".md")
        {
            continue;
        }

        std::ifstream file(path);
        if (!file.is_open())
        {
            continue;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        NoteTemplateDefinition tmpl;
        tmpl.name = path.stem().string();
        tmpl.file_path = path;
        tmpl.content = content;
        tmpl.variables = parse_variables(content);

        // Extract description from first comment line if present
        if (content.size() > 4 && content.substr(0, 4) == "<!--")
        {
            auto end_comment = content.find("-->");
            if (end_comment != std::string::npos)
            {
                tmpl.description = content.substr(4, end_comment - 4);
                // Trim whitespace
                auto trim_start = tmpl.description.find_first_not_of(" \t\n\r");
                auto trim_end = tmpl.description.find_last_not_of(" \t\n\r");
                if (trim_start != std::string::npos && trim_end != std::string::npos)
                {
                    tmpl.description =
                        tmpl.description.substr(trim_start, trim_end - trim_start + 1);
                }
            }
        }

        templates_.push_back(std::move(tmpl));
    }

    // Sort by name for consistent ordering
    std::sort(templates_.begin(),
              templates_.end(),
              [](const NoteTemplateDefinition& lhs, const NoteTemplateDefinition& rhs)
              { return lhs.name < rhs.name; });
}

// ============================================================================
// List / Get templates
// ============================================================================

auto NoteTemplateEngine::list_templates() const -> std::vector<NoteTemplateDefinition>
{
    return templates_;
}

auto NoteTemplateEngine::get_template(const std::string& name) const
    -> std::optional<NoteTemplateDefinition>
{
    for (const auto& tmpl : templates_)
    {
        if (tmpl.name == name)
        {
            return tmpl;
        }
    }
    return std::nullopt;
}

// ============================================================================
// Apply template
// ============================================================================

auto NoteTemplateEngine::apply(const NoteTemplateDefinition& tmpl,
                               const NoteTemplateContext& context) const -> std::string
{
    std::string result = tmpl.content;

    // Replace all {{variable}} patterns
    static const std::regex var_re(R"(\{\{(\w+)(?::([^}]*))?\}\})");

    std::string output;
    output.reserve(result.size());

    std::sregex_iterator it(result.begin(), result.end(), var_re);
    std::sregex_iterator end;
    size_t last_pos = 0;

    for (; it != end; ++it)
    {
        const auto& match = *it;
        // Append text before this match
        output.append(result, last_pos, static_cast<size_t>(match.position()) - last_pos);

        std::string var_name = match[1].str();
        std::string default_val = match[2].matched ? match[2].str() : "";

        // Skip cursor marker (remove it from output)
        if (var_name == "cursor")
        {
            last_pos = static_cast<size_t>(match.position()) + static_cast<size_t>(match.length());
            continue;
        }

        // Resolve variable
        std::string resolved = resolve_variable(var_name, context);
        if (resolved.empty() && !default_val.empty())
        {
            resolved = default_val;
        }

        output.append(resolved);
        last_pos = static_cast<size_t>(match.position()) + static_cast<size_t>(match.length());
    }

    // Append remaining text
    output.append(result, last_pos);

    return output;
}

// ============================================================================
// Resolve variable
// ============================================================================

auto NoteTemplateEngine::resolve_variable(const std::string& name,
                                          const NoteTemplateContext& context) const -> std::string
{
    // 1. Check explicit context values
    auto ctx_it = context.values.find(name);
    if (ctx_it != context.values.end())
    {
        return ctx_it->second;
    }

    // 2. Check custom variable providers
    auto custom_it = custom_variables_.find(name);
    if (custom_it != custom_variables_.end())
    {
        return custom_it->second();
    }

    // 3. Check built-in variables
    return builtin_variable(name, context);
}

// ============================================================================
// Built-in variables
// ============================================================================

auto NoteTemplateEngine::builtin_variable(const std::string& name,
                                          const NoteTemplateContext& context) const -> std::string
{
    auto time_point = context.timestamp;
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm_buf{};
    localtime_r(&time_t, &tm_buf);

    auto format_time = [&](const std::string& fmt) -> std::string
    {
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, fmt.c_str());
        return oss.str();
    };

    auto get_date_format = [this]() -> std::string
    {
        auto fmt = config_.get_string("templates.date_format");
        return !fmt.empty() ? fmt : "%Y-%m-%d";
    };

    auto get_time_format = [this]() -> std::string
    {
        auto fmt = config_.get_string("templates.time_format");
        return !fmt.empty() ? fmt : "%H:%M";
    };

    if (name == "date")
    {
        return format_time(get_date_format());
    }
    if (name == "time")
    {
        return format_time(get_time_format());
    }
    if (name == "datetime")
    {
        return format_time(get_date_format() + " " + get_time_format());
    }
    if (name == "title")
    {
        return context.document_title;
    }
    if (name == "author")
    {
        return config_.get_string("templates.default_author");
    }
    if (name == "vault")
    {
        return context.vault_name;
    }
    if (name == "yesterday")
    {
        auto yesterday_tp = time_point - std::chrono::hours(24);
        auto yt = std::chrono::system_clock::to_time_t(yesterday_tp);
        std::tm ytm{};
        localtime_r(&yt, &ytm);
        std::ostringstream oss;
        oss << std::put_time(&ytm, get_date_format().c_str());
        return oss.str();
    }
    if (name == "tomorrow")
    {
        auto tomorrow_tp = time_point + std::chrono::hours(24);
        auto tt = std::chrono::system_clock::to_time_t(tomorrow_tp);
        std::tm ttm{};
        localtime_r(&tt, &ttm);
        std::ostringstream oss;
        oss << std::put_time(&ttm, get_date_format().c_str());
        return oss.str();
    }
    if (name == "day")
    {
        return format_time("%A"); // Full day name
    }

    // Unknown built-in: return empty
    return "";
}

// ============================================================================
// Parse variables from template content
// ============================================================================

auto NoteTemplateEngine::parse_variables(const std::string& content) const
    -> std::vector<NoteTemplateVariable>
{
    std::vector<NoteTemplateVariable> variables;
    std::unordered_map<std::string, bool> seen;

    // Standard variables: {{name}} or {{name:default}}
    static const std::regex var_re(R"(\{\{(\w+)(?::([^}]*))?\}\})");
    std::sregex_iterator it(content.begin(), content.end(), var_re);
    std::sregex_iterator end;

    for (; it != end; ++it)
    {
        std::string var_name = (*it)[1].str();

        if (var_name == "cursor")
        {
            continue; // Not a variable
        }

        if (seen.contains(var_name))
        {
            continue;
        }
        seen[var_name] = true;

        NoteTemplateVariable var;
        var.name = var_name;

        if ((*it)[2].matched)
        {
            std::string default_or_spec = (*it)[2].str();

            // Check for choice syntax: {{choice:opt1|opt2|opt3}}
            if (var_name == "choice" || default_or_spec.find('|') != std::string::npos)
            {
                var.type = NoteTemplateVariable::Type::Choice;
                // Split by '|'
                std::istringstream ss(default_or_spec);
                std::string option;
                while (std::getline(ss, option, '|'))
                {
                    var.choices.push_back(option);
                }
                if (!var.choices.empty())
                {
                    var.default_value = var.choices[0];
                }
            }
            else
            {
                var.default_value = default_or_spec;
            }
        }

        // Detect input variables: {{input:Description}}
        if (var_name == "input")
        {
            var.requires_input = true;
            var.description = var.default_value;
            var.default_value.clear();
        }

        // Mark known built-ins
        static const std::unordered_map<std::string, bool> builtins = {
            {"date", true},
            {"time", true},
            {"datetime", true},
            {"title", true},
            {"author", true},
            {"vault", true},
            {"yesterday", true},
            {"tomorrow", true},
            {"day", true},
        };
        if (builtins.contains(var_name))
        {
            var.is_builtin = true;
        }

        variables.push_back(std::move(var));
    }

    return variables;
}

// ============================================================================
// Create from template
// ============================================================================

auto NoteTemplateEngine::create_from_template(const std::string& template_name,
                                              const std::string& document_title,
                                              const NoteTemplateContext& context)
    -> std::expected<std::string, std::string>
{
    auto tmpl_opt = get_template(template_name);
    if (!tmpl_opt.has_value())
    {
        return std::unexpected("Template not found: " + template_name);
    }

    // Build context with title
    NoteTemplateContext ctx = context;
    ctx.document_title = document_title;

    std::string expanded = apply(*tmpl_opt, ctx);

    // Create document via VaultService
    auto doc_result = vault_service_.create_document(document_title);
    if (!doc_result.has_value())
    {
        return std::unexpected("Failed to create document: " + doc_result.error());
    }

    auto doc = *doc_result;
    const std::string doc_id = doc->id();

    // Write expanded content to file
    auto file_path = vault_service_.vault_path() / (document_title + ".md");
    std::ofstream out(file_path);
    if (out.is_open())
    {
        out << expanded;
    }

    // Publish event
    events::TemplateAppliedEvent evt;
    evt.template_name = template_name;
    evt.document_id = doc_id;
    event_bus_.publish(evt);

    return doc_id;
}

// ============================================================================
// Expand template (without creating document)
// ============================================================================

auto NoteTemplateEngine::expand_template(const std::string& template_name,
                                         const NoteTemplateContext& context) const
    -> std::expected<std::string, std::string>
{
    auto tmpl_opt = get_template(template_name);
    if (!tmpl_opt.has_value())
    {
        return std::unexpected("Template not found: " + template_name);
    }

    return apply(*tmpl_opt, context);
}

// ============================================================================
// Register custom variable
// ============================================================================

auto NoteTemplateEngine::register_variable(const std::string& name,
                                           std::function<std::string()> provider) -> void
{
    custom_variables_[name] = std::move(provider);
}

// ============================================================================
// Create template file
// ============================================================================

auto NoteTemplateEngine::create_template(const std::string& name,
                                         const std::string& content,
                                         const std::string& description)
    -> std::expected<void, std::string>
{
    const auto folder = templates_folder();
    std::filesystem::create_directories(folder);

    auto file_path = folder / (name + ".md");
    if (std::filesystem::exists(file_path))
    {
        return std::unexpected("Template already exists: " + name);
    }

    std::ofstream out(file_path);
    if (!out.is_open())
    {
        return std::unexpected("Failed to create template file: " + file_path.string());
    }

    if (!description.empty())
    {
        out << "<!-- " << description << " -->\n";
    }
    out << content;

    // Reload templates
    const_cast<NoteTemplateEngine*>(this)->load_templates();

    return {};
}

// ============================================================================
// Delete template file
// ============================================================================

auto NoteTemplateEngine::delete_template(const std::string& name)
    -> std::expected<void, std::string>
{
    auto tmpl_opt = get_template(name);
    if (!tmpl_opt.has_value())
    {
        return std::unexpected("Template not found: " + name);
    }

    std::error_code ec;
    std::filesystem::remove(tmpl_opt->file_path, ec);
    if (ec)
    {
        return std::unexpected("Failed to delete template: " + ec.message());
    }

    // Remove from in-memory list
    templates_.erase(std::remove_if(templates_.begin(),
                                    templates_.end(),
                                    [&name](const NoteTemplateDefinition& t)
                                    { return t.name == name; }),
                     templates_.end());

    return {};
}

// ============================================================================
// Register variable
// ============================================================================

} // namespace markamp::core
