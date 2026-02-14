#include "core/ExtensionManifest.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── ActivationEvent Parsing ──

TEST_CASE("ActivationEvent: parse star", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("*");
    REQUIRE(evt.kind == ActivationEventKind::kStar);
    REQUIRE(evt.argument.empty());
    REQUIRE(evt.raw == "*");
}

TEST_CASE("ActivationEvent: parse onStartupFinished", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onStartupFinished");
    REQUIRE(evt.kind == ActivationEventKind::kOnStartupFinished);
    REQUIRE(evt.argument.empty());
}

TEST_CASE("ActivationEvent: parse onLanguage", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onLanguage:markdown");
    REQUIRE(evt.kind == ActivationEventKind::kOnLanguage);
    REQUIRE(evt.argument == "markdown");
    REQUIRE(evt.raw == "onLanguage:markdown");
}

TEST_CASE("ActivationEvent: parse onCommand", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onCommand:myExt.sayHello");
    REQUIRE(evt.kind == ActivationEventKind::kOnCommand);
    REQUIRE(evt.argument == "myExt.sayHello");
}

TEST_CASE("ActivationEvent: parse onView", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onView:myCustomView");
    REQUIRE(evt.kind == ActivationEventKind::kOnView);
    REQUIRE(evt.argument == "myCustomView");
}

TEST_CASE("ActivationEvent: parse onUri", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onUri");
    REQUIRE(evt.kind == ActivationEventKind::kOnUri);
}

TEST_CASE("ActivationEvent: parse onFileSystem", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onFileSystem:ftp");
    REQUIRE(evt.kind == ActivationEventKind::kOnFileSystem);
    REQUIRE(evt.argument == "ftp");
}

TEST_CASE("ActivationEvent: parse onCustomEditor", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onCustomEditor:myViewType");
    REQUIRE(evt.kind == ActivationEventKind::kOnCustomEditor);
    REQUIRE(evt.argument == "myViewType");
}

TEST_CASE("ActivationEvent: parse unknown event", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("onFoo:bar");
    REQUIRE(evt.kind == ActivationEventKind::kUnknown);
    REQUIRE(evt.argument == "bar");
}

TEST_CASE("ActivationEvent: parse string without colon", "[extension-manifest]")
{
    const auto evt = ActivationEvent::parse("something");
    REQUIRE(evt.kind == ActivationEventKind::kUnknown);
}

// ── ExtensionIdentifier ──

TEST_CASE("ExtensionIdentifier: construct and to_string", "[extension-manifest]")
{
    const ExtensionIdentifier ext_id("MyPublisher", "myExtension");
    REQUIRE(ext_id.publisher() == "MyPublisher");
    REQUIRE(ext_id.name() == "myExtension");
    REQUIRE(ext_id.to_string() == "MyPublisher.myExtension");
}

TEST_CASE("ExtensionIdentifier: to_key lowercases", "[extension-manifest]")
{
    const ExtensionIdentifier ext_id("MyPublisher", "MyExtension");
    REQUIRE(ext_id.to_key() == "mypublisher.myextension");
}

TEST_CASE("ExtensionIdentifier: case-insensitive equals", "[extension-manifest]")
{
    const ExtensionIdentifier id_a("Foo", "Bar");
    const ExtensionIdentifier id_b("foo", "bar");
    const ExtensionIdentifier id_c("FOO", "BAR");
    const ExtensionIdentifier id_d("Foo", "Baz");

    REQUIRE(id_a.equals(id_b));
    REQUIRE(id_a.equals(id_c));
    REQUIRE(id_a == id_b);
    REQUIRE_FALSE(id_a.equals(id_d));
    REQUIRE(id_a != id_d);
}

TEST_CASE("ExtensionIdentifier: from_string", "[extension-manifest]")
{
    const auto ext_id = ExtensionIdentifier::from_string("publisher.name");
    REQUIRE(ext_id.publisher() == "publisher");
    REQUIRE(ext_id.name() == "name");
}

TEST_CASE("ExtensionIdentifier: from_string invalid - no dot", "[extension-manifest]")
{
    REQUIRE_THROWS_AS(ExtensionIdentifier::from_string("nodot"), std::runtime_error);
}

TEST_CASE("ExtensionIdentifier: from_string invalid - leading dot", "[extension-manifest]")
{
    REQUIRE_THROWS_AS(ExtensionIdentifier::from_string(".name"), std::runtime_error);
}

TEST_CASE("ExtensionIdentifier: from_string invalid - trailing dot", "[extension-manifest]")
{
    REQUIRE_THROWS_AS(ExtensionIdentifier::from_string("publisher."), std::runtime_error);
}

// ── ManifestParser ──

TEST_CASE("ManifestParser: parse minimal manifest", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "my-extension",
        "version": "1.0.0",
        "publisher": "testPublisher"
    })";

    const auto manifest = ManifestParser::parse(json);
    REQUIRE(manifest.name == "my-extension");
    REQUIRE(manifest.version == "1.0.0");
    REQUIRE(manifest.publisher == "testPublisher");
    REQUIRE(manifest.display_name.empty());
    REQUIRE(manifest.activation_events.empty());
    REQUIRE(manifest.contributes.commands.empty());
}

TEST_CASE("ManifestParser: parse full manifest", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "markdown-extras",
        "version": "2.1.3",
        "publisher": "markamp",
        "displayName": "Markdown Extras",
        "description": "Extra markdown features",
        "icon": "images/icon.png",
        "license": "MIT",
        "main": "./out/extension.js",
        "engines": { "vscode": "^1.60.0" },
        "activationEvents": ["onLanguage:markdown", "onCommand:ext.insertToc"],
        "categories": ["Programming Languages", "Formatters"],
        "keywords": ["markdown", "toc"],
        "extensionDependencies": ["ms-vscode.markdown"],
        "extensionPack": ["ext.pack1", "ext.pack2"],
        "repository": { "type": "git", "url": "https://github.com/foo/bar" },
        "bugs": "https://github.com/foo/bar/issues",
        "contributes": {
            "commands": [
                {
                    "command": "ext.insertToc",
                    "title": "Insert TOC",
                    "category": "Markdown"
                },
                {
                    "command": "ext.formatTable",
                    "title": "Format Table"
                }
            ],
            "keybindings": [
                {
                    "command": "ext.insertToc",
                    "key": "ctrl+shift+t",
                    "mac": "cmd+shift+t",
                    "when": "editorTextFocus"
                }
            ],
            "languages": [
                {
                    "id": "markdown",
                    "extensions": [".md", ".mdx"],
                    "aliases": ["Markdown", "md"]
                }
            ],
            "grammars": [
                {
                    "language": "markdown",
                    "scopeName": "source.markdown",
                    "path": "./syntaxes/markdown.tmLanguage.json"
                }
            ],
            "themes": [
                {
                    "id": "monokai",
                    "label": "Monokai",
                    "uiTheme": "vs-dark",
                    "path": "./themes/monokai.json"
                }
            ],
            "snippets": [
                {
                    "language": "markdown",
                    "path": "./snippets/md.json"
                }
            ],
            "configuration": {
                "title": "Markdown Extras Settings",
                "properties": {
                    "markdownExtras.autoToc": {
                        "type": "boolean",
                        "description": "Auto-generate TOC",
                        "default": "true"
                    },
                    "markdownExtras.tocDepth": {
                        "type": "number",
                        "description": "Max heading depth for TOC",
                        "default": "3",
                        "enum": ["1", "2", "3", "4", "5", "6"]
                    }
                }
            }
        }
    })";

    const auto manifest = ManifestParser::parse(json);

    // Basic fields
    REQUIRE(manifest.name == "markdown-extras");
    REQUIRE(manifest.version == "2.1.3");
    REQUIRE(manifest.publisher == "markamp");
    REQUIRE(manifest.display_name == "Markdown Extras");
    REQUIRE(manifest.description == "Extra markdown features");
    REQUIRE(manifest.icon == "images/icon.png");
    REQUIRE(manifest.license == "MIT");
    REQUIRE(manifest.main == "./out/extension.js");
    REQUIRE(manifest.engines_vscode == "^1.60.0");

    // Activation events
    REQUIRE(manifest.activation_events.size() == 2);
    REQUIRE(manifest.activation_events[0].kind == ActivationEventKind::kOnLanguage);
    REQUIRE(manifest.activation_events[0].argument == "markdown");
    REQUIRE(manifest.activation_events[1].kind == ActivationEventKind::kOnCommand);
    REQUIRE(manifest.activation_events[1].argument == "ext.insertToc");

    // Categories, keywords
    REQUIRE(manifest.categories.size() == 2);
    REQUIRE(manifest.keywords.size() == 2);

    // Dependencies
    REQUIRE(manifest.extension_dependencies.size() == 1);
    REQUIRE(manifest.extension_dependencies[0] == "ms-vscode.markdown");
    REQUIRE(manifest.extension_pack.size() == 2);

    // Repository
    REQUIRE(manifest.repository.has_value());
    REQUIRE(manifest.repository->type == "git");
    REQUIRE(manifest.repository->url == "https://github.com/foo/bar");
    REQUIRE(manifest.bugs_url == "https://github.com/foo/bar/issues");

    // Commands
    REQUIRE(manifest.contributes.commands.size() == 2);
    REQUIRE(manifest.contributes.commands[0].command == "ext.insertToc");
    REQUIRE(manifest.contributes.commands[0].title == "Insert TOC");
    REQUIRE(manifest.contributes.commands[0].category == "Markdown");
    REQUIRE(manifest.contributes.commands[1].command == "ext.formatTable");

    // Keybindings
    REQUIRE(manifest.contributes.keybindings.size() == 1);
    REQUIRE(manifest.contributes.keybindings[0].command == "ext.insertToc");
    REQUIRE(manifest.contributes.keybindings[0].key == "ctrl+shift+t");
    REQUIRE(manifest.contributes.keybindings[0].mac == "cmd+shift+t");
    REQUIRE(manifest.contributes.keybindings[0].when == "editorTextFocus");

    // Languages
    REQUIRE(manifest.contributes.languages.size() == 1);
    REQUIRE(manifest.contributes.languages[0].language_id == "markdown");
    REQUIRE(manifest.contributes.languages[0].extensions.size() == 2);
    REQUIRE(manifest.contributes.languages[0].aliases.size() == 2);

    // Grammars
    REQUIRE(manifest.contributes.grammars.size() == 1);
    REQUIRE(manifest.contributes.grammars[0].scope_name == "source.markdown");

    // Themes
    REQUIRE(manifest.contributes.themes.size() == 1);
    REQUIRE(manifest.contributes.themes[0].label == "Monokai");
    REQUIRE(manifest.contributes.themes[0].ui_theme == "vs-dark");

    // Snippets
    REQUIRE(manifest.contributes.snippets.size() == 1);
    REQUIRE(manifest.contributes.snippets[0].language == "markdown");

    // Configuration
    REQUIRE(manifest.contributes.configuration.size() == 1);
    REQUIRE(manifest.contributes.configuration[0].title == "Markdown Extras Settings");
    REQUIRE(manifest.contributes.configuration[0].properties.size() == 2);
    REQUIRE(manifest.contributes.configuration[0].properties[0].key == "markdownExtras.autoToc");
    REQUIRE(manifest.contributes.configuration[0].properties[0].type == "boolean");

    // Check second config property has enum values
    bool found_toc_depth = false;
    for (const auto& prop : manifest.contributes.configuration[0].properties)
    {
        if (prop.key == "markdownExtras.tocDepth")
        {
            found_toc_depth = true;
            REQUIRE(prop.enum_values.size() == 6);
        }
    }
    REQUIRE(found_toc_depth);

    // Identifier
    const auto ext_id = manifest.identifier();
    REQUIRE(ext_id.to_string() == "markamp.markdown-extras");
    REQUIRE(ext_id.to_key() == "markamp.markdown-extras");
}

TEST_CASE("ManifestParser: missing name throws", "[extension-manifest]")
{
    const std::string json = R"({
        "version": "1.0.0",
        "publisher": "test"
    })";
    REQUIRE_THROWS_AS(ManifestParser::parse(json), std::runtime_error);
}

TEST_CASE("ManifestParser: missing version throws", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "test",
        "publisher": "test"
    })";
    REQUIRE_THROWS_AS(ManifestParser::parse(json), std::runtime_error);
}

TEST_CASE("ManifestParser: missing publisher throws", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "test",
        "version": "1.0.0"
    })";
    REQUIRE_THROWS_AS(ManifestParser::parse(json), std::runtime_error);
}

TEST_CASE("ManifestParser: invalid JSON throws", "[extension-manifest]")
{
    const std::string json = "{ this is not valid json }";
    REQUIRE_THROWS_AS(ManifestParser::parse(json), std::runtime_error);
}

TEST_CASE("ManifestParser: root is not object throws", "[extension-manifest]")
{
    const std::string json = R"(["not", "an", "object"])";
    REQUIRE_THROWS_AS(ManifestParser::parse(json), std::runtime_error);
}

TEST_CASE("ManifestParser: configuration as array", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "test",
        "version": "1.0.0",
        "publisher": "pub",
        "contributes": {
            "configuration": [
                {
                    "title": "Section A",
                    "properties": {
                        "test.setting1": { "type": "boolean", "description": "Setting 1" }
                    }
                },
                {
                    "title": "Section B",
                    "properties": {
                        "test.setting2": { "type": "string", "description": "Setting 2" }
                    }
                }
            ]
        }
    })";

    const auto manifest = ManifestParser::parse(json);
    REQUIRE(manifest.contributes.configuration.size() == 2);
    REQUIRE(manifest.contributes.configuration[0].title == "Section A");
    REQUIRE(manifest.contributes.configuration[0].properties.size() == 1);
    REQUIRE(manifest.contributes.configuration[1].title == "Section B");
    REQUIRE(manifest.contributes.configuration[1].properties.size() == 1);
}

TEST_CASE("ManifestParser: repository as string", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "test",
        "version": "1.0.0",
        "publisher": "pub",
        "repository": "https://github.com/test/repo"
    })";

    const auto manifest = ManifestParser::parse(json);
    REQUIRE(manifest.repository.has_value());
    REQUIRE(manifest.repository->url == "https://github.com/test/repo");
    REQUIRE(manifest.repository->type.empty());
}

TEST_CASE("ManifestParser: empty contributes is fine", "[extension-manifest]")
{
    const std::string json = R"({
        "name": "test",
        "version": "1.0.0",
        "publisher": "pub",
        "contributes": {}
    })";

    const auto manifest = ManifestParser::parse(json);
    REQUIRE(manifest.contributes.commands.empty());
    REQUIRE(manifest.contributes.keybindings.empty());
    REQUIRE(manifest.contributes.themes.empty());
}
