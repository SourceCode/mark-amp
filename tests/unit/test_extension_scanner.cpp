#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionEnablement.h"
#include "core/ExtensionManifest.h"
#include "core/ExtensionScanner.h"
#include "core/ExtensionStorage.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

namespace fs = std::filesystem;

namespace
{

/// Helper to create a temporary directory for tests.
class TempDir
{
public:
    TempDir()
        : path_(fs::temp_directory_path() /
                ("markamp_test_" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
        fs::create_directories(path_);
    }

    ~TempDir()
    {
        std::error_code cleanup_error;
        fs::remove_all(path_, cleanup_error);
    }

    TempDir(const TempDir&) = delete;
    auto operator=(const TempDir&) -> TempDir& = delete;
    TempDir(TempDir&&) = delete;
    auto operator=(TempDir&&) -> TempDir& = delete;

    [[nodiscard]] auto path() const -> const fs::path&
    {
        return path_;
    }

private:
    fs::path path_;
};

/// Write a minimal valid package.json into a subdirectory.
void write_package_json(const fs::path& ext_dir,
                        const std::string& ext_name,
                        const std::string& version,
                        const std::string& publisher)
{
    fs::create_directories(ext_dir);
    std::ofstream ofs(ext_dir / "package.json");
    ofs << R"({
        "name": ")"
        << ext_name << R"(",
        "version": ")"
        << version << R"(",
        "publisher": ")"
        << publisher << R"("
    })";
}

} // anonymous namespace

// ── ExtensionScanner Tests ──

TEST_CASE("ExtensionScanner: scan empty directory", "[extension-scanner]")
{
    TempDir tmp;
    ExtensionScannerService scanner(tmp.path());

    const auto extensions = scanner.scan_extensions();
    REQUIRE(extensions.empty());
}

TEST_CASE("ExtensionScanner: scan non-existent directory", "[extension-scanner]")
{
    ExtensionScannerService scanner(fs::path("/nonexistent/markamp/extensions"));

    const auto extensions = scanner.scan_extensions();
    REQUIRE(extensions.empty());
}

TEST_CASE("ExtensionScanner: scan directory with valid extensions", "[extension-scanner]")
{
    TempDir tmp;
    write_package_json(tmp.path() / "ext-beta", "beta-ext", "1.0.0", "pubB");
    write_package_json(tmp.path() / "ext-alpha", "alpha-ext", "2.0.0", "pubA");

    ExtensionScannerService scanner(tmp.path());
    const auto extensions = scanner.scan_extensions();

    // Should find 2 extensions, sorted by identifier key
    REQUIRE(extensions.size() == 2);
    REQUIRE(extensions[0].manifest.name == "alpha-ext");
    REQUIRE(extensions[0].manifest.publisher == "pubA");
    REQUIRE(extensions[0].manifest.version == "2.0.0");
    REQUIRE(extensions[0].is_builtin == false);
    REQUIRE(extensions[1].manifest.name == "beta-ext");
}

TEST_CASE("ExtensionScanner: skip directories without package.json", "[extension-scanner]")
{
    TempDir tmp;
    write_package_json(tmp.path() / "valid-ext", "valid", "1.0.0", "pub");
    fs::create_directories(tmp.path() / "invalid-ext"); // No package.json

    ExtensionScannerService scanner(tmp.path());
    const auto extensions = scanner.scan_extensions();

    REQUIRE(extensions.size() == 1);
    REQUIRE(extensions[0].manifest.name == "valid");
}

TEST_CASE("ExtensionScanner: skip invalid package.json", "[extension-scanner]")
{
    TempDir tmp;
    write_package_json(tmp.path() / "valid-ext", "valid", "1.0.0", "pub");

    // Create an extension with invalid JSON
    const auto bad_ext_dir = tmp.path() / "bad-ext";
    fs::create_directories(bad_ext_dir);
    std::ofstream bad_json(bad_ext_dir / "package.json");
    bad_json << "{ not valid json }";
    bad_json.close();

    ExtensionScannerService scanner(tmp.path());
    const auto extensions = scanner.scan_extensions();

    REQUIRE(extensions.size() == 1);
    REQUIRE(extensions[0].manifest.name == "valid");
}

TEST_CASE("ExtensionScanner: skip regular files (non-directories)", "[extension-scanner]")
{
    TempDir tmp;
    write_package_json(tmp.path() / "valid-ext", "valid", "1.0.0", "pub");

    // Create a regular file at the top level
    std::ofstream file(tmp.path() / "random-file.txt");
    file << "not an extension";
    file.close();

    ExtensionScannerService scanner(tmp.path());
    const auto extensions = scanner.scan_extensions();

    REQUIRE(extensions.size() == 1);
}

TEST_CASE("ExtensionScanner: location is set correctly", "[extension-scanner]")
{
    TempDir tmp;
    write_package_json(tmp.path() / "my-ext", "my-ext", "1.0.0", "pub");

    ExtensionScannerService scanner(tmp.path());
    const auto extensions = scanner.scan_extensions();

    REQUIRE(extensions.size() == 1);
    REQUIRE(extensions[0].location == tmp.path() / "my-ext");
}

// ── ExtensionStorage Tests ──

TEST_CASE("ExtensionStorage: start empty", "[extension-storage]")
{
    TempDir tmp;
    ExtensionStorageService storage(tmp.path() / "extensions.json");
    storage.load();

    REQUIRE(storage.count() == 0);
    REQUIRE(storage.get_all().empty());
}

TEST_CASE("ExtensionStorage: upsert and retrieve", "[extension-storage]")
{
    TempDir tmp;
    ExtensionStorageService storage(tmp.path() / "extensions.json");

    ExtensionMetadata meta;
    meta.extension_id = "pub.my-ext";
    meta.version = "1.0.0";
    meta.source = ExtensionSource::kVsix;
    meta.installed_at = "2026-01-15T10:00:00Z";
    meta.enabled = true;
    meta.location = "/path/to/ext";

    storage.upsert(meta);

    REQUIRE(storage.count() == 1);
    REQUIRE(storage.contains("pub.my-ext"));

    const auto* retrieved = storage.get("pub.my-ext");
    REQUIRE(retrieved != nullptr);
    REQUIRE(retrieved->version == "1.0.0");
    REQUIRE(retrieved->source == ExtensionSource::kVsix);
    REQUIRE(retrieved->enabled == true);
}

TEST_CASE("ExtensionStorage: upsert overwrites existing", "[extension-storage]")
{
    TempDir tmp;
    ExtensionStorageService storage(tmp.path() / "extensions.json");

    ExtensionMetadata meta;
    meta.extension_id = "pub.ext";
    meta.version = "1.0.0";
    storage.upsert(meta);

    meta.version = "2.0.0";
    storage.upsert(meta);

    REQUIRE(storage.count() == 1);
    REQUIRE(storage.get("pub.ext")->version == "2.0.0");
}

TEST_CASE("ExtensionStorage: remove", "[extension-storage]")
{
    TempDir tmp;
    ExtensionStorageService storage(tmp.path() / "extensions.json");

    ExtensionMetadata meta;
    meta.extension_id = "pub.ext";
    meta.version = "1.0.0";
    storage.upsert(meta);
    REQUIRE(storage.count() == 1);

    storage.remove("pub.ext");
    REQUIRE(storage.count() == 0);
    REQUIRE_FALSE(storage.contains("pub.ext"));
    REQUIRE(storage.get("pub.ext") == nullptr);
}

TEST_CASE("ExtensionStorage: save and reload", "[extension-storage]")
{
    TempDir tmp;
    const auto storage_file = tmp.path() / "extensions.json";

    // Write
    {
        ExtensionStorageService storage(storage_file);

        ExtensionMetadata m1;
        m1.extension_id = "pub.ext1";
        m1.version = "1.0.0";
        m1.source = ExtensionSource::kGallery;
        m1.enabled = true;
        storage.upsert(m1);

        ExtensionMetadata m2;
        m2.extension_id = "pub.ext2";
        m2.version = "2.0.0";
        m2.source = ExtensionSource::kVsix;
        m2.enabled = false;
        storage.upsert(m2);

        storage.save();
    }

    // Reload
    {
        ExtensionStorageService storage(storage_file);
        storage.load();

        REQUIRE(storage.count() == 2);
        REQUIRE(storage.get("pub.ext1")->version == "1.0.0");
        REQUIRE(storage.get("pub.ext1")->enabled == true);
        REQUIRE(storage.get("pub.ext2")->version == "2.0.0");
        REQUIRE(storage.get("pub.ext2")->enabled == false);
    }
}

TEST_CASE("ExtensionStorage: get nonexistent returns nullptr", "[extension-storage]")
{
    TempDir tmp;
    ExtensionStorageService storage(tmp.path() / "extensions.json");
    REQUIRE(storage.get("nonexistent") == nullptr);
}

// ── ExtensionEnablement Tests ──

TEST_CASE("ExtensionEnablement: enabled by default", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);
    REQUIRE(enablement.is_enabled("pub.any-ext"));
    REQUIRE(enablement.disabled_count() == 0);
}

TEST_CASE("ExtensionEnablement: disable an extension", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);
    enablement.disable("pub.my-ext");

    REQUIRE_FALSE(enablement.is_enabled("pub.my-ext"));
    REQUIRE(enablement.disabled_count() == 1);
}

TEST_CASE("ExtensionEnablement: enable a disabled extension", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);
    enablement.disable("pub.my-ext");
    REQUIRE_FALSE(enablement.is_enabled("pub.my-ext"));

    enablement.enable("pub.my-ext");
    REQUIRE(enablement.is_enabled("pub.my-ext"));
    REQUIRE(enablement.disabled_count() == 0);
}

TEST_CASE("ExtensionEnablement: toggle extension", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);

    // Toggle off (was enabled)
    const bool state1 = enablement.toggle("pub.ext");
    REQUIRE(state1 == false);
    REQUIRE_FALSE(enablement.is_enabled("pub.ext"));

    // Toggle on (was disabled)
    const bool state2 = enablement.toggle("pub.ext");
    REQUIRE(state2 == true);
    REQUIRE(enablement.is_enabled("pub.ext"));
}

TEST_CASE("ExtensionEnablement: fires ExtensionEnablementChangedEvent on disable",
          "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    bool event_fired = false;
    std::string fired_ext_id;
    bool fired_enabled = true;

    auto sub = bus.subscribe<events::ExtensionEnablementChangedEvent>(
        [&](const events::ExtensionEnablementChangedEvent& evt)
        {
            event_fired = true;
            fired_ext_id = evt.extension_id;
            fired_enabled = evt.enabled;
        });

    ExtensionEnablementService enablement(bus, cfg);
    enablement.disable("pub.ext");

    REQUIRE(event_fired);
    REQUIRE(fired_ext_id == "pub.ext");
    REQUIRE(fired_enabled == false);
}

TEST_CASE("ExtensionEnablement: fires ExtensionEnablementChangedEvent on enable",
          "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);
    enablement.disable("pub.ext");

    bool event_fired = false;
    bool fired_enabled = false;

    auto sub = bus.subscribe<events::ExtensionEnablementChangedEvent>(
        [&](const events::ExtensionEnablementChangedEvent& evt)
        {
            event_fired = true;
            fired_enabled = evt.enabled;
        });

    enablement.enable("pub.ext");

    REQUIRE(event_fired);
    REQUIRE(fired_enabled == true);
}

TEST_CASE("ExtensionEnablement: no duplicate events on redundant operations",
          "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    int event_count = 0;
    auto sub = bus.subscribe<events::ExtensionEnablementChangedEvent>(
        [&](const events::ExtensionEnablementChangedEvent& /*evt*/) { ++event_count; });

    ExtensionEnablementService enablement(bus, cfg);

    // Enable an already-enabled extension — should not fire
    enablement.enable("pub.ext");
    REQUIRE(event_count == 0);

    // Disable — should fire once
    enablement.disable("pub.ext");
    REQUIRE(event_count == 1);

    // Disable again — should not fire
    enablement.disable("pub.ext");
    REQUIRE(event_count == 1);
}

TEST_CASE("ExtensionEnablement: get_disabled_ids", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    ExtensionEnablementService enablement(bus, cfg);
    enablement.disable("pub.ext1");
    enablement.disable("pub.ext2");

    const auto disabled = enablement.get_disabled_ids();
    REQUIRE(disabled.size() == 2);
}

TEST_CASE("ExtensionEnablement: persists to Config", "[extension-enablement]")
{
    EventBus bus;
    Config cfg;

    // Disable two extensions
    {
        ExtensionEnablementService enablement(bus, cfg);
        enablement.disable("pub.ext1");
        enablement.disable("pub.ext2");
    }

    // Config should have the disabled IDs
    const std::string stored = cfg.get_string("extensions.disabled", "");
    REQUIRE_FALSE(stored.empty());

    // Reload from config — both should still be disabled
    {
        ExtensionEnablementService enablement2(bus, cfg);
        REQUIRE_FALSE(enablement2.is_enabled("pub.ext1"));
        REQUIRE_FALSE(enablement2.is_enabled("pub.ext2"));
        REQUIRE(enablement2.disabled_count() == 2);
    }
}
