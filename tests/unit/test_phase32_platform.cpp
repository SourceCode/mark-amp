/// @file test_phase32_platform.cpp
/// @brief V9 Phase 32 – Platform-Specific Optimization tests.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// Phase 32 new components
#include "core/CrossPlatformPath.h"
#include "core/PlatformFonts.h"
#include "core/PlatformKeyboard.h"
#include "core/PlatformServices.h"

// Events
#include "core/Events.h"

#include <map>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// CredentialManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("CredentialManager: store and retrieve", "[phase32][credentials]")
{
    CredentialManager mgr;
    Credential cred{"github.com", "user1", "token123"};
    CHECK(mgr.store(cred));
    auto retrieved = mgr.retrieve("github.com");
    CHECK(retrieved.service == "github.com");
    CHECK(retrieved.username == "user1");
    CHECK(retrieved.password == "token123");
}

TEST_CASE("CredentialManager: retrieve non-existent returns empty", "[phase32][credentials]")
{
    const CredentialManager mgr;
    auto cred = mgr.retrieve("nonexistent");
    CHECK(cred.service.empty());
    CHECK(cred.username.empty());
}

TEST_CASE("CredentialManager: has and remove", "[phase32][credentials]")
{
    CredentialManager mgr;
    mgr.store({"service1", "user", "pass"});
    CHECK(mgr.has("service1"));
    CHECK(mgr.remove("service1"));
    CHECK_FALSE(mgr.has("service1"));
    CHECK_FALSE(mgr.remove("service1")); // Already removed
}

TEST_CASE("CredentialManager: list services", "[phase32][credentials]")
{
    CredentialManager mgr;
    mgr.store({"beta", "u1", "p1"});
    mgr.store({"alpha", "u2", "p2"});
    mgr.store({"gamma", "u3", "p3"});
    auto services = mgr.list_services();
    REQUIRE(services.size() == 3);
    CHECK(services[0] == "alpha"); // Sorted
    CHECK(services[1] == "beta");
    CHECK(services[2] == "gamma");
}

// ═══════════════════════════════════════════════════════════════════
// NotificationService Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("NotificationService: show and history", "[phase32][notifications]")
{
    NotificationService svc;
    auto notification_id =
        svc.show({"Test Title", "Test Body", "", NotificationUrgency::kNormal, ""});
    CHECK_FALSE(notification_id.empty());
    CHECK(svc.history().size() == 1);
    CHECK(svc.history()[0].title == "Test Title");
}

TEST_CASE("NotificationService: dismiss removes from history", "[phase32][notifications]")
{
    NotificationService svc;
    auto notification_id = svc.show({"Alert", "Body", "", NotificationUrgency::kCritical, ""});
    REQUIRE(svc.history().size() == 1);
    svc.dismiss(notification_id);
    CHECK(svc.history().empty());
}

TEST_CASE("NotificationService: clear history", "[phase32][notifications]")
{
    NotificationService svc;
    svc.show({"N1", "", "", NotificationUrgency::kLow, ""});
    svc.show({"N2", "", "", NotificationUrgency::kLow, ""});
    REQUIRE(svc.history().size() == 2);
    svc.clear_history();
    CHECK(svc.history().empty());
}

// ═══════════════════════════════════════════════════════════════════
// FileWatcher Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("FileWatcher: watch and unwatch", "[phase32][filewatcher]")
{
    FileWatcher watcher;
    CHECK(watcher.watch("/tmp/test_dir"));
    CHECK(watcher.is_watching("/tmp/test_dir"));
    CHECK_FALSE(watcher.watch("/tmp/test_dir")); // Already watching
    CHECK(watcher.unwatch("/tmp/test_dir"));
    CHECK_FALSE(watcher.is_watching("/tmp/test_dir"));
}

TEST_CASE("FileWatcher: inject and poll events", "[phase32][filewatcher]")
{
    FileWatcher watcher;
    watcher.watch("/tmp/docs");
    watcher.inject_event({"/tmp/docs/file.md", FileChangeType::kCreated, ""});
    watcher.inject_event({"/tmp/docs/old.md", FileChangeType::kRenamed, "/tmp/docs/new.md"});
    auto events = watcher.poll_events();
    REQUIRE(events.size() == 2);
    CHECK(events[0].change_type == FileChangeType::kCreated);
    CHECK(events[1].change_type == FileChangeType::kRenamed);
    CHECK(events[1].new_path == "/tmp/docs/new.md");
    // After poll, events are consumed
    CHECK(watcher.poll_events().empty());
}

TEST_CASE("FileWatcher: watched paths list", "[phase32][filewatcher]")
{
    FileWatcher watcher;
    watcher.watch("/a");
    watcher.watch("/b");
    auto paths = watcher.watched_paths();
    CHECK(paths.size() == 2);
}

// ═══════════════════════════════════════════════════════════════════
// AutoUpdateService Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("AutoUpdateService: version comparison", "[phase32][autoupdate]")
{
    CHECK(AutoUpdateService::is_newer("1.0.0", "2.0.0"));
    CHECK(AutoUpdateService::is_newer("1.2.3", "1.3.0"));
    CHECK(AutoUpdateService::is_newer("1.2.3", "1.2.4"));
    CHECK_FALSE(AutoUpdateService::is_newer("2.0.0", "1.0.0"));
    CHECK_FALSE(AutoUpdateService::is_newer("1.0.0", "1.0.0")); // Equal
}

TEST_CASE("AutoUpdateService: channel and version", "[phase32][autoupdate]")
{
    AutoUpdateService svc;
    svc.set_channel(UpdateChannel::kBeta);
    CHECK(svc.channel() == UpdateChannel::kBeta);
    svc.set_current_version("2.3.0");
    auto info = svc.check_for_update();
    CHECK(info.channel == UpdateChannel::kBeta);
    CHECK(info.current_version == "2.3.0");
}

TEST_CASE("AutoUpdateService: apply on restart", "[phase32][autoupdate]")
{
    AutoUpdateService svc;
    AutoUpdateInfo info;
    info.latest_version = "3.0.0";
    info.update_available = true;
    CHECK(svc.download_update(info));
    CHECK(svc.apply_on_restart(info));
}

// ═══════════════════════════════════════════════════════════════════
// SystemTrayService Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SystemTrayService: show hide and state", "[phase32][tray]")
{
    SystemTrayService tray;
    CHECK_FALSE(tray.is_visible());
    tray.show();
    CHECK(tray.is_visible());
    tray.hide();
    CHECK_FALSE(tray.is_visible());
}

TEST_CASE("SystemTrayService: icon tooltip and menu", "[phase32][tray]")
{
    SystemTrayService tray;
    tray.set_icon("/icons/tray.png");
    tray.set_tooltip("MarkAmp");
    tray.set_menu({
        {"Open", "open", true, false},
        {"", "", false, true}, // separator
        {"Quit", "quit", true, false},
    });
    CHECK(tray.icon_path() == "/icons/tray.png");
    CHECK(tray.tooltip() == "MarkAmp");
    REQUIRE(tray.menu_items().size() == 3);
    CHECK(tray.menu_items()[1].separator);
}

TEST_CASE("SystemTrayService: visibility mode", "[phase32][tray]")
{
    SystemTrayService tray;
    CHECK(tray.visibility_mode() == TrayVisibility::kWhenMinimized);
    tray.set_visibility_mode(TrayVisibility::kAlways);
    CHECK(tray.visibility_mode() == TrayVisibility::kAlways);
}

// ═══════════════════════════════════════════════════════════════════
// ShellIntegration Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ShellIntegration: register and check", "[phase32][shell]")
{
    ShellIntegration shell;
    CHECK_FALSE(shell.are_associations_registered());
    CHECK(shell.register_file_associations());
    CHECK(shell.are_associations_registered());
    CHECK_FALSE(shell.is_protocol_registered());
    CHECK(shell.register_protocol_handler());
    CHECK(shell.is_protocol_registered());
}

TEST_CASE("ShellIntegration: file icons", "[phase32][shell]")
{
    const ShellIntegration shell;
    CHECK_THAT(shell.get_file_icon(".md"), Catch::Matchers::ContainsSubstring("markdown"));
    CHECK_THAT(shell.get_file_icon(".markamp-nb"), Catch::Matchers::ContainsSubstring("notebook"));
    CHECK_THAT(shell.get_file_icon(".markamp-board"), Catch::Matchers::ContainsSubstring("board"));
    CHECK_THAT(shell.get_file_icon(".txt"), Catch::Matchers::ContainsSubstring("generic"));
}

// ═══════════════════════════════════════════════════════════════════
// PlatformKeyboard Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PlatformKeyboard: detect platform", "[phase32][keyboard]")
{
    auto platform = PlatformKeyboard::detect_platform();
#if defined(__APPLE__)
    CHECK(platform == PlatformType::kMacOS);
#elif defined(_WIN32)
    CHECK(platform == PlatformType::kWindows);
#else
    CHECK(platform == PlatformType::kLinux);
#endif
}

TEST_CASE("PlatformKeyboard: primary modifier", "[phase32][keyboard]")
{
    auto mod = PlatformKeyboard::get_primary_modifier();
#if defined(__APPLE__)
    CHECK(mod == KeyModifier::kMeta);
#else
    CHECK(mod == KeyModifier::kCtrl);
#endif
}

TEST_CASE("PlatformKeyboard: get shortcut", "[phase32][keyboard]")
{
    auto save = PlatformKeyboard::get_shortcut(KeyAction::kSave);
    CHECK(save.key == 'S');
    CHECK(save.action == KeyAction::kSave);
    CHECK_FALSE(save.display_string.empty());
}

TEST_CASE("PlatformKeyboard: all shortcuts non-empty", "[phase32][keyboard]")
{
    auto shortcuts = PlatformKeyboard::get_all_shortcuts();
    CHECK(shortcuts.size() >= 20);
    for (const auto& shortcut : shortcuts)
    {
        CHECK_FALSE(shortcut.display_string.empty());
    }
}

TEST_CASE("PlatformKeyboard: format shortcut", "[phase32][keyboard]")
{
    auto copy = PlatformKeyboard::get_shortcut(KeyAction::kCopy);
    auto formatted = PlatformKeyboard::format_shortcut(copy);
#if defined(__APPLE__)
    CHECK_THAT(formatted, Catch::Matchers::ContainsSubstring("C"));
    // Should contain ⌘ symbol
#else
    CHECK_THAT(formatted, Catch::Matchers::ContainsSubstring("Ctrl+C"));
#endif
}

TEST_CASE("PlatformKeyboard: quit shortcut detection", "[phase32][keyboard]")
{
#if defined(__APPLE__)
    CHECK(PlatformKeyboard::is_quit_shortcut('Q', KeyModifier::kMeta));
    CHECK_FALSE(PlatformKeyboard::is_quit_shortcut('Q', KeyModifier::kCtrl));
#else
    CHECK(PlatformKeyboard::is_quit_shortcut('F', KeyModifier::kAlt));
    CHECK_FALSE(PlatformKeyboard::is_quit_shortcut('Q', KeyModifier::kCtrl));
#endif
}

TEST_CASE("PlatformKeyboard: IME support", "[phase32][keyboard]")
{
    CHECK(PlatformKeyboard::supports_ime());
}

TEST_CASE("PlatformKeyboard: macOS menu structure", "[phase32][keyboard]")
{
    auto menus = PlatformKeyboard::get_menu_structure();
#if defined(__APPLE__)
    CHECK(menus.size() >= 5); // MarkAmp, File, Edit, Window, Help
    CHECK(menus[0].title == "MarkAmp");
    CHECK(menus[1].title == "File");
    CHECK(menus[2].title == "Edit");
#else
    CHECK(menus.empty()); // Only macOS gets HIG menus
#endif
}

// ═══════════════════════════════════════════════════════════════════
// PlatformFonts Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PlatformFonts: default code font", "[phase32][fonts]")
{
    auto font = PlatformFonts::get_default_code_font();
    CHECK(font.is_monospace);
    CHECK_FALSE(font.family.empty());
#if defined(__APPLE__)
    CHECK(font.family == "SF Mono");
#elif defined(_WIN32)
    CHECK(font.family == "Cascadia Mono");
#else
    CHECK(font.family == "DejaVu Sans Mono");
#endif
}

TEST_CASE("PlatformFonts: default UI font", "[phase32][fonts]")
{
    auto font = PlatformFonts::get_default_ui_font();
    CHECK_FALSE(font.is_monospace);
    CHECK_FALSE(font.family.empty());
}

TEST_CASE("PlatformFonts: fallback chain non-empty", "[phase32][fonts]")
{
    auto mono_chain = PlatformFonts::get_fallback_chain("SF Mono");
    CHECK(mono_chain.size() >= 5);
    auto sans_chain = PlatformFonts::get_fallback_chain("Arial");
    CHECK(sans_chain.size() >= 5);
}

TEST_CASE("PlatformFonts: CJK font", "[phase32][fonts]")
{
    auto font = PlatformFonts::get_cjk_font();
    CHECK_FALSE(font.family.empty());
#if defined(__APPLE__)
    CHECK(font.family == "Hiragino Sans");
#endif
}

TEST_CASE("PlatformFonts: enumerate system fonts", "[phase32][fonts]")
{
    auto fonts = PlatformFonts::enumerate_system_fonts();
    CHECK(fonts.size() >= 15);
    // Should include both mono and non-mono
    bool has_mono = false;
    bool has_sans = false;
    for (const auto& font : fonts)
    {
        if (font.is_monospace)
            has_mono = true;
        else
            has_sans = true;
    }
    CHECK(has_mono);
    CHECK(has_sans);
}

TEST_CASE("PlatformFonts: font availability", "[phase32][fonts]")
{
    CHECK(PlatformFonts::is_font_available("Arial"));
    CHECK(PlatformFonts::is_font_available("Courier New"));
    CHECK_FALSE(PlatformFonts::is_font_available("ThisFontDoesNotExist42"));
}

// ═══════════════════════════════════════════════════════════════════
// CrossPlatformPath Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("CrossPlatformPath: case sensitivity", "[phase32][path]")
{
#if defined(__linux__)
    CHECK(CrossPlatformPath::is_case_sensitive());
#else
    CHECK_FALSE(CrossPlatformPath::is_case_sensitive());
#endif
}

TEST_CASE("CrossPlatformPath: normalize", "[phase32][path]")
{
    auto normalized = CrossPlatformPath::normalize("/home/user//docs/");
    CHECK_FALSE(normalized.empty());
    // Should remove trailing slash and collapse double slashes
    CHECK(normalized.back() != '/');
}

TEST_CASE("CrossPlatformPath: compare", "[phase32][path]")
{
#if defined(__linux__)
    CHECK_FALSE(CrossPlatformPath::compare("/Foo/Bar", "/foo/bar"));
#else
    CHECK(CrossPlatformPath::compare("/Foo/Bar", "/foo/bar"));
#endif
    CHECK(CrossPlatformPath::compare("/exact/match", "/exact/match"));
}

TEST_CASE("CrossPlatformPath: separator", "[phase32][path]")
{
    auto sep = CrossPlatformPath::get_separator();
#if defined(_WIN32)
    CHECK(sep == '\\');
#else
    CHECK(sep == '/');
#endif
}

TEST_CASE("CrossPlatformPath: max path length", "[phase32][path]")
{
    auto max_len = CrossPlatformPath::get_max_path_length();
    CHECK(max_len > 0);
#if defined(_WIN32)
    CHECK(max_len == 260);
#else
    CHECK(max_len == 4096);
#endif
}

TEST_CASE("CrossPlatformPath: dialog filters", "[phase32][path]")
{
    auto filters = CrossPlatformPath::get_default_dialog_filters();
    REQUIRE(filters.size() >= 3);
    CHECK(filters[0].description == "Markdown Files");
    CHECK_FALSE(filters[0].extensions.empty());
}

TEST_CASE("CrossPlatformPath: last directory memory", "[phase32][path]")
{
    CrossPlatformPath path_svc;
    CHECK(path_svc.get_last_directory().empty());
    path_svc.set_last_directory("/home/user/docs");
    CHECK(path_svc.get_last_directory() == "/home/user/docs");
}

// ═══════════════════════════════════════════════════════════════════
// Platform Event Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Event: PlatformDarkModeChangedEvent", "[phase32][events]")
{
    PlatformDarkModeChangedEvent evt;
    evt.is_dark = true;
    CHECK(evt.is_dark);
}

TEST_CASE("Event: PlatformDpiChangedEvent", "[phase32][events]")
{
    PlatformDpiChangedEvent evt;
    evt.scale_factor = 2.0;
    CHECK(evt.scale_factor == 2.0);
}

TEST_CASE("Event: FileWatchChangeEvent", "[phase32][events]")
{
    FileWatchChangeEvent evt;
    evt.path = "/docs/readme.md";
    evt.change_type = "modified";
    CHECK(evt.path == "/docs/readme.md");
    CHECK(evt.change_type == "modified");
}

TEST_CASE("Event: CrashReportAvailableEvent", "[phase32][events]")
{
    CrashReportAvailableEvent evt;
    evt.report_path = "/tmp/crashes/crash_123.txt";
    CHECK_THAT(evt.report_path, Catch::Matchers::ContainsSubstring("crash"));
}

TEST_CASE("Event: AutoUpdateAvailableEvent", "[phase32][events]")
{
    AutoUpdateAvailableEvent evt;
    evt.version = "3.1.0";
    evt.changelog_url = "https://markamp.dev/changelog/3.1.0";
    CHECK(evt.version == "3.1.0");
    CHECK_THAT(evt.changelog_url, Catch::Matchers::ContainsSubstring("changelog"));
}
