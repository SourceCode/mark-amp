#include "core/Notebook.h"

#include "core/Events.h"

#include <algorithm>
#include <fstream>
#include <sstream>

// Minimal JSON helpers to avoid external dependency
// We use a simple hand-written JSON read/write for NotebookConf
namespace
{

auto escape_json_string(const std::string& s) -> std::string
{
    std::string result;
    result.reserve(s.size() + 8);
    for (char ch : s)
    {
        switch (ch)
        {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += ch;
                break;
        }
    }
    return result;
}

auto serialize_notebook_conf(const markamp::core::NotebookConf& conf) -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "    \"name\": \"" << escape_json_string(conf.name) << "\",\n";
    oss << "    \"sort\": " << conf.sort << ",\n";
    oss << "    \"closed\": " << (conf.closed ? "true" : "false") << ",\n";
    oss << "    \"refCreateSavePath\": \"" << escape_json_string(conf.ref_create_save_path)
        << "\",\n";
    oss << "    \"createDocNameTemplate\": \"" << escape_json_string(conf.create_doc_name_template)
        << "\",\n";
    oss << "    \"dailyNoteSavePath\": \"" << escape_json_string(conf.daily_note_save_path)
        << "\",\n";
    oss << "    \"dailyNoteTemplatePath\": \"" << escape_json_string(conf.daily_note_template_path)
        << "\"\n";
    oss << "}\n";
    return oss.str();
}

// Simple JSON value extractor — finds "key": "value" or "key": number/bool
auto extract_json_string(const std::string& json, const std::string& key) -> std::string
{
    auto search = "\"" + key + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos)
    {
        return "";
    }
    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos)
    {
        return "";
    }
    pos = json.find('"', pos + 1);
    if (pos == std::string::npos)
    {
        return "";
    }
    ++pos; // skip opening quote
    std::string result;
    while (pos < json.size() && json[pos] != '"')
    {
        if (json[pos] == '\\' && pos + 1 < json.size())
        {
            ++pos;
            switch (json[pos])
            {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                default:
                    result += json[pos];
                    break;
            }
        }
        else
        {
            result += json[pos];
        }
        ++pos;
    }
    return result;
}

auto extract_json_int(const std::string& json, const std::string& key) -> int32_t
{
    auto search = "\"" + key + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos)
    {
        return 0;
    }
    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos)
    {
        return 0;
    }
    ++pos;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
    {
        ++pos;
    }
    std::string num_str;
    while (pos < json.size() && (json[pos] == '-' || (json[pos] >= '0' && json[pos] <= '9')))
    {
        num_str += json[pos];
        ++pos;
    }
    if (num_str.empty())
    {
        return 0;
    }
    return static_cast<int32_t>(std::stoi(num_str));
}

auto extract_json_bool(const std::string& json, const std::string& key) -> bool
{
    auto search = "\"" + key + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos)
    {
        return false;
    }
    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos)
    {
        return false;
    }
    return json.find("true", pos) < json.find("false", pos);
}

auto deserialize_notebook_conf(const std::string& json) -> markamp::core::NotebookConf
{
    markamp::core::NotebookConf conf;
    conf.name = extract_json_string(json, "name");
    conf.sort = extract_json_int(json, "sort");
    conf.closed = extract_json_bool(json, "closed");
    conf.ref_create_save_path = extract_json_string(json, "refCreateSavePath");
    conf.create_doc_name_template = extract_json_string(json, "createDocNameTemplate");
    conf.daily_note_save_path = extract_json_string(json, "dailyNoteSavePath");
    conf.daily_note_template_path = extract_json_string(json, "dailyNoteTemplatePath");
    return conf;
}

auto read_file_contents(const std::filesystem::path& path) -> std::string
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return "";
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

auto atomic_write_file(const std::filesystem::path& path, const std::string& content) -> bool
{
    auto tmp_path = path;
    tmp_path += ".tmp";

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec)
    {
        return false;
    }

    {
        std::ofstream file(tmp_path);
        if (!file.is_open())
        {
            return false;
        }
        file << content;
        if (!file.good())
        {
            return false;
        }
    }

    std::filesystem::rename(tmp_path, path, ec);
    return !ec;
}

} // anonymous namespace

namespace markamp::core
{

auto Notebook::is_open() const -> bool
{
    return !closed;
}

NotebookManager::NotebookManager(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto NotebookManager::create_notebook(const std::string& name, const std::string& save_path)
    -> std::expected<std::string, std::string>
{
    auto id = BlockIDGenerator::generate();

    auto dir = save_path.empty() ? notebook_data_path(id) : std::filesystem::path(save_path);

    std::error_code ec;
    std::filesystem::create_directories(dir / ".markamp", ec);
    if (ec)
    {
        return std::unexpected("Failed to create notebook directory: " + ec.message());
    }

    Notebook nb;
    nb.id = id;
    nb.name = name;
    nb.sort = 0;
    nb.closed = false;
    nb.conf.name = name;
    nb.conf.sort = 0;
    nb.conf.closed = false;

    auto conf_path = dir / ".markamp" / "conf.json";
    auto json = serialize_notebook_conf(nb.conf);
    if (!atomic_write_file(conf_path, json))
    {
        return std::unexpected("Failed to write notebook conf.json");
    }

    {
        std::unique_lock lock(mutex_);
        notebooks_.push_back(nb);
    }

    events::NotebookCreatedEvent event;
    event.notebook_id = id;
    event.name = name;
    event_bus_.publish(event);

    return id;
}

auto NotebookManager::rename_notebook(const std::string& id, const std::string& new_name)
    -> std::expected<void, std::string>
{
    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return std::unexpected("Notebook not found: " + id);
        }
        it->name = new_name;
        it->conf.name = new_name;
    }
    save_notebook_conf(id);

    events::NotebookRenamedEvent event;
    event.notebook_id = id;
    event.new_name = new_name;
    event_bus_.publish(event);

    return {};
}

auto NotebookManager::remove_notebook(const std::string& id) -> std::expected<void, std::string>
{
    auto src_path = notebook_data_path(id);

    // Move to history
    auto history_dir = data_dir() / ".markamp" / "history";
    auto timestamp = BlockIDGenerator::current_timestamp_string();
    auto dest_path = history_dir / (id + "_" + timestamp);

    std::error_code ec;
    std::filesystem::create_directories(history_dir, ec);
    if (ec)
    {
        return std::unexpected("Failed to create history directory: " + ec.message());
    }

    if (std::filesystem::exists(src_path, ec))
    {
        std::filesystem::rename(src_path, dest_path, ec);
        if (ec)
        {
            return std::unexpected("Failed to move notebook to history: " + ec.message());
        }
    }

    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return std::unexpected("Notebook not found: " + id);
        }
        notebooks_.erase(it);
    }

    events::NotebookRemovedEvent event;
    event.notebook_id = id;
    event_bus_.publish(event);

    return {};
}

auto NotebookManager::set_notebook_icon(const std::string& id, const std::string& icon) -> void
{
    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return;
        }
        it->icon = icon;
    }
}

auto NotebookManager::open_notebook(const std::string& id) -> std::expected<void, std::string>
{
    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return std::unexpected("Notebook not found: " + id);
        }
        it->closed = false;
        it->conf.closed = false;
    }
    save_notebook_conf(id);

    events::NotebookOpenedEvent event;
    event.notebook_id = id;
    event_bus_.publish(event);

    return {};
}

auto NotebookManager::close_notebook(const std::string& id) -> std::expected<void, std::string>
{
    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return std::unexpected("Notebook not found: " + id);
        }
        it->closed = true;
        it->conf.closed = true;
    }
    save_notebook_conf(id);

    events::NotebookClosedEvent event;
    event.notebook_id = id;
    event_bus_.publish(event);

    return {};
}

auto NotebookManager::list_notebooks() const -> std::vector<Notebook>
{
    std::shared_lock lock(mutex_);
    auto result = notebooks_;
    std::sort(result.begin(),
              result.end(),
              [](const Notebook& a, const Notebook& b)
              {
                  if (a.sort != b.sort)
                  {
                      return a.sort < b.sort;
                  }
                  return a.id < b.id;
              });
    return result;
}

auto NotebookManager::get_notebook(const std::string& id) const -> std::optional<Notebook>
{
    std::shared_lock lock(mutex_);
    auto it = find_notebook_unlocked(id);
    if (it == notebooks_.end())
    {
        return std::nullopt;
    }
    return *it;
}

auto NotebookManager::get_open_notebooks() const -> std::vector<Notebook>
{
    std::shared_lock lock(mutex_);
    std::vector<Notebook> result;
    for (const auto& nb : notebooks_)
    {
        if (nb.is_open())
        {
            result.push_back(nb);
        }
    }
    std::sort(result.begin(),
              result.end(),
              [](const Notebook& a, const Notebook& b) { return a.sort < b.sort; });
    return result;
}

auto NotebookManager::sort_notebooks(const std::vector<std::string>& ordered_ids) -> void
{
    {
        std::unique_lock lock(mutex_);
        for (int32_t i = 0; i < static_cast<int32_t>(ordered_ids.size()); ++i)
        {
            auto it = find_notebook_unlocked(ordered_ids[static_cast<std::size_t>(i)]);
            if (it != notebooks_.end())
            {
                it->sort = i;
                it->conf.sort = i;
            }
        }
    }
    // Persist all
    for (const auto& id : ordered_ids)
    {
        save_notebook_conf(id);
    }

    events::NotebooksSortedEvent event;
    event_bus_.publish(event);
}

auto NotebookManager::get_notebook_conf(const std::string& id) const -> std::optional<NotebookConf>
{
    std::shared_lock lock(mutex_);
    auto it = find_notebook_unlocked(id);
    if (it == notebooks_.end())
    {
        return std::nullopt;
    }
    return it->conf;
}

auto NotebookManager::set_notebook_conf(const std::string& id, const NotebookConf& conf) -> void
{
    {
        std::unique_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return;
        }
        it->conf = conf;
        it->name = conf.name;
        it->sort = conf.sort;
        it->closed = conf.closed;
    }
    save_notebook_conf(id);
}

auto NotebookManager::load_notebooks() -> void
{
    std::unique_lock lock(mutex_);
    notebooks_.clear();
    auto dir = data_dir();
    std::error_code ec;
    if (std::filesystem::exists(dir, ec))
    {
        load_notebooks_from_disk(dir);
    }
}

auto NotebookManager::load_notebooks_from_disk(const std::filesystem::path& dir) -> void
{
    std::error_code ec;
    for (auto& entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (!entry.is_directory())
        {
            continue;
        }
        auto conf_path = entry.path() / ".markamp" / "conf.json";
        if (!std::filesystem::exists(conf_path, ec))
        {
            continue;
        }
        auto json = read_file_contents(conf_path);
        if (json.empty())
        {
            continue;
        }
        auto conf = deserialize_notebook_conf(json);

        Notebook nb;
        nb.id = entry.path().filename().string();
        nb.name = conf.name;
        nb.sort = conf.sort;
        nb.closed = conf.closed;
        nb.conf = conf;
        notebooks_.push_back(nb);
    }
    std::sort(notebooks_.begin(),
              notebooks_.end(),
              [](const Notebook& a, const Notebook& b) { return a.sort < b.sort; });
}

auto NotebookManager::save_notebook_conf(const std::string& id) -> void
{
    NotebookConf conf;
    {
        std::shared_lock lock(mutex_);
        auto it = find_notebook_unlocked(id);
        if (it == notebooks_.end())
        {
            return;
        }
        conf = it->conf;
    }
    auto path = notebook_conf_path(id);
    auto json = serialize_notebook_conf(conf);
    atomic_write_file(path, json);
}

auto NotebookManager::notebook_data_path(const std::string& id) const -> std::filesystem::path
{
    return data_dir() / id;
}

auto NotebookManager::notebook_conf_path(const std::string& id) const -> std::filesystem::path
{
    return notebook_data_path(id) / ".markamp" / "conf.json";
}

auto NotebookManager::data_dir() const -> std::filesystem::path
{
    return config_.get_string("knowledgebase.data_dir", "~/.markamp/data");
}

auto NotebookManager::find_notebook_unlocked(const std::string& id)
    -> std::vector<Notebook>::iterator
{
    return std::find_if(
        notebooks_.begin(), notebooks_.end(), [&id](const Notebook& nb) { return nb.id == id; });
}

auto NotebookManager::find_notebook_unlocked(const std::string& id) const
    -> std::vector<Notebook>::const_iterator
{
    return std::find_if(
        notebooks_.cbegin(), notebooks_.cend(), [&id](const Notebook& nb) { return nb.id == id; });
}

} // namespace markamp::core
