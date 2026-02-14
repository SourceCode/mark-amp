#include "core/ExtensionManifest.h"
#include "core/VsixService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <zip.h>

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
                ("markamp_vsix_test_" +
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

/// Create a minimal valid VSIX file with package.json.
void create_test_vsix(const fs::path& vsix_path,
                      const std::string& ext_name = "test-ext",
                      const std::string& version = "1.0.0",
                      const std::string& publisher = "test-pub")
{
    int zip_error = 0;
    auto* archive = zip_open(vsix_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &zip_error);
    REQUIRE(archive != nullptr);

    // Create extension/package.json content
    std::string package_json = R"({
        "name": ")" + ext_name +
                               R"(",
        "version": ")" + version +
                               R"(",
        "publisher": ")" + publisher +
                               R"(",
        "description": "A test extension"
    })";

    auto* source = zip_source_buffer(archive, package_json.data(), package_json.size(), 0);
    REQUIRE(source != nullptr);
    REQUIRE(zip_file_add(archive, "extension/package.json", source, ZIP_FL_OVERWRITE) >= 0);

    // Add a sample file
    std::string readme = "# Test Extension\nThis is a test.";
    auto* readme_source = zip_source_buffer(archive, readme.data(), readme.size(), 0);
    REQUIRE(readme_source != nullptr);
    REQUIRE(zip_file_add(archive, "extension/README.md", readme_source, ZIP_FL_OVERWRITE) >= 0);

    REQUIRE(zip_close(archive) == 0);
}

/// Create an invalid ZIP file (just random bytes).
void create_invalid_zip(const fs::path& file_path)
{
    std::ofstream ofs(file_path, std::ios::binary);
    ofs << "not a zip file at all";
}

/// Create a ZIP file without extension/package.json.
void create_zip_without_manifest(const fs::path& zip_path)
{
    int zip_error = 0;
    auto* archive = zip_open(zip_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &zip_error);
    REQUIRE(archive != nullptr);

    std::string content = "some content";
    auto* source = zip_source_buffer(archive, content.data(), content.size(), 0);
    REQUIRE(source != nullptr);
    REQUIRE(zip_file_add(archive, "random/file.txt", source, ZIP_FL_OVERWRITE) >= 0);

    REQUIRE(zip_close(archive) == 0);
}

} // anonymous namespace

// ── VsixPackageService Tests ──

TEST_CASE("VsixPackageService: validate valid VSIX", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "valid.vsix";
    create_test_vsix(vsix_file);

    const auto result = VsixPackageService::validate(vsix_file);
    REQUIRE(result.has_value());
}

TEST_CASE("VsixPackageService: validate nonexistent file", "[vsix]")
{
    const auto result = VsixPackageService::validate("/nonexistent/file.vsix");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("VsixPackageService: validate invalid ZIP", "[vsix]")
{
    TempDir tmp;
    const auto bad_file = tmp.path() / "bad.vsix";
    create_invalid_zip(bad_file);

    const auto result = VsixPackageService::validate(bad_file);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("VsixPackageService: validate ZIP without package.json", "[vsix]")
{
    TempDir tmp;
    const auto zip_file = tmp.path() / "no-manifest.vsix";
    create_zip_without_manifest(zip_file);

    const auto result = VsixPackageService::validate(zip_file);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("VsixPackageService: read_manifest from VSIX", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "test.vsix";
    create_test_vsix(vsix_file, "my-ext", "2.0.0", "my-pub");

    const auto result = VsixPackageService::read_manifest(vsix_file);
    REQUIRE(result.has_value());
    REQUIRE(result.value().name == "my-ext");
    REQUIRE(result.value().version == "2.0.0");
    REQUIRE(result.value().publisher == "my-pub");
}

TEST_CASE("VsixPackageService: inspect VSIX", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "test.vsix";
    create_test_vsix(vsix_file);

    const auto result = VsixPackageService::inspect(vsix_file);
    REQUIRE(result.has_value());
    REQUIRE(result.value().manifest.name == "test-ext");
    REQUIRE(result.value().file_list.size() == 2); // package.json + README.md
    REQUIRE(result.value().total_size_bytes > 0);
}

// ── VsixInstallService Tests ──

TEST_CASE("VsixInstallService: install valid VSIX", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "install-test.vsix";
    const auto ext_root = tmp.path() / "extensions";
    create_test_vsix(vsix_file, "my-ext", "1.0.0", "pub");

    VsixInstallService installer(ext_root);
    const auto result = installer.install(vsix_file);

    REQUIRE(result.has_value());
    REQUIRE(result.value().manifest.name == "my-ext");
    REQUIRE(result.value().install_path == ext_root / "pub.my-ext-1.0.0");
    REQUIRE(fs::exists(ext_root / "pub.my-ext-1.0.0" / "package.json"));
    REQUIRE(fs::exists(ext_root / "pub.my-ext-1.0.0" / "README.md"));
}

TEST_CASE("VsixInstallService: install overwrites existing", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "overwrite-test.vsix";
    const auto ext_root = tmp.path() / "extensions";
    create_test_vsix(vsix_file, "my-ext", "1.0.0", "pub");

    VsixInstallService installer(ext_root);

    // Install once
    auto result1 = installer.install(vsix_file);
    REQUIRE(result1.has_value());

    // Install again — should overwrite
    auto result2 = installer.install(vsix_file);
    REQUIRE(result2.has_value());
    REQUIRE(fs::exists(ext_root / "pub.my-ext-1.0.0" / "package.json"));
}

TEST_CASE("VsixInstallService: install invalid VSIX fails", "[vsix]")
{
    TempDir tmp;
    const auto bad_file = tmp.path() / "bad.vsix";
    create_invalid_zip(bad_file);

    VsixInstallService installer(tmp.path() / "extensions");
    const auto result = installer.install(bad_file);

    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("VsixInstallService: uninstall existing extension", "[vsix]")
{
    TempDir tmp;
    const auto vsix_file = tmp.path() / "uninstall-test.vsix";
    const auto ext_root = tmp.path() / "extensions";
    create_test_vsix(vsix_file, "my-ext", "1.0.0", "pub");

    VsixInstallService installer(ext_root);
    auto install_result = installer.install(vsix_file);
    REQUIRE(install_result.has_value());
    REQUIRE(fs::exists(ext_root / "pub.my-ext-1.0.0"));

    auto uninstall_result = installer.uninstall("pub.my-ext");
    REQUIRE(uninstall_result.has_value());
    REQUIRE_FALSE(fs::exists(ext_root / "pub.my-ext-1.0.0"));
}

TEST_CASE("VsixInstallService: uninstall nonexistent extension fails", "[vsix]")
{
    TempDir tmp;
    const auto ext_root = tmp.path() / "extensions";
    fs::create_directories(ext_root);

    VsixInstallService installer(ext_root);
    const auto result = installer.uninstall("nonexistent.ext");
    REQUIRE_FALSE(result.has_value());
}

// ── VsixExportService Tests ──

TEST_CASE("VsixExportService: export installed extension", "[vsix]")
{
    TempDir tmp;

    // First install an extension
    const auto vsix_file = tmp.path() / "export-test.vsix";
    const auto ext_root = tmp.path() / "extensions";
    create_test_vsix(vsix_file, "exp-ext", "1.0.0", "pub");

    VsixInstallService installer(ext_root);
    auto install_result = installer.install(vsix_file);
    REQUIRE(install_result.has_value());

    // Now export it back
    const auto export_path = tmp.path() / "exported.vsix";
    const auto result =
        VsixExportService::export_to_vsix(install_result.value().install_path, export_path);

    REQUIRE(result.has_value());
    REQUIRE(fs::exists(export_path));

    // Verify the exported VSIX is valid
    const auto validate_result = VsixPackageService::validate(export_path);
    REQUIRE(validate_result.has_value());
}

TEST_CASE("VsixExportService: export nonexistent directory fails", "[vsix]")
{
    const auto result = VsixExportService::export_to_vsix("/nonexistent/dir", "/tmp/output.vsix");
    REQUIRE_FALSE(result.has_value());
}
