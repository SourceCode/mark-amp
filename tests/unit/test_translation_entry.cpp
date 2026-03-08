// test_translation_entry.cpp — 10 tests for TranslationEntry struct
#include "core/TranslationCatalog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TranslationEntry defaults", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    CHECK(entry.key.empty());
    CHECK(entry.value.empty());
    CHECK(entry.locale_id.empty());
    CHECK(entry.context.empty());
}

TEST_CASE("TranslationEntry with values", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "btn.save";
    entry.value = "Save";
    entry.locale_id = "en-US";
    entry.context = "Button label";
    CHECK(entry.key == "btn.save");
    CHECK(entry.value == "Save");
    CHECK(entry.locale_id == "en-US");
    CHECK(entry.context == "Button label");
}

TEST_CASE("TranslationEntry different locales", "[i18n][translation_entry]")
{
    TranslationEntry en;
    en.key = "greeting";
    en.value = "Hello";
    en.locale_id = "en-US";
    TranslationEntry fr;
    fr.key = "greeting";
    fr.value = "Bonjour";
    fr.locale_id = "fr-FR";
    CHECK(en.value != fr.value);
    CHECK(en.key == fr.key);
}

TEST_CASE("TranslationEntry long value", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "long.text";
    entry.value = "This is a very long translation string that might span multiple words";
    entry.locale_id = "en-US";
    CHECK(entry.value.size() > 50);
}

TEST_CASE("TranslationEntry with context disambiguation", "[i18n][translation_entry]")
{
    TranslationEntry menu_open;
    menu_open.key = "open";
    menu_open.value = "Open";
    menu_open.locale_id = "en-US";
    menu_open.context = "Menu item";
    TranslationEntry state_open;
    state_open.key = "open";
    state_open.value = "Open";
    state_open.locale_id = "en-US";
    state_open.context = "State label";
    CHECK(menu_open.context != state_open.context);
}

TEST_CASE("TranslationEntry empty context", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "title";
    entry.value = "App Title";
    entry.locale_id = "en-US";
    CHECK(entry.context.empty());
}

TEST_CASE("TranslationEntry RTL locale", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "greeting";
    entry.value = "مرحبا";
    entry.locale_id = "ar-SA";
    CHECK(entry.locale_id == "ar-SA");
    CHECK_FALSE(entry.value.empty());
}

TEST_CASE("TranslationEntry CJK locale", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "greeting";
    entry.value = "こんにちは";
    entry.locale_id = "ja-JP";
    CHECK_FALSE(entry.value.empty());
}

TEST_CASE("TranslationEntry key format", "[i18n][translation_entry]")
{
    TranslationEntry entry;
    entry.key = "menu.file.new";
    entry.value = "New File";
    entry.locale_id = "en-US";
    CHECK(entry.key.find('.') != std::string::npos);
}

TEST_CASE("TranslationCatalog add with context", "[i18n][translation_entry]")
{
    TranslationCatalog catalog;
    catalog.add_translation("btn.ok", "OK", "en-US", "Dialog button");
    CHECK(catalog.has_translation("btn.ok", "en-US"));
    CHECK(catalog.translate("btn.ok", "en-US") == "OK");
}
