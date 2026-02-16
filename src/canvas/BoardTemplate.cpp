#include "BoardTemplate.h"

#include "canvas/Board.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>

namespace markamp::canvas
{

auto BoardTemplateLibrary::load_builtins(const std::filesystem::path& resources_dir) -> void
{
    load_from_directory(resources_dir, true);
}

auto BoardTemplateLibrary::load_user_templates(const std::filesystem::path& user_dir) -> void
{
    load_from_directory(user_dir, false);
}

void BoardTemplateLibrary::load_from_directory(const std::filesystem::path& dir, bool builtin)
{
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
    {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        if (entry.path().extension() != ".markboard")
        {
            continue;
        }

        TemplateInfo info;
        info.id = entry.path().stem().string();
        info.name = entry.path().stem().string();
        info.template_path = entry.path();
        info.is_builtin = builtin;

        // Try to infer category from directory structure.
        if (entry.path().parent_path().filename() != dir.filename())
        {
            info.category = entry.path().parent_path().filename().string();
        }
        else
        {
            info.category = builtin ? "General" : "Custom";
        }

        // Read first line of file for description if available.
        std::ifstream file(entry.path());
        if (file.is_open())
        {
            std::string first_line;
            if (std::getline(file, first_line) && first_line.size() > 2 && first_line[0] == '#' &&
                first_line[1] == ' ')
            {
                info.description = first_line.substr(2);
            }
        }

        templates_.push_back(std::move(info));
    }
}

auto BoardTemplateLibrary::templates_in_category(const std::string& category) const
    -> std::vector<const TemplateInfo*>
{
    std::vector<const TemplateInfo*> result;
    for (const auto& tmpl : templates_)
    {
        if (tmpl.category == category)
        {
            result.push_back(&tmpl);
        }
    }
    return result;
}

auto BoardTemplateLibrary::categories() const -> std::vector<std::string>
{
    std::set<std::string> cats;
    for (const auto& tmpl : templates_)
    {
        cats.insert(tmpl.category);
    }
    return {cats.begin(), cats.end()};
}

auto BoardTemplateLibrary::search(const std::string& query) const
    -> std::vector<const TemplateInfo*>
{
    std::vector<const TemplateInfo*> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& tmpl : templates_)
    {
        std::string lower_name = tmpl.name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (lower_name.find(lower_query) != std::string::npos)
        {
            result.push_back(&tmpl);
        }
    }
    return result;
}

auto BoardTemplateLibrary::apply_template(const std::string& template_id) const
    -> std::unique_ptr<Board>
{
    for (const auto& tmpl : templates_)
    {
        if (tmpl.id == template_id)
        {
            // In production, we'd deserialize the .markboard file into a new Board.
            // Stub: return a new empty Board.
            auto board = std::make_unique<Board>();
            // Note: Board naming is handled externally by the file system path.
            return board;
        }
    }
    return nullptr;
}

auto BoardTemplateLibrary::save_as_template(const Board& /*board*/,
                                            const TemplateInfo& info,
                                            const std::filesystem::path& user_dir) -> bool
{
    std::filesystem::create_directories(user_dir);

    const auto file_path = user_dir / (info.id + ".markboard");
    std::ofstream file(file_path);
    if (!file.is_open())
    {
        return false;
    }

    // Write a simple header with metadata.
    file << "# " << info.name << "\n";
    file << "# Category: " << info.category << "\n";
    file << "# Author: " << info.author << "\n";
    file << "# Description: " << info.description << "\n";
    file << "---\n";
    // In production: serialize the full board JSON.
    file << "{\"template\":\"" << info.name << "\"}\n";

    return true;
}

auto BoardTemplateLibrary::all_templates() const -> const std::vector<TemplateInfo>&
{
    return templates_;
}

auto BoardTemplateLibrary::template_count() const -> size_t
{
    return templates_.size();
}

// ── Improvements (#29-30) ───────────────────────────────────────

auto BoardTemplateLibrary::delete_template(const std::string& template_id) -> bool
{
    auto iter = std::find_if(templates_.begin(),
                             templates_.end(),
                             [&template_id](const TemplateInfo& tmpl)
                             { return tmpl.id == template_id && !tmpl.is_builtin; });
    if (iter == templates_.end())
    {
        return false;
    }
    templates_.erase(iter);
    return true;
}

auto BoardTemplateLibrary::sort_by_rating() -> void
{
    std::sort(templates_.begin(),
              templates_.end(),
              [](const TemplateInfo& lhs, const TemplateInfo& rhs)
              { return lhs.rating > rhs.rating; });
}

auto BoardTemplateLibrary::sort_by_popularity() -> void
{
    std::sort(templates_.begin(),
              templates_.end(),
              [](const TemplateInfo& lhs, const TemplateInfo& rhs)
              { return lhs.usage_count > rhs.usage_count; });
}

// ── Batch 10 (#57-58) ─────────────────────────────────────────────

auto BoardTemplateLibrary::duplicate_template(const std::string& source_id,
                                              const std::string& new_id,
                                              const std::string& new_name) -> bool
{
    const auto* source = template_by_id(source_id);
    if (source == nullptr)
    {
        return false;
    }

    TemplateInfo copy = *source;
    copy.id = new_id;
    copy.name = new_name;
    copy.is_builtin = false;
    copy.usage_count = 0;
    copy.rating = 0.0;
    templates_.push_back(std::move(copy));
    return true;
}

auto BoardTemplateLibrary::template_by_id(const std::string& template_id) const
    -> const TemplateInfo*
{
    for (const auto& tmpl : templates_)
    {
        if (tmpl.id == template_id)
        {
            return &tmpl;
        }
    }
    return nullptr;
}

} // namespace markamp::canvas
