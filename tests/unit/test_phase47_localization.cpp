/// @file test_phase47_localization.cpp
/// @brief Phase 47 tests — Localization & Internationalization.

#include "core/Events.h"
#include "core/InternationalizationCommandProvider.h"
#include "core/LocaleManager.h"
#include "core/LocalizationCommandProvider.h"
#include "core/StringFormatter.h"
#include "core/TextDirectionEngine.h"
#include "core/TranslationCatalog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// LocaleManager
// ============================================================================

TEST_CASE("LocaleManager initial state", "[phase47][locale]")
{
    LocaleManager mgr;
    REQUIRE(mgr.locale_count() == 0);
}

TEST_CASE("LocaleManager load defaults", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    REQUIRE(mgr.locale_count() == 5);
}

TEST_CASE("LocaleManager first locale becomes active", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    REQUIRE(mgr.get_locale().locale_id == "en-US");
}

TEST_CASE("LocaleManager set locale", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    REQUIRE(mgr.set_locale("fr-FR"));
    REQUIRE(mgr.get_locale().locale_id == "fr-FR");
    REQUIRE(mgr.get_locale().display_name == "Français (France)");
}

TEST_CASE("LocaleManager set nonexistent locale fails", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    REQUIRE_FALSE(mgr.set_locale("zh-CN"));
}

TEST_CASE("LocaleManager find locale", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    const auto* jp = mgr.find_locale("ja-JP");
    REQUIRE(jp != nullptr);
    REQUIRE(jp->language_code == "ja");
    REQUIRE(jp->region_code == "JP");
}

TEST_CASE("LocaleManager add custom locale", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    LocaleInfo ar_sa;
    ar_sa.locale_id = "ar-SA";
    ar_sa.display_name = "العربية (المملكة العربية السعودية)";
    ar_sa.language_code = "ar";
    ar_sa.region_code = "SA";
    ar_sa.text_direction = LocaleTextDirection::kRTL;
    ar_sa.is_rtl = true;
    mgr.add_locale(ar_sa);
    REQUIRE(mgr.locale_count() == 6);
    REQUIRE(mgr.set_locale("ar-SA"));
    REQUIRE(mgr.is_rtl());
}

TEST_CASE("LocaleManager available locales", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    auto all = mgr.available_locales();
    REQUIRE(all.size() == 5);
}

TEST_CASE("LocaleManager clear", "[phase47][locale]")
{
    LocaleManager mgr;
    mgr.load_defaults();
    mgr.clear();
    REQUIRE(mgr.locale_count() == 0);
}

// ============================================================================
// TranslationCatalog
// ============================================================================

TEST_CASE("TranslationCatalog initial state", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    REQUIRE(catalog.entry_count() == 0);
    REQUIRE(catalog.locale_count() == 0);
}

TEST_CASE("TranslationCatalog add and translate", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("app.title", "MarkAmp", "en-US");
    REQUIRE(catalog.translate("app.title", "en-US") == "MarkAmp");
}

TEST_CASE("TranslationCatalog missing key falls back to key", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    REQUIRE(catalog.translate("missing.key", "en-US") == "missing.key");
}

TEST_CASE("TranslationCatalog has_translation", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("menu.file", "File", "en-US");
    REQUIRE(catalog.has_translation("menu.file", "en-US"));
    REQUIRE_FALSE(catalog.has_translation("menu.file", "fr-FR"));
}

TEST_CASE("TranslationCatalog remove translation", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("test", "value", "en-US");
    REQUIRE(catalog.remove_translation("test", "en-US"));
    REQUIRE_FALSE(catalog.has_translation("test", "en-US"));
}

TEST_CASE("TranslationCatalog load defaults", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.load_defaults();
    REQUIRE(catalog.entry_count() == 10);
    REQUIRE(catalog.translate("action.save", "en-US") == "Save");
}

TEST_CASE("TranslationCatalog multi-locale", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("menu.file", "File", "en-US");
    catalog.add_translation("menu.file", "Fichier", "fr-FR");
    REQUIRE(catalog.translate("menu.file", "en-US") == "File");
    REQUIRE(catalog.translate("menu.file", "fr-FR") == "Fichier");
    REQUIRE(catalog.locale_count() == 2);
}

TEST_CASE("TranslationCatalog missing translations", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("app.title", "MarkAmp", "en-US");
    catalog.add_translation("menu.file", "File", "en-US");
    // fr-FR has only one
    catalog.add_translation("app.title", "MarkAmp", "fr-FR");
    auto missing = catalog.missing_translations("fr-FR");
    REQUIRE(missing.size() == 1);
    REQUIRE(missing[0] == "menu.file");
}

TEST_CASE("TranslationCatalog all keys", "[phase47][catalog]")
{
    TranslationCatalog catalog;
    catalog.add_translation("a", "1", "en-US");
    catalog.add_translation("b", "2", "en-US");
    auto keys = catalog.all_keys();
    REQUIRE(keys.size() == 2);
}

// ============================================================================
// StringFormatter
// ============================================================================

TEST_CASE("StringFormatter format number", "[phase47][format]")
{
    StringFormatter fmt;
    auto result = fmt.format_number(1234567.89);
    REQUIRE(result == "1,234,567.89");
}

TEST_CASE("StringFormatter format number custom", "[phase47][format]")
{
    StringFormatter fmt;
    NumberFormat nf;
    nf.decimal_separator = ',';
    nf.thousands_separator = '.';
    nf.decimal_places = 2;
    auto result = fmt.format_number(1234.50, nf);
    REQUIRE(result == "1.234,50");
}

TEST_CASE("StringFormatter format plural singular", "[phase47][format]")
{
    REQUIRE(StringFormatter::format_plural(1, "file", "files") == "1 file");
}

TEST_CASE("StringFormatter format plural", "[phase47][format]")
{
    REQUIRE(StringFormatter::format_plural(5, "file", "files") == "5 files");
}

TEST_CASE("StringFormatter format template", "[phase47][format]")
{
    auto result =
        StringFormatter::format_template("Hello {0}, you have {1} messages", {"Alice", "3"});
    REQUIRE(result == "Hello Alice, you have 3 messages");
}

TEST_CASE("StringFormatter format date ISO", "[phase47][format]")
{
    StringFormatter fmt;
    // Use epoch for predictable test
    auto epoch = std::chrono::system_clock::from_time_t(0);
    auto result = fmt.format_date(epoch, DateFormat::kISO8601);
    REQUIRE(result == "1970-01-01");
}

TEST_CASE("StringFormatter locale", "[phase47][format]")
{
    StringFormatter fmt;
    REQUIRE(fmt.get_locale() == "en-US");
    fmt.set_locale("de-DE");
    REQUIRE(fmt.get_locale() == "de-DE");
}

// ============================================================================
// TextDirectionEngine
// ============================================================================

TEST_CASE("TextDirectionEngine default LTR", "[phase47][bidi]")
{
    TextDirectionEngine engine;
    REQUIRE(engine.get_default_direction() == TextDirection::kLTR);
}

TEST_CASE("TextDirectionEngine detect LTR", "[phase47][bidi]")
{
    TextDirectionEngine engine;
    auto dir = engine.detect_direction("Hello world");
    REQUIRE(dir == TextDirection::kLTR);
}

TEST_CASE("TextDirectionEngine detect empty uses default", "[phase47][bidi]")
{
    TextDirectionEngine engine;
    engine.set_default_direction(TextDirection::kRTL);
    REQUIRE(engine.detect_direction("") == TextDirection::kRTL);
}

TEST_CASE("TextDirectionEngine segment text", "[phase47][bidi]")
{
    TextDirectionEngine engine;
    auto segments = engine.segment_text("Hello");
    REQUIRE(segments.size() == 1);
    REQUIRE(segments[0].text == "Hello");
    REQUIRE(segments[0].direction == TextDirection::kLTR);
}

TEST_CASE("TextDirectionEngine apply bidi markers", "[phase47][bidi]")
{
    auto marked = TextDirectionEngine::apply_bidi_markers("Hello", TextDirection::kLTR);
    REQUIRE(marked.size() > 5); // Has markers
    auto stripped = TextDirectionEngine::strip_bidi_markers(marked);
    REQUIRE(stripped == "Hello");
}

TEST_CASE("TextDirectionEngine strip markers roundtrip", "[phase47][bidi]")
{
    auto marked = TextDirectionEngine::apply_bidi_markers("Test", TextDirection::kRTL);
    auto stripped = TextDirectionEngine::strip_bidi_markers(marked);
    REQUIRE(stripped == "Test");
}

TEST_CASE("TextDirectionEngine auto no markers", "[phase47][bidi]")
{
    auto result = TextDirectionEngine::apply_bidi_markers("Hello", TextDirection::kAuto);
    REQUIRE(result == "Hello");
}

// ============================================================================
// LocalizationCommandProvider
// ============================================================================

TEST_CASE("LocalizationCommandProvider provides 8 commands", "[phase47][commands]")
{
    REQUIRE(LocalizationCommandProvider::command_count() == 8);
    REQUIRE(LocalizationCommandProvider::command_ids().size() == 8);
}

TEST_CASE("LocalizationCommandProvider lookup", "[phase47][commands]")
{
    LocalizationCommandProvider provider;
    auto cmd = provider.get_command("locale.change");
    REQUIRE(cmd.id == "locale.change");
    REQUIRE(cmd.category == "Localization");
}

// ============================================================================
// InternationalizationCommandProvider
// ============================================================================

TEST_CASE("InternationalizationCommandProvider provides 8 commands", "[phase47][commands]")
{
    REQUIRE(InternationalizationCommandProvider::command_count() == 8);
    REQUIRE(InternationalizationCommandProvider::command_ids().size() == 8);
}

TEST_CASE("InternationalizationCommandProvider lookup", "[phase47][commands]")
{
    InternationalizationCommandProvider provider;
    auto cmd = provider.get_command("i18n.formatNumber");
    REQUIRE(cmd.id == "i18n.formatNumber");
    REQUIRE(cmd.category == "Internationalization");
}

// ============================================================================
// Phase 47 Events
// ============================================================================

TEST_CASE("LocaleChangedEvent fields", "[phase47][events]")
{
    events::LocaleChangedEvent evt;
    evt.old_locale = "en-US";
    evt.new_locale = "fr-FR";
    REQUIRE(evt.new_locale == "fr-FR");
}

TEST_CASE("TranslationAddedEvent fields", "[phase47][events]")
{
    events::TranslationAddedEvent evt;
    evt.key = "app.title";
    evt.locale_id = "en-US";
    REQUIRE(evt.key == "app.title");
}

TEST_CASE("TranslationMissingEvent fields", "[phase47][events]")
{
    events::TranslationMissingEvent evt;
    evt.key = "missing.key";
    evt.locale_id = "fr-FR";
    REQUIRE(evt.locale_id == "fr-FR");
}

TEST_CASE("TextDirectionChangedEvent fields", "[phase47][events]")
{
    events::TextDirectionChangedEvent evt;
    evt.direction = "rtl";
    REQUIRE(evt.direction == "rtl");
}

TEST_CASE("CatalogLoadedEvent fields", "[phase47][events]")
{
    events::CatalogLoadedEvent evt;
    evt.entries_loaded = 100;
    evt.locales_loaded = 5;
    REQUIRE(evt.entries_loaded == 100);
}

TEST_CASE("FormatterLocaleChangedEvent fields", "[phase47][events]")
{
    events::FormatterLocaleChangedEvent evt;
    evt.old_locale = "en-US";
    evt.new_locale = "de-DE";
    REQUIRE(evt.new_locale == "de-DE");
}
