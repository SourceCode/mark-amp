#include "EnvironmentService.h"

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/utils.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <random>
#include <sstream>

namespace markamp::core
{

namespace
{

/// Generate a RFC 4122 v4 UUID.
auto generate_uuid() -> std::string
{
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    auto hex = [](uint32_t val, int digits) -> std::string
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(digits) << val;
        return oss.str();
    };

    // 8-4-4-4-12 format with version 4 and variant 1
    return hex(dist(gen), 8) + "-" + hex(dist(gen) & 0xFFFF, 4) + "-" +
           hex((dist(gen) & 0x0FFF) | 0x4000, 4) + "-" + // version 4
           hex((dist(gen) & 0x3FFF) | 0x8000, 4) + "-" + // variant 1
           hex(dist(gen), 8) + hex(dist(gen) & 0xFFFF, 4);
}

} // anonymous namespace

auto EnvironmentService::app_name() const -> std::string
{
    return "MarkAmp";
}

auto EnvironmentService::app_root() const -> std::string
{
    return app_root_;
}

auto EnvironmentService::language() const -> std::string
{
    if (!language_.empty())
    {
        return language_;
    }

    // Detect system locale
    const char* lang = std::getenv("LANG"); // NOLINT(concurrency-mt-unsafe)
    if (lang != nullptr)
    {
        std::string locale(lang);
        // Extract language code (e.g. "en_US.UTF-8" → "en")
        auto underscore = locale.find('_');
        if (underscore != std::string::npos)
        {
            return locale.substr(0, underscore);
        }
        auto dot_pos = locale.find('.');
        if (dot_pos != std::string::npos)
        {
            return locale.substr(0, dot_pos);
        }
        return locale;
    }
    return "en";
}

auto EnvironmentService::machine_id() const -> std::string
{
    if (!machine_id_.empty())
    {
        return machine_id_;
    }

    // Generate and persist a UUID in the config directory
    namespace fs = std::filesystem;
    const std::string config_dir = app_root_.empty() ? "." : app_root_;
    const fs::path id_file = fs::path(config_dir) / ".markamp-machine-id";

    // Try to read existing ID
    if (fs::exists(id_file))
    {
        std::ifstream file_in(id_file);
        std::string stored_id;
        if (std::getline(file_in, stored_id) && !stored_id.empty())
        {
            // Cache it
            machine_id_ = stored_id;
            return machine_id_;
        }
    }

    // Generate a new UUID and persist it
    machine_id_ = generate_uuid();
    try
    {
        fs::create_directories(fs::path(id_file).parent_path());
        std::ofstream file_out(id_file);
        file_out << machine_id_ << '\n';
    }
    catch (const fs::filesystem_error& /*err*/)
    {
        // Silently continue — the ID is still usable in memory
    }

    return machine_id_;
}

auto EnvironmentService::uri_scheme() const -> std::string
{
    return "markamp";
}

auto EnvironmentService::clipboard_read() const -> std::string
{
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            wxTheClipboard->Close();
            return data.GetText().ToStdString();
        }
        wxTheClipboard->Close();
    }
    return {};
}

void EnvironmentService::clipboard_write(const std::string& text)
{
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(wxString(text)));
        wxTheClipboard->Close();
    }
}

auto EnvironmentService::open_external(const std::string& uri) -> bool
{
    if (uri.empty())
    {
        return false;
    }
    return wxLaunchDefaultBrowser(wxString(uri));
}

void EnvironmentService::set_app_root(const std::string& root)
{
    app_root_ = root;
}

void EnvironmentService::set_language(const std::string& lang)
{
    language_ = lang;
}

void EnvironmentService::set_machine_id(const std::string& machine_id)
{
    machine_id_ = machine_id;
}

} // namespace markamp::core
