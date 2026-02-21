#include "rendering/JsonRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::rendering;

TEST_CASE("JsonRenderer handles valid JSON", "[rendering][json]")
{
    JsonRenderer renderer;
    std::string json = "{\"name\": \"markamp\", \"version\": 1.0, \"active\": true, \"tags\": "
                       "[\"cpp\", \"markdown\"]}";

    std::string html = renderer.render(json);

    REQUIRE(html.find("class=\"json-preview\"") != std::string::npos);
    REQUIRE(html.find("markamp") != std::string::npos);
    REQUIRE(html.find("version") != std::string::npos);
    REQUIRE(html.find("1.0") != std::string::npos);
    REQUIRE(html.find("active") != std::string::npos);
    REQUIRE(html.find("true") != std::string::npos);
    REQUIRE(html.find("cpp") != std::string::npos);
}

TEST_CASE("JsonRenderer handles invalid JSON gracefully", "[rendering][json]")
{
    JsonRenderer renderer;
    std::string invalid_json =
        "{\"name\": \"markamp\", \"version\": 1.0, "; // Missing closing brace

    std::string html = renderer.render(invalid_json);

    REQUIRE(html.find("class=\"json-error\"") != std::string::npos);
    REQUIRE(html.find("JSON Parse Error:") != std::string::npos);
    REQUIRE(html.find(invalid_json) != std::string::npos); // Should contain raw content as fallback
}

TEST_CASE("JsonRenderer handles empty arrays and objects", "[rendering][json]")
{
    JsonRenderer renderer;
    std::string json = "{\"empty_obj\": {}, \"empty_arr\": []}";

    std::string html = renderer.render(json);

    REQUIRE(html.find("{}") != std::string::npos);
    REQUIRE(html.find("[]") != std::string::npos);
}

TEST_CASE("JsonRenderer handles nested structures", "[rendering][json]")
{
    JsonRenderer renderer;
    std::string json = "{\"user\": {\"id\": 42, \"roles\": [\"admin\", \"user\"]}}";

    std::string html = renderer.render(json);

    REQUIRE(html.find("user") != std::string::npos);
    REQUIRE(html.find("id") != std::string::npos);
    REQUIRE(html.find("42") != std::string::npos);
    REQUIRE(html.find("roles") != std::string::npos);
    REQUIRE(html.find("admin") != std::string::npos);
}

TEST_CASE("JsonRenderer escapes HTML in string values", "[rendering][json]")
{
    JsonRenderer renderer;
    std::string json = "{\"script\": \"<script>alert(1)</script>\"}";

    std::string html = renderer.render(json);

    REQUIRE(html.find("&lt;script&gt;alert(1)&lt;/script&gt;") != std::string::npos);
    REQUIRE(html.find("<script>") == std::string::npos);
}
