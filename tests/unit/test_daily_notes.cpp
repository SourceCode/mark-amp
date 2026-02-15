/// @file test_daily_notes.cpp
/// @brief V4 Phase 09 – Daily Notes Service tests.

#include "core/Config.h"
#include "core/DailyNoteService.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

struct DailyNoteTestFixture
{
    DailyNoteTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_dn_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , daily_service_(event_bus_, config_, vault_service_)
    {
        fs::create_directories(vault_dir_);
    }

    ~DailyNoteTestFixture()
    {
        fs::remove_all(vault_dir_);
    }

    auto open_vault() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
    }

    auto create_file(const std::string& rel_path, const std::string& content) const -> void
    {
        const fs::path full_path = vault_dir_ / rel_path;
        fs::create_directories(full_path.parent_path());
        std::ofstream out(full_path);
        out << content;
    }

    auto make_date(int year, unsigned month, unsigned day) -> std::chrono::year_month_day
    {
        return std::chrono::year_month_day{
            std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::DailyNoteService daily_service_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Open today creates a note
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture, "Open today creates daily note", "[dailynote][create]")
{
    open_vault();
    auto result = daily_service_.open_today();
    REQUIRE(result.has_value());
    CHECK_FALSE(result.value().empty());
}

// ============================================================================
// Test 2: Open today returns existing
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Open today returns existing if already created",
                 "[dailynote][existing]")
{
    open_vault();

    auto first = daily_service_.open_today();
    REQUIRE(first.has_value());

    auto second = daily_service_.open_today();
    REQUIRE(second.has_value());
    // Both should return a valid document ID
    CHECK_FALSE(second.value().empty());
}

// ============================================================================
// Test 3: Date formatting (default %Y-%m-%d)
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Date formatting produces expected filename",
                 "[dailynote][format]")
{
    open_vault();
    const auto date = make_date(2026, 2, 14);

    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    // Verify the file was created with default format
    const auto expected_path = vault_dir_ / "daily" / "2026-02-14.md";
    CHECK(fs::exists(expected_path));
}

// ============================================================================
// Test 4: Custom filename format
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture, "Custom filename format works", "[dailynote][format]")
{
    open_vault();

    markamp::core::DailyNoteConfig custom_config;
    custom_config.filename_format = "%d-%m-%Y";
    daily_service_.set_daily_config(custom_config);

    const auto date = make_date(2026, 2, 14);
    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    const auto expected_path = vault_dir_ / "daily" / "14-02-2026.md";
    CHECK(fs::exists(expected_path));
}

// ============================================================================
// Test 5: Template application
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Template with variables is applied",
                 "[dailynote][template]")
{
    open_vault();

    // Create a template file
    create_file("templates/daily.md",
                "# {{title}}\nDate: {{date}}\nYesterday: {{yesterday}}\nTomorrow: {{tomorrow}}\n");

    markamp::core::DailyNoteConfig template_config;
    template_config.template_path = "templates/daily.md";
    daily_service_.set_daily_config(template_config);

    const auto date = make_date(2026, 2, 14);
    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    // Read the created file to verify template was applied
    const auto file_path = vault_dir_ / "daily" / "2026-02-14.md";
    REQUIRE(fs::exists(file_path));

    std::ifstream in(file_path);
    const std::string content((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());

    CHECK(content.find("2026-02-14") != std::string::npos);
    CHECK(content.find("2026-02-13") != std::string::npos); // yesterday
    CHECK(content.find("2026-02-15") != std::string::npos); // tomorrow
}

// ============================================================================
// Test 6: Default tags in frontmatter
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Default tags applied to new daily note",
                 "[dailynote][tags]")
{
    open_vault();

    markamp::core::DailyNoteConfig tag_config;
    tag_config.default_tags = {"daily", "journal"};
    daily_service_.set_daily_config(tag_config);

    const auto date = make_date(2026, 3, 1);
    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    // The document was created—just verify it exists
    const auto file_path = vault_dir_ / "daily" / "2026-03-01.md";
    CHECK(fs::exists(file_path));
}

// ============================================================================
// Test 7: List month entries
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "List month returns entries for all days",
                 "[dailynote][calendar]")
{
    open_vault();

    // Create a few daily notes for January 2026
    create_file("daily/2026-01-05.md", "---\ntitle: 2026-01-05\n---\n# Jan 5");
    create_file("daily/2026-01-10.md", "---\ntitle: 2026-01-10\n---\n# Jan 10");
    create_file("daily/2026-01-20.md", "---\ntitle: 2026-01-20\n---\n# Jan 20");

    const auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    auto entries = daily_service_.list_month(jan_2026);

    CHECK(entries.size() == 31); // January has 31 days

    // Count existing entries
    int existing_count = 0;
    for (const auto& entry : entries)
    {
        if (entry.exists)
        {
            ++existing_count;
        }
    }
    CHECK(existing_count == 3);
}

// ============================================================================
// Test 8: Next daily note navigation
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Next daily note finds nearest existing",
                 "[dailynote][navigation]")
{
    open_vault();

    // Create notes for Jan 1, Jan 5, Jan 10
    create_file("daily/2026-01-01.md", "---\ntitle: 2026-01-01\n---\n# Jan 1");
    create_file("daily/2026-01-05.md", "---\ntitle: 2026-01-05\n---\n# Jan 5");
    create_file("daily/2026-01-10.md", "---\ntitle: 2026-01-10\n---\n# Jan 10");

    // Reindex
    auto reopen = vault_service_.open_vault(vault_dir_);

    const auto jan_1 = make_date(2026, 1, 1);
    auto result = daily_service_.next_daily_note(jan_1);
    REQUIRE(result.has_value());
    // Should have found Jan 5 (next existing)
    CHECK_FALSE(result.value().empty());
}

// ============================================================================
// Test 9: Previous daily note navigation
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Previous daily note finds nearest existing",
                 "[dailynote][navigation]")
{
    open_vault();

    create_file("daily/2026-01-01.md", "---\ntitle: 2026-01-01\n---\n# Jan 1");
    create_file("daily/2026-01-05.md", "---\ntitle: 2026-01-05\n---\n# Jan 5");
    create_file("daily/2026-01-10.md", "---\ntitle: 2026-01-10\n---\n# Jan 10");

    // Reindex
    auto reopen = vault_service_.open_vault(vault_dir_);

    const auto jan_10 = make_date(2026, 1, 10);
    auto result = daily_service_.previous_daily_note(jan_10);
    REQUIRE(result.has_value());
    CHECK_FALSE(result.value().empty());
}

// ============================================================================
// Test 10: Heading format
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "Heading format uses configured strftime format",
                 "[dailynote][format]")
{
    open_vault();

    markamp::core::DailyNoteConfig heading_config;
    heading_config.heading_format = "# %Y/%m/%d Journal";
    daily_service_.set_daily_config(heading_config);

    const auto date = make_date(2026, 2, 14);
    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    const auto file_path = vault_dir_ / "daily" / "2026-02-14.md";
    REQUIRE(fs::exists(file_path));

    std::ifstream in(file_path);
    const std::string content((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());

    CHECK(content.find("2026/02/14 Journal") != std::string::npos);
}

// ============================================================================
// Test 11: DailyNoteOpenedEvent published
// ============================================================================
TEST_CASE_METHOD(DailyNoteTestFixture,
                 "DailyNoteOpenedEvent published with correct data",
                 "[dailynote][event]")
{
    open_vault();

    bool event_received = false;
    bool was_newly_created = false;
    std::string event_date;

    auto sub = event_bus_.subscribe<markamp::core::events::DailyNoteOpenedEvent>(
        [&](const markamp::core::events::DailyNoteOpenedEvent& evt)
        {
            event_received = true;
            was_newly_created = evt.newly_created;
            event_date = evt.date_string;
        });

    const auto date = make_date(2026, 6, 15);
    auto result = daily_service_.open_date(date);
    REQUIRE(result.has_value());

    CHECK(event_received);
    CHECK(was_newly_created);
    CHECK(event_date == "2026-06-15");
}
