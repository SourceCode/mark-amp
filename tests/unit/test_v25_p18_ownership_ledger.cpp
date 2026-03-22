/// @file test_v25_p18_ownership_ledger.cpp
/// @brief V25 Phase 18: Duplicate ownership ledger tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P18: Track unresolved duplicates", "[v25][p18]")
{
    DuplicateOwnershipLedger ledger;
    DuplicateOwnership entry;
    entry.workflow_id = "file.save";
    entry.legacy_path = "LayoutManager::onSave";
    entry.new_path = "PersistenceCoordinator::save";
    ledger.add_entry(entry);
    REQUIRE(ledger.unresolved_count() == 1);
}

TEST_CASE("V25 P18: Retire legacy path", "[v25][p18]")
{
    DuplicateOwnershipLedger ledger;
    DuplicateOwnership entry;
    entry.workflow_id = "file.save";
    entry.new_path_validated = true;
    ledger.add_entry(entry);
    REQUIRE(ledger.retire_legacy("file.save"));
    REQUIRE(ledger.resolved_entries().size() == 1);
}

TEST_CASE("V25 P18: Entry count", "[v25][p18]")
{
    DuplicateOwnershipLedger ledger;
    REQUIRE(ledger.entry_count() == 0);
    DuplicateOwnership e; e.workflow_id = "test";
    ledger.add_entry(e);
    REQUIRE(ledger.entry_count() == 1);
}
