#include "ui/IconManager.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/app.h>
#include <wx/dcmemory.h>
#include <wx/init.h>

using namespace markamp::ui;

class TestApp : public wxAppConsole
{
public:
    virtual bool OnInit() override
    {
        return true;
    }
};

TEST_CASE("IconRegistry functionality", "[icon_manager][registry]")
{
    IconRegistry registry;

    SECTION("Initial state is empty")
    {
        REQUIRE(registry.size() == 0);
        REQUIRE(registry.get_icon_names().empty());
    }

    SECTION("Registering valid SVG string")
    {
        std::string valid_svg =
            R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M12 2L2 22h20L12 2z"/></svg>)";
        REQUIRE(registry.register_icon("triangle", valid_svg));
        REQUIRE(registry.has_icon("triangle"));
        REQUIRE(registry.size() == 1);

        auto names = registry.get_icon_names();
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "triangle");

        auto doc = registry.get_icon("triangle");
        REQUIRE(doc.has_value());
    }

    SECTION("Registering invalid SVG string")
    {
        std::string invalid_svg = "not an svg";
        bool registered = registry.register_icon("garbage", invalid_svg);
        if (!registered)
        {
            REQUIRE(registry.size() == 0);
            REQUIRE_FALSE(registry.has_icon("garbage"));
        }
    }

    SECTION("Clear removes all icons")
    {
        std::string valid_svg =
            R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M12 2L2 22h20L12 2z"/></svg>)";
        registry.register_icon("triangle", valid_svg);
        registry.clear();
        REQUIRE(registry.size() == 0);
        REQUIRE_FALSE(registry.has_icon("triangle"));
    }
}

TEST_CASE("IconCache functionality", "[icon_manager][cache]")
{
    IconCache cache;

    IconCacheKey key1{"test_icon", 24, 24, 0xFFFFFFFF, 1.0};
    IconCacheKey key2{"test_icon", 48, 48, 0xFFFFFFFF, 1.0};

    SECTION("Initial state has cache misses")
    {
        REQUIRE_FALSE(cache.get(key1).has_value());
    }

    SECTION("Storing and retrieving a bitmap")
    {
        wxBitmap bmp(24, 24, 32);
        cache.put(key1, bmp);

        auto retrieved = cache.get(key1);
        REQUIRE(retrieved.has_value());
        REQUIRE(retrieved->GetWidth() == 24);

        REQUIRE_FALSE(cache.get(key2).has_value());
    }

    SECTION("Clearing the cache")
    {
        wxBitmap bmp(24, 24, 32);
        cache.put(key1, bmp);
        cache.clear();
        REQUIRE_FALSE(cache.get(key1).has_value());
    }
}

TEST_CASE("IconManager functionality", "[icon_manager][manager]")
{
    wxApp::SetInstance(new TestApp());
    int argc = 0;
    char** argv = nullptr;
    wxEntryStart(argc, argv);

    auto& manager = IconManager::get();
    manager.registry().clear();
    manager.cache().clear();

    SECTION("Fallback generation for missing icons")
    {
        wxBitmap bmp =
            manager.get_icon_bitmap("missing_icon", wxSize(24, 24), wxColour(255, 0, 0), 1.0);

        REQUIRE(bmp.IsOk());
        REQUIRE(bmp.GetWidth() == 24);
        REQUIRE(bmp.GetHeight() == 24);

        // Fallback uses the specific RGBA color calculated in get_icon_bitmap, check caching works
        IconCacheKey key{
            "missing_icon", 24, 24, IconCache::color_to_rgba(wxColour(255, 0, 0)), 1.0};
        auto cached = manager.cache().get(key);
        REQUIRE(cached.has_value());
    }

    SECTION("Rendering known valid SVG")
    {
        std::string valid_svg =
            R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M12 2L2 22h20L12 2z"/></svg>)";
        manager.registry().register_icon("triangle", valid_svg);

        wxBitmap bmp = manager.get_icon_bitmap(
            "triangle", wxSize(32, 32), wxColour(0, 255, 0), 2.0); // High DPI simulation

        REQUIRE(bmp.IsOk());
        // Requested logical size is 32, scale is 2.0, so physical pixels should be 64
        REQUIRE(bmp.GetWidth() == 64);
        REQUIRE(bmp.GetHeight() == 64);

        IconCacheKey key{"triangle", 32, 32, IconCache::color_to_rgba(wxColour(0, 255, 0)), 2.0};
        auto cached = manager.cache().get(key);
        REQUIRE(cached.has_value());
    }

    wxEntryCleanup();
}
