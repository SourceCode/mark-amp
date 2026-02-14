#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionManagement.h"
#include "core/ExtensionManifest.h"
#include "core/ExtensionScanner.h"
#include "core/GalleryService.h"
#include "core/VsixService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <zip.h>

using namespace markamp::core;
using namespace markamp::core::events;

namespace fs = std::filesystem;

namespace
{

/// Helper to create a temporary directory for tests.
class TempDir
{
public:
    TempDir()
        : path_(fs::temp_directory_path() /
                ("markamp_mgmt_test_" +
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

/// Create a minimal valid VSIX file.
void create_test_vsix(const fs::path& vsix_path,
                      const std::string& ext_name = "test-ext",
                      const std::string& version = "1.0.0",
                      const std::string& publisher = "test-pub",
                      const std::vector<std::string>& deps = {})
{
    int zip_error = 0;
    auto* archive = zip_open(vsix_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &zip_error);
    REQUIRE(archive != nullptr);

    // Build deps JSON array
    std::string deps_json = "[";
    for (std::size_t dep_idx = 0; dep_idx < deps.size(); ++dep_idx)
    {
        if (dep_idx > 0)
        {
            deps_json += ",";
        }
        deps_json += "\"" + deps[dep_idx] + "\"";
    }
    deps_json += "]";

    std::string package_json = R"({
        "name": ")" + ext_name +
                               R"(",
        "version": ")" + version +
                               R"(",
        "publisher": ")" + publisher +
                               R"(",
        "description": "A test extension",
        "extensionDependencies": )" +
                               deps_json + R"(
    })";

    auto* source = zip_source_buffer(archive, package_json.data(), package_json.size(), 0);
    REQUIRE(source != nullptr);
    REQUIRE(zip_file_add(archive, "extension/package.json", source, ZIP_FL_OVERWRITE) >= 0);

    REQUIRE(zip_close(archive) == 0);
}

/// Create a mock installed extension directory.
void create_installed_extension(const fs::path& ext_root,
                                const std::string& publisher,
                                const std::string& name,
                                const std::string& version,
                                const std::vector<std::string>& deps = {})
{
    const auto dir_name = publisher + "." + name + "-" + version;
    const auto ext_dir = ext_root / dir_name;
    fs::create_directories(ext_dir);

    std::string deps_json = "[";
    for (std::size_t dep_idx = 0; dep_idx < deps.size(); ++dep_idx)
    {
        if (dep_idx > 0)
        {
            deps_json += ",";
        }
        deps_json += "\"" + deps[dep_idx] + "\"";
    }
    deps_json += "]";

    std::ofstream ofs(ext_dir / "package.json");
    ofs << R"({
        "name": ")"
        << name << R"(",
        "version": ")"
        << version << R"(",
        "publisher": ")"
        << publisher << R"(",
        "description": "Test",
        "extensionDependencies": )"
        << deps_json << R"(
    })";
}

/// Mock gallery service for testing (no HTTP calls).
class MockGalleryService : public IExtensionGalleryService
{
public:
    // Extensions to return from queries
    std::vector<GalleryExtension> mock_extensions;
    bool should_fail{false};

    auto query(const GalleryQueryOptions& /*options*/)
        -> std::expected<GalleryQueryResult, std::string> override
    {
        if (should_fail)
        {
            return std::unexpected("Mock gallery failure");
        }
        GalleryQueryResult result;
        result.extensions = mock_extensions;
        result.total_count = static_cast<std::int64_t>(mock_extensions.size());
        return result;
    }

    auto get_extensions(const std::vector<std::string>& /*identifiers*/)
        -> std::expected<std::vector<GalleryExtension>, std::string> override
    {
        if (should_fail)
        {
            return std::unexpected("Mock gallery failure");
        }
        return mock_extensions;
    }

    auto download(const GalleryExtension& /*extension*/, const std::string& /*dest_path*/)
        -> std::expected<void, std::string> override
    {
        if (should_fail)
        {
            return std::unexpected("Mock download failure");
        }
        // In tests, we pre-create the VSIX file, so this is a no-op
        return {};
    }

    auto get_readme(const GalleryExtension& /*extension*/)
        -> std::expected<std::string, std::string> override
    {
        return std::string("# Mock README");
    }

    auto get_changelog(const GalleryExtension& /*extension*/)
        -> std::expected<std::string, std::string> override
    {
        return std::string("# Mock Changelog");
    }
};

} // anonymous namespace

// ── Install from VSIX ──

TEST_CASE("ExtensionManagement: install from VSIX", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    const auto vsix_file = tmp.path() / "test.vsix";
    create_test_vsix(vsix_file, "my-ext", "1.0.0", "my-pub");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto result = mgr.install(vsix_file);
    REQUIRE(result.has_value());
    REQUIRE(result.value().manifest.name == "my-ext");
    REQUIRE(result.value().manifest.version == "1.0.0");
    REQUIRE(fs::exists(ext_root / "my-pub.my-ext-1.0.0" / "package.json"));
}

TEST_CASE("ExtensionManagement: install invalid VSIX fails", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto result = mgr.install("/nonexistent/file.vsix");
    REQUIRE_FALSE(result.has_value());
}

// ── Get installed ──

TEST_CASE("ExtensionManagement: get_installed returns extensions", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    create_installed_extension(ext_root, "pub-a", "ext-a", "1.0.0");
    create_installed_extension(ext_root, "pub-b", "ext-b", "2.0.0");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto installed = mgr.get_installed();
    REQUIRE(installed.size() == 2);
}

TEST_CASE("ExtensionManagement: get_installed empty dir", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    fs::create_directories(ext_root);

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto installed = mgr.get_installed();
    REQUIRE(installed.empty());
}

// ── Uninstall ──

TEST_CASE("ExtensionManagement: uninstall removes extension", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    const auto vsix_file = tmp.path() / "test.vsix";
    create_test_vsix(vsix_file, "rm-ext", "1.0.0", "pub");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);
    auto install_result = mgr.install(vsix_file);
    REQUIRE(install_result.has_value());

    auto uninstall_result = mgr.uninstall("pub.rm-ext");
    REQUIRE(uninstall_result.has_value());
    REQUIRE_FALSE(fs::exists(ext_root / "pub.rm-ext-1.0.0"));
}

TEST_CASE("ExtensionManagement: uninstall nonexistent fails", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    fs::create_directories(ext_root);

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto result = mgr.uninstall("no.such-ext");
    REQUIRE_FALSE(result.has_value());
}

// ── Dependency checking on uninstall ──

TEST_CASE("ExtensionManagement: uninstall blocked by dependent", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";

    // Create base extension and dependent
    create_installed_extension(ext_root, "pub", "base-ext", "1.0.0");
    create_installed_extension(ext_root, "pub", "child-ext", "1.0.0", {"pub.base-ext"});

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    // Should fail because child-ext depends on base-ext
    auto result = mgr.uninstall("pub.base-ext");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().find("depend") != std::string::npos);

    // base-ext should still exist
    REQUIRE(fs::exists(ext_root / "pub.base-ext-1.0.0"));
}

TEST_CASE("ExtensionManagement: uninstall leaf extension succeeds", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";

    // child has dependency on base, but we uninstall child (leaf)
    create_installed_extension(ext_root, "pub", "base-ext", "1.0.0");
    create_installed_extension(ext_root, "pub", "child-ext", "1.0.0", {"pub.base-ext"});

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    // Uninstall child (no dependents)
    auto result = mgr.uninstall("pub.child-ext");
    REQUIRE(result.has_value());
    REQUIRE_FALSE(fs::exists(ext_root / "pub.child-ext-1.0.0"));
}

// ── Update checking ──

TEST_CASE("ExtensionManagement: check_updates finds updates", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    create_installed_extension(ext_root, "pub", "my-ext", "1.0.0");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    // Mock gallery returns newer version
    GalleryExtension gallery_ext;
    gallery_ext.identifier = "pub.my-ext";
    gallery_ext.version = "2.0.0";
    gallery.mock_extensions = {gallery_ext};

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto updates = mgr.check_updates();
    REQUIRE(updates.has_value());
    REQUIRE(updates.value().size() == 1);
    REQUIRE(updates.value()[0].extension_id == "pub.my-ext");
    REQUIRE(updates.value()[0].current_version == "1.0.0");
    REQUIRE(updates.value()[0].available_version == "2.0.0");
}

TEST_CASE("ExtensionManagement: check_updates no updates", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    create_installed_extension(ext_root, "pub", "my-ext", "1.0.0");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    // Mock gallery returns same version
    GalleryExtension gallery_ext;
    gallery_ext.identifier = "pub.my-ext";
    gallery_ext.version = "1.0.0";
    gallery.mock_extensions = {gallery_ext};

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto updates = mgr.check_updates();
    REQUIRE(updates.has_value());
    REQUIRE(updates.value().empty());
}

TEST_CASE("ExtensionManagement: check_updates with no installed extensions", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    fs::create_directories(ext_root);

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);

    auto updates = mgr.check_updates();
    REQUIRE(updates.has_value());
    REQUIRE(updates.value().empty());
}

// ── Event publishing ──

TEST_CASE("ExtensionManagement: install publishes event", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    const auto vsix_file = tmp.path() / "event-test.vsix";
    create_test_vsix(vsix_file, "evt-ext", "1.0.0", "pub");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    bool event_received = false;
    auto sub = bus.subscribe<ExtensionInstalledEvent>(
        [&event_received](const ExtensionInstalledEvent& evt)
        {
            event_received = true;
            REQUIRE(evt.extension_id == "pub.evt-ext");
            REQUIRE(evt.version == "1.0.0");
        });

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);
    auto result = mgr.install(vsix_file);
    REQUIRE(result.has_value());
    REQUIRE(event_received);
}

TEST_CASE("ExtensionManagement: uninstall publishes event", "[management]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    create_installed_extension(ext_root, "pub", "evt-ext", "1.0.0");

    VsixInstallService vsix(ext_root);
    ExtensionScannerService scanner(ext_root);
    MockGalleryService gallery;
    EventBus bus;

    bool event_received = false;
    auto sub = bus.subscribe<ExtensionUninstalledEvent>(
        [&event_received](const ExtensionUninstalledEvent& evt)
        {
            event_received = true;
            REQUIRE(evt.extension_id == "pub.evt-ext");
        });

    ExtensionManagementService mgr(vsix, scanner, gallery, bus);
    auto result = mgr.uninstall("pub.evt-ext");
    REQUIRE(result.has_value());
    REQUIRE(event_received);
}
