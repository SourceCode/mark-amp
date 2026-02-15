#include "core/BlockID.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Notebook.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <thread>
#include <vector>

using namespace markamp::core;

namespace
{

// RAII temp directory for test isolation
struct TempDir
{
    std::filesystem::path path;

    TempDir()
    {
        auto base = std::filesystem::temp_directory_path() / "markamp_test";
        auto id = BlockIDGenerator::generate();
        path = base / id;
        std::filesystem::create_directories(path);
    }

    ~TempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
    }
};

auto make_test_config(const std::filesystem::path& data_dir) -> Config
{
    Config config;
    config.set("knowledgebase.data_dir", data_dir.string());
    return config;
}

} // anonymous namespace

TEST_CASE("Create notebook", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto result = mgr.create_notebook("Test");
    REQUIRE(result.has_value());

    auto id = result.value();
    CHECK(BlockIDGenerator::is_valid(id));

    // Verify directory exists
    CHECK(std::filesystem::exists(tmp.path / id));
    CHECK(std::filesystem::exists(tmp.path / id / ".markamp" / "conf.json"));

    // Verify list returns it
    auto notebooks = mgr.list_notebooks();
    REQUIRE(notebooks.size() == 1);
    CHECK(notebooks[0].name == "Test");
    CHECK(notebooks[0].id == id);
}

TEST_CASE("Rename notebook", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id = mgr.create_notebook("Original").value();

    auto result = mgr.rename_notebook(id, "Renamed");
    CHECK(result.has_value());

    auto nb = mgr.get_notebook(id);
    REQUIRE(nb.has_value());
    CHECK(nb->name == "Renamed");
}

TEST_CASE("Remove notebook", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id = mgr.create_notebook("ToDelete").value();

    auto result = mgr.remove_notebook(id);
    CHECK(result.has_value());

    CHECK_FALSE(mgr.get_notebook(id).has_value());
    CHECK(mgr.list_notebooks().empty());

    // Original directory should be gone
    CHECK_FALSE(std::filesystem::exists(tmp.path / id));

    // History should contain the moved notebook
    auto history = tmp.path / ".markamp" / "history";
    CHECK(std::filesystem::exists(history));
    int count = 0;
    for (auto& entry : std::filesystem::directory_iterator(history))
    {
        if (entry.is_directory())
        {
            ++count;
        }
    }
    CHECK(count == 1);
}

TEST_CASE("Open and close notebook", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id = mgr.create_notebook("Toggle").value();

    // Default state is open
    auto nb = mgr.get_notebook(id);
    REQUIRE(nb.has_value());
    CHECK(nb->is_open());

    // Close it
    auto close_result = mgr.close_notebook(id);
    CHECK(close_result.has_value());
    nb = mgr.get_notebook(id);
    CHECK_FALSE(nb->is_open());

    // Open it
    auto open_result = mgr.open_notebook(id);
    CHECK(open_result.has_value());
    nb = mgr.get_notebook(id);
    CHECK(nb->is_open());
}

TEST_CASE("List notebooks sorted", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id_c = mgr.create_notebook("C").value();
    auto id_a = mgr.create_notebook("A").value();
    auto id_b = mgr.create_notebook("B").value();

    // Set sort orders: A=0, B=1, C=2
    mgr.sort_notebooks({id_a, id_b, id_c});

    auto notebooks = mgr.list_notebooks();
    REQUIRE(notebooks.size() == 3);
    CHECK(notebooks[0].name == "A");
    CHECK(notebooks[1].name == "B");
    CHECK(notebooks[2].name == "C");
}

TEST_CASE("Sort notebooks by ID list", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id1 = mgr.create_notebook("First").value();
    auto id2 = mgr.create_notebook("Second").value();
    auto id3 = mgr.create_notebook("Third").value();

    mgr.sort_notebooks({id3, id1, id2});

    auto notebooks = mgr.list_notebooks();
    CHECK(notebooks[0].id == id3);
    CHECK(notebooks[1].id == id1);
    CHECK(notebooks[2].id == id2);
}

TEST_CASE("Notebook conf persistence", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);

    std::string id;
    {
        NotebookManager mgr(bus, config);
        id = mgr.create_notebook("Persist").value();

        NotebookConf conf;
        conf.name = "Persist";
        conf.daily_note_save_path = "/daily/";
        conf.create_doc_name_template = "{{.title}}";
        mgr.set_notebook_conf(id, conf);
    }

    // New manager, reload from disk
    NotebookManager mgr2(bus, config);
    mgr2.load_notebooks();

    auto nb = mgr2.get_notebook(id);
    REQUIRE(nb.has_value());
    CHECK(nb->name == "Persist");
    CHECK(nb->conf.daily_note_save_path == "/daily/");
    CHECK(nb->conf.create_doc_name_template == "{{.title}}");
}

TEST_CASE("Duplicate names allowed", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id1 = mgr.create_notebook("Duplicate").value();
    auto id2 = mgr.create_notebook("Duplicate").value();

    CHECK(id1 != id2);
    CHECK(mgr.list_notebooks().size() == 2);
}

TEST_CASE("Invalid ID operations", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    CHECK_FALSE(mgr.rename_notebook("nonexistent", "x").has_value());
    CHECK_FALSE(mgr.remove_notebook("nonexistent").has_value());
    CHECK_FALSE(mgr.open_notebook("nonexistent").has_value());
    CHECK_FALSE(mgr.close_notebook("nonexistent").has_value());
    CHECK_FALSE(mgr.get_notebook("nonexistent").has_value());
}

TEST_CASE("Concurrent read/write", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    std::atomic<bool> stop{false};
    std::atomic<int> created{0};

    // Reader threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i)
    {
        threads.emplace_back(
            [&]()
            {
                while (!stop.load())
                {
                    auto list = mgr.list_notebooks();
                    (void)list;
                }
            });
    }

    // Writer thread
    threads.emplace_back(
        [&]()
        {
            for (int i = 0; i < 10 && !stop.load(); ++i)
            {
                auto result = mgr.create_notebook("Thread-" + std::to_string(i));
                if (result.has_value())
                {
                    created.fetch_add(1);
                }
            }
        });

    // Let it run briefly
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop.store(true);

    for (auto& t : threads)
    {
        t.join();
    }

    CHECK(mgr.list_notebooks().size() == static_cast<std::size_t>(created.load()));
}

TEST_CASE("Set notebook icon", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id = mgr.create_notebook("WithIcon").value();
    mgr.set_notebook_icon(id, "1f4d4");

    auto nb = mgr.get_notebook(id);
    REQUIRE(nb.has_value());
    CHECK(nb->icon == "1f4d4");
}

TEST_CASE("Get open notebooks filter", "[notebook][phase03]")
{
    TempDir tmp;
    EventBus bus;
    auto config = make_test_config(tmp.path);
    NotebookManager mgr(bus, config);

    auto id1 = mgr.create_notebook("Open1").value();
    auto id2 = mgr.create_notebook("Closed").value();
    auto id3 = mgr.create_notebook("Open2").value();

    mgr.close_notebook(id2);

    auto open = mgr.get_open_notebooks();
    CHECK(open.size() == 2);
    for (const auto& nb : open)
    {
        CHECK(nb.is_open());
        CHECK(nb.id != id2);
    }
}
