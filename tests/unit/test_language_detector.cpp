// test_language_detector.cpp — 10 tests for LanguageDetector
#include "core/LanguageDetector.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LanguageDetector detects markdown from .md", "[lang_detect]")
{
    LanguageDetector detector;
    CHECK(detector.detect("notes.md") == "markdown");
}

TEST_CASE("LanguageDetector detects python from .py", "[lang_detect]")
{
    LanguageDetector detector;
    CHECK(detector.detect("script.py") == "python");
}

TEST_CASE("LanguageDetector detects json from .json", "[lang_detect]")
{
    LanguageDetector detector;
    CHECK(detector.detect("config.json") == "json");
}

TEST_CASE("LanguageDetector detects cpp from .cpp", "[lang_detect]")
{
    LanguageDetector detector;
    auto lang = detector.detect("main.cpp");
    CHECK((lang == "cpp" || lang == "c++"));
}

TEST_CASE("LanguageDetector detects from frontmatter lang field", "[lang_detect]")
{
    LanguageDetector detector;
    auto lang = detector.detect("notes.md", "---\nlang: python\n---\nContent");
    CHECK(lang == "python");
}

TEST_CASE("LanguageDetector detects from shebang", "[lang_detect]")
{
    LanguageDetector detector;
    auto lang = detector.detect("script", "#!/usr/bin/env python3\nprint('hi')");
    CHECK(lang == "python");
}

TEST_CASE("LanguageDetector defaults to markdown for unknown", "[lang_detect]")
{
    LanguageDetector detector;
    auto lang = detector.detect("unknown_file", "");
    CHECK(lang == "markdown");
}

TEST_CASE("LanguageDetector detect_from_extension for .js", "[lang_detect]")
{
    LanguageDetector detector;
    CHECK(detector.detect_from_extension("app.js") == "javascript");
}

TEST_CASE("LanguageDetector detect_from_extension for .ts", "[lang_detect]")
{
    LanguageDetector detector;
    CHECK(detector.detect_from_extension("app.ts") == "typescript");
}

TEST_CASE("LanguageDetector frontmatter takes priority over extension", "[lang_detect]")
{
    LanguageDetector detector;
    auto lang = detector.detect("file.md", "---\nlang: rust\n---\n");
    CHECK(lang == "rust");
}
