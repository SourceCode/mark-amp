// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/BoardSerializerModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Schema versioning and migration check", "[serializer][version]")
{
    BoardSerializerModel model;
    model.set_schema_version(3);
    REQUIRE(model.schema_version() == 3);
    REQUIRE(model.needs_migration(1));
    REQUIRE(model.needs_migration(2));
    REQUIRE_FALSE(model.needs_migration(3));
}

TEST_CASE("Migration history", "[serializer][migration]")
{
    BoardSerializerModel model;
    model.add_migration({1, 2, "Add layers"});
    model.add_migration({2, 3, "Add connectors"});
    REQUIRE(model.migrations().size() == 2);
}

TEST_CASE("Partial-load recovery", "[serializer][recovery]")
{
    BoardSerializerModel model;
    model.set_load_status(LoadStatus::kPartialRecovery);
    model.set_recovered_count(5);
    REQUIRE(model.load_status() == LoadStatus::kPartialRecovery);
    REQUIRE(model.recovered_count() == 5);
}

TEST_CASE("Atomic save state machine", "[serializer][save]")
{
    BoardSerializerModel model;
    REQUIRE_FALSE(model.is_saving());
    model.begin_save();
    REQUIRE(model.is_saving());
    model.commit_save();
    REQUIRE_FALSE(model.is_saving());
    REQUIRE(model.save_succeeded());
}

TEST_CASE("Save rollback on failure", "[serializer][save]")
{
    BoardSerializerModel model;
    model.begin_save();
    model.rollback_save();
    REQUIRE_FALSE(model.is_saving());
    REQUIRE_FALSE(model.save_succeeded());
}

TEST_CASE("Diagnostics and error counting", "[serializer][diagnostics]")
{
    BoardSerializerModel model;
    model.add_diagnostic({"obj-1", "Missing field", true});
    model.add_diagnostic({"obj-2", "Invalid type", false});
    model.add_diagnostic({"obj-3", "Deprecated format", true});
    REQUIRE(model.diagnostics().size() == 3);
    REQUIRE(model.error_count() == 1);
    model.clear_diagnostics();
    REQUIRE(model.diagnostics().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
