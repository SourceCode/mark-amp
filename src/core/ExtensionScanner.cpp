#include "ExtensionScanner.h"

#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

namespace fs = std::filesystem;

ExtensionScannerService::ExtensionScannerService(fs::path extensions_root)
    : extensions_root_(std::move(extensions_root))
{
}

ExtensionScannerService::ExtensionScannerService()
    : extensions_root_(
          []() -> fs::path
          {
              const char* home = std::getenv("HOME"); // NOLINT(concurrency-mt-unsafe)
              if (home != nullptr)
              {
                  return fs::path(home) / ".markamp" / "extensions";
              }
              return fs::path(".markamp") / "extensions";
          }())
{
}

auto ExtensionScannerService::scan_extensions() -> std::vector<LocalExtension>
{
    return scan_directory(extensions_root_);
}

auto ExtensionScannerService::scan_directory(const fs::path& dir) -> std::vector<LocalExtension>
{
    std::vector<LocalExtension> extensions;

    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
        return extensions;
    }

    std::error_code dir_error;
    for (const auto& entry : fs::directory_iterator(dir, dir_error))
    {
        if (dir_error)
        {
            MARKAMP_LOG_WARN("Error iterating extensions directory: {}", dir_error.message());
            break;
        }

        if (!entry.is_directory())
        {
            continue;
        }

        const auto package_json = entry.path() / "package.json";
        if (!fs::exists(package_json))
        {
            MARKAMP_LOG_WARN("Extension directory missing package.json: {}",
                             entry.path().filename().string());
            continue;
        }

        try
        {
            auto manifest = ManifestParser::parse_file(package_json.string());
            LocalExtension ext;
            ext.manifest = std::move(manifest);
            ext.location = entry.path();
            ext.is_builtin = false;
            extensions.push_back(std::move(ext));
        }
        catch (const std::exception& parse_error)
        {
            MARKAMP_LOG_WARN("Failed to parse extension at {}: {}",
                             entry.path().filename().string(),
                             parse_error.what());
        }
    }

    // Sort by identifier for deterministic ordering
    std::sort(extensions.begin(),
              extensions.end(),
              [](const LocalExtension& lhs, const LocalExtension& rhs)
              { return lhs.manifest.identifier().to_key() < rhs.manifest.identifier().to_key(); });

    return extensions;
}

} // namespace markamp::core
