// test_translation_catalog.cpp — 10 tests for TranslationCatalog
#include "core/TranslationCatalog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TranslationCatalog starts empty", "[i18n][translation]")
{
    TranslationCatalog catalog;
    CHECK(catalog.entry_count() == 0);
    CHECK(catalog.locale_count() == 0);
}

TEST_CASE("TranslationCatalog add and translate", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("greeting", "Hello", "en-US");
    auto result = catalog.translate("greeting", "en-US");
    CHECK(result == "Hello");
}

TEST_CASE("TranslationCatalog translate missing returns key", "[i18n][translation]")
{
    TranslationCatalog catalog;
    auto result = catalog.translate("missing_key", "en-US");
    CHECK(result == "missing_key");
}

TEST_CASE("TranslationCatalog has_translation", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("title", "Title", "en-US");
    CHECK(catalog.has_translation("title", "en-US"));
    CHECK_FALSE(catalog.has_translation("title", "fr-FR"));
}

TEST_CASE("TranslationCatalog remove_translation", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("remove_me", "Value", "en-US");
    CHECK(catalog.remove_translation("remove_me", "en-US"));
    CHECK_FALSE(catalog.has_translation("remove_me", "en-US"));
}

TEST_CASE("TranslationCatalog translations_for_locale", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("key1", "Value1", "de-DE");
    catalog.add_translation("key2", "Value2", "de-DE");
    auto entries = catalog.translations_for_locale("de-DE");
    CHECK(entries.size() == 2);
}

TEST_CASE("TranslationCatalog missing_translations", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("key1", "English", "en-US");
    catalog.add_translation("key2", "English2", "en-US");
    catalog.add_translation("key1", "French", "fr-FR");
    auto missing = catalog.missing_translations("fr-FR");
    CHECK_FALSE(missing.empty());
}

TEST_CASE("TranslationCatalog all_keys", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("alpha", "A", "en-US");
    catalog.add_translation("beta", "B", "en-US");
    auto keys = catalog.all_keys();
    CHECK(keys.size() >= 2);
}

TEST_CASE("TranslationCatalog entry_count and locale_count", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("k1", "V1", "en-US");
    catalog.add_translation("k2", "V2", "fr-FR");
    CHECK(catalog.entry_count() == 2);
    CHECK(catalog.locale_count() == 2);
}

TEST_CASE("TranslationCatalog clear removes everything", "[i18n][translation]")
{
    TranslationCatalog catalog;
    catalog.add_translation("k1", "V1", "en-US");
    catalog.clear();
    CHECK(catalog.entry_count() == 0);
    CHECK(catalog.locale_count() == 0);
}
