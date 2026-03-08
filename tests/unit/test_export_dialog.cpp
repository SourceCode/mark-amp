/// test_export_dialog.cpp — Unit tests for ExportDialog
#include "core/ExportDialog.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ExportDialog: type compiles", "[export_dialog]")
{
    static_assert(sizeof(ExportDialog) > 0);
}
