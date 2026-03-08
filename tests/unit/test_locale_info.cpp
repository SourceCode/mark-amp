// test_locale_info.cpp — 10 tests for LocaleInfo struct
#include "core/LocaleManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LocaleInfo default fields", "[i18n][localeinfo]")
{
    LocaleInfo info;
    CHECK(info.locale_id.empty());
    CHECK(info.display_name.empty());
    CHECK(info.language_code.empty());
    CHECK(info.region_code.empty());
    CHECK(info.text_direction == LocaleTextDirection::kLTR);
    CHECK_FALSE(info.is_rtl);
}

TEST_CASE("LocaleInfo populate en-US", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "en-US";
    info.display_name = "English (United States)";
    info.language_code = "en";
    info.region_code = "US";
    CHECK(info.locale_id == "en-US");
    CHECK(info.language_code == "en");
    CHECK(info.region_code == "US");
}

TEST_CASE("LocaleInfo RTL locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "ar-SA";
    info.display_name = "Arabic (Saudi Arabia)";
    info.language_code = "ar";
    info.region_code = "SA";
    info.text_direction = LocaleTextDirection::kRTL;
    info.is_rtl = true;
    CHECK(info.is_rtl);
    CHECK(info.text_direction == LocaleTextDirection::kRTL);
}

TEST_CASE("LocaleInfo Chinese locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "zh-CN";
    info.display_name = "Chinese (Simplified)";
    info.language_code = "zh";
    info.region_code = "CN";
    info.text_direction = LocaleTextDirection::kLTR;
    CHECK(info.locale_id == "zh-CN");
    CHECK_FALSE(info.is_rtl);
}

TEST_CASE("LocaleInfo Japanese locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "ja-JP";
    info.display_name = "Japanese";
    info.language_code = "ja";
    info.region_code = "JP";
    CHECK(info.locale_id == "ja-JP");
    CHECK(info.language_code == "ja");
}

TEST_CASE("LocaleTextDirection enum values", "[i18n][localeinfo]")
{
    CHECK(LocaleTextDirection::kLTR != LocaleTextDirection::kRTL);
    CHECK(static_cast<uint8_t>(LocaleTextDirection::kLTR) == 0);
    CHECK(static_cast<uint8_t>(LocaleTextDirection::kRTL) == 1);
}

TEST_CASE("LocaleInfo German locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "de-DE";
    info.display_name = "German (Germany)";
    info.language_code = "de";
    info.region_code = "DE";
    CHECK(info.locale_id == "de-DE");
    CHECK(info.display_name == "German (Germany)");
}

TEST_CASE("LocaleInfo Korean locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "ko-KR";
    info.display_name = "Korean";
    info.language_code = "ko";
    info.region_code = "KR";
    CHECK(info.language_code == "ko");
    CHECK_FALSE(info.is_rtl);
}

TEST_CASE("LocaleInfo Hebrew (RTL)", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "he-IL";
    info.display_name = "Hebrew";
    info.language_code = "he";
    info.region_code = "IL";
    info.text_direction = LocaleTextDirection::kRTL;
    info.is_rtl = true;
    CHECK(info.is_rtl);
}

TEST_CASE("LocaleInfo Spanish locale", "[i18n][localeinfo]")
{
    LocaleInfo info;
    info.locale_id = "es-ES";
    info.display_name = "Spanish (Spain)";
    info.language_code = "es";
    info.region_code = "ES";
    CHECK(info.locale_id == "es-ES");
    CHECK(info.region_code == "ES");
}
