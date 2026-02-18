/// @file FileTemplateEngine.cpp
/// @brief Phase 20 – File template engine implementation.

#include "core/FileTemplateEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

FileTemplateEngine::FileTemplateEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
    register_builtins();
}

// ============================================================================
// Template Management
// ============================================================================

void FileTemplateEngine::register_template(FileTemplate tmpl)
{
    auto template_id = tmpl.id;
    templates_[template_id] = std::move(tmpl);
}

auto FileTemplateEngine::unregister_template(const std::string& template_id) -> bool
{
    return templates_.erase(template_id) > 0;
}

auto FileTemplateEngine::list_templates() const -> std::vector<FileTemplate>
{
    std::vector<FileTemplate> result;
    result.reserve(templates_.size());
    for (const auto& [template_id, tmpl] : templates_)
    {
        result.push_back(tmpl);
    }
    std::sort(result.begin(),
              result.end(),
              [](const FileTemplate& tmpl_a, const FileTemplate& tmpl_b)
              { return tmpl_a.name < tmpl_b.name; });
    return result;
}

auto FileTemplateEngine::templates_for_extension(const std::string& ext) const
    -> std::vector<FileTemplate>
{
    std::vector<FileTemplate> result;
    for (const auto& [template_id, tmpl] : templates_)
    {
        if (tmpl.extension == ext)
        {
            result.push_back(tmpl);
        }
    }
    return result;
}

auto FileTemplateEngine::search_templates(const std::string& query) const
    -> std::vector<FileTemplate>
{
    std::vector<FileTemplate> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    for (const auto& [template_id, tmpl] : templates_)
    {
        std::string lower_name = tmpl.name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        std::string lower_desc = tmpl.description;
        std::transform(lower_desc.begin(),
                       lower_desc.end(),
                       lower_desc.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        if (lower_name.find(lower_query) != std::string::npos ||
            lower_desc.find(lower_query) != std::string::npos)
        {
            result.push_back(tmpl);
        }
    }
    return result;
}

auto FileTemplateEngine::find_template(const std::string& template_id) const -> const FileTemplate*
{
    auto iter = templates_.find(template_id);
    if (iter != templates_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto FileTemplateEngine::template_count() const -> std::size_t
{
    return templates_.size();
}

// ============================================================================
// File Creation
// ============================================================================

auto FileTemplateEngine::create_from_template(
    const std::string& template_id,
    const std::string& output_path,
    const std::unordered_map<std::string, std::string>& variables)
    -> std::expected<std::string, std::string>
{
    auto iter = templates_.find(template_id);
    if (iter == templates_.end())
    {
        return std::unexpected("Template not found: " + template_id);
    }

    const auto& tmpl = iter->second;

    // Check required variables.
    for (const auto& var : tmpl.variables)
    {
        if (var.required && !variables.contains(var.name) && var.default_value.empty())
        {
            return std::unexpected("Missing required variable: " + var.name);
        }
    }

    // Build substitution map with defaults.
    auto effective_vars = variables;
    for (const auto& var : tmpl.variables)
    {
        if (!effective_vars.contains(var.name) && !var.default_value.empty())
        {
            effective_vars[var.name] = var.default_value;
        }
    }

    auto content = substitute_variables(tmpl.content_template, effective_vars);

    // Publish event.
    events::FileCreatedFromTemplateEvent evt;
    evt.template_id = template_id;
    evt.path = output_path;
    event_bus_.publish(evt);

    return content;
}

// ============================================================================
// User Templates
// ============================================================================

auto FileTemplateEngine::add_user_template(const std::string& name,
                                           const std::string& content,
                                           const std::string& extension,
                                           const std::string& category) -> std::string
{
    auto template_id = "user-" + std::to_string(next_user_id_++);

    FileTemplate tmpl;
    tmpl.id = template_id;
    tmpl.name = name;
    tmpl.description = "User-defined template";
    tmpl.category = category;
    tmpl.extension = extension;
    tmpl.content_template = content;
    tmpl.is_builtin = false;

    templates_[template_id] = std::move(tmpl);
    return template_id;
}

// ============================================================================
// Utilities
// ============================================================================

auto FileTemplateEngine::substitute_variables(
    const std::string& content, const std::unordered_map<std::string, std::string>& variables)
    -> std::string
{
    std::string result = content;

    for (const auto& [var_name, var_value] : variables)
    {
        const std::string kPlaceholder = "{{" + var_name + "}}";
        std::size_t pos = 0;
        while ((pos = result.find(kPlaceholder, pos)) != std::string::npos)
        {
            result.replace(pos, kPlaceholder.length(), var_value);
            pos += var_value.length();
        }
    }

    return result;
}

auto FileTemplateEngine::categories() const -> std::vector<std::string>
{
    std::set<std::string> cats;
    for (const auto& [template_id, tmpl] : templates_)
    {
        if (!tmpl.category.empty())
        {
            cats.insert(tmpl.category);
        }
    }
    return {cats.begin(), cats.end()};
}

void FileTemplateEngine::clear()
{
    templates_.clear();
}

// ============================================================================
// Built-in Templates
// ============================================================================

void FileTemplateEngine::register_builtins()
{
    // Blank Markdown document.
    {
        FileTemplate tmpl;
        tmpl.id = "blank-md";
        tmpl.name = "Blank Document";
        tmpl.description = "A blank Markdown document with a title heading.";
        tmpl.category = "Document";
        tmpl.extension = "md";
        tmpl.content_template = "# {{title}}\n\n";
        tmpl.is_builtin = true;

        TemplateVariable title_var;
        title_var.name = "title";
        title_var.default_value = "Untitled";
        title_var.description = "Document title";
        tmpl.variables.push_back(std::move(title_var));

        templates_[tmpl.id] = std::move(tmpl);
    }

    // Meeting notes.
    {
        FileTemplate tmpl;
        tmpl.id = "meeting-notes";
        tmpl.name = "Meeting Notes";
        tmpl.description = "Template for meeting notes with attendees and action items.";
        tmpl.category = "Document";
        tmpl.extension = "md";
        tmpl.content_template = "# {{title}}\n\n"
                                "**Date:** {{date}}\n"
                                "**Attendees:** {{attendees}}\n\n"
                                "## Agenda\n\n- \n\n"
                                "## Notes\n\n\n\n"
                                "## Action Items\n\n- [ ] \n";
        tmpl.is_builtin = true;

        TemplateVariable title_var;
        title_var.name = "title";
        title_var.default_value = "Meeting Notes";
        tmpl.variables.push_back(std::move(title_var));

        TemplateVariable date_var;
        date_var.name = "date";
        date_var.default_value = "today";
        tmpl.variables.push_back(std::move(date_var));

        TemplateVariable attendees_var;
        attendees_var.name = "attendees";
        attendees_var.default_value = "";
        tmpl.variables.push_back(std::move(attendees_var));

        templates_[tmpl.id] = std::move(tmpl);
    }

    // Blank notebook.
    {
        FileTemplate tmpl;
        tmpl.id = "blank-notebook";
        tmpl.name = "Blank Notebook";
        tmpl.description = "An empty Jupyter notebook with a single code cell.";
        tmpl.category = "Notebook";
        tmpl.extension = "ipynb";
        tmpl.content_template = R"({
    "nbformat": 4,
    "nbformat_minor": 5,
    "metadata": {
        "kernelspec": {
            "name": "{{kernel}}",
            "display_name": "{{kernel_display}}",
            "language": "{{language}}"
        },
        "language_info": {
            "name": "{{language}}"
        }
    },
    "cells": [
        {
            "cell_type": "code",
            "source": "",
            "metadata": {},
            "outputs": [],
            "execution_count": null
        }
    ]
})";
        tmpl.is_builtin = true;

        TemplateVariable kernel_var;
        kernel_var.name = "kernel";
        kernel_var.default_value = "python3";
        tmpl.variables.push_back(std::move(kernel_var));

        TemplateVariable kernel_display_var;
        kernel_display_var.name = "kernel_display";
        kernel_display_var.default_value = "Python 3";
        tmpl.variables.push_back(std::move(kernel_display_var));

        TemplateVariable lang_var;
        lang_var.name = "language";
        lang_var.default_value = "python";
        tmpl.variables.push_back(std::move(lang_var));

        templates_[tmpl.id] = std::move(tmpl);
    }

    // Canvas board.
    {
        FileTemplate tmpl;
        tmpl.id = "blank-canvas";
        tmpl.name = "Blank Canvas";
        tmpl.description = "An empty canvas board.";
        tmpl.category = "Canvas";
        tmpl.extension = "canvas";
        tmpl.content_template = R"({
    "nodes": [],
    "edges": [],
    "metadata": {
        "name": "{{title}}",
        "created": "{{date}}"
    }
})";
        tmpl.is_builtin = true;

        TemplateVariable title_var;
        title_var.name = "title";
        title_var.default_value = "Untitled Canvas";
        tmpl.variables.push_back(std::move(title_var));

        TemplateVariable date_var;
        date_var.name = "date";
        date_var.default_value = "today";
        tmpl.variables.push_back(std::move(date_var));

        templates_[tmpl.id] = std::move(tmpl);
    }

    // Daily journal.
    {
        FileTemplate tmpl;
        tmpl.id = "daily-journal";
        tmpl.name = "Daily Journal";
        tmpl.description = "Template for daily journal entries.";
        tmpl.category = "Document";
        tmpl.extension = "md";
        tmpl.content_template = "# {{date}}\n\n"
                                "## Goals\n\n- \n\n"
                                "## Notes\n\n\n\n"
                                "## Reflections\n\n\n";
        tmpl.is_builtin = true;

        TemplateVariable date_var;
        date_var.name = "date";
        date_var.default_value = "today";
        tmpl.variables.push_back(std::move(date_var));

        templates_[tmpl.id] = std::move(tmpl);
    }
}

} // namespace markamp::core
