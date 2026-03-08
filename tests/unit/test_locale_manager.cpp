// test_locale_manager.cpp — 10 tests for LocaleManager
#include "core/LocaleManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LocaleManager starts with zero locales", "[i18n][locale]")
{
    LocaleManager manager;
    CHECK(manager.locale_count() == 0);
}

TEST_CASE("LocaleManager add_locale increases count", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "en-US";
    info.display_name = "English (United States)";
    info.language_code = "en";
    info.region_code = "US";
    manager.add_locale(info);
    CHECK(manager.locale_count() == 1);
}

TEST_CASE("LocaleManager set_locale activates locale", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "en-US";
    info.display_name = "English (United States)";
    info.language_code = "en";
    info.region_code = "US";
    manager.add_locale(info);
    CHECK(manager.set_locale("en-US"));
    CHECK(manager.get_locale().locale_id == "en-US");
}

TEST_CASE("LocaleManager set_locale returns false for unknown", "[i18n][locale]")
{
    LocaleManager manager;
    CHECK_FALSE(manager.set_locale("xx-XX"));
}

TEST_CASE("LocaleManager is_rtl returns false for LTR locale", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "en-US";
    info.text_direction = LocaleTextDirection::kLTR;
    info.is_rtl = false;
    manager.add_locale(info);
    manager.set_locale("en-US");
    CHECK_FALSE(manager.is_rtl());
}

TEST_CASE("LocaleManager is_rtl returns true for RTL locale", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "ar-SA";
    info.display_name = "Arabic (Saudi Arabia)";
    info.language_code = "ar";
    info.region_code = "SA";
    info.text_direction = LocaleTextDirection::kRTL;
    info.is_rtl = true;
    manager.add_locale(info);
    manager.set_locale("ar-SA");
    CHECK(manager.is_rtl());
}

TEST_CASE("LocaleManager find_locale returns pointer for existing", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "ja-JP";
    info.display_name = "Japanese";
    manager.add_locale(info);
    auto found = manager.find_locale("ja-JP");
    REQUIRE(found != nullptr);
    CHECK(found->locale_id == "ja-JP");
}

TEST_CASE("LocaleManager find_locale returns null for missing", "[i18n][locale]")
{
    LocaleManager manager;
    CHECK(manager.find_locale("zz-ZZ") == nullptr);
}

TEST_CASE("LocaleManager available_locales lists all", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo en;
    en.locale_id = "en-US";
    LocaleInfo fr;
    fr.locale_id = "fr-FR";
    manager.add_locale(en);
    manager.add_locale(fr);
    auto available = manager.available_locales();
    CHECK(available.size() == 2);
}

TEST_CASE("LocaleManager clear removes all locales", "[i18n][locale]")
{
    LocaleManager manager;
    LocaleInfo info;
    info.locale_id = "en-US";
    manager.add_locale(info);
    manager.clear();
    CHECK(manager.locale_count() == 0);
}
