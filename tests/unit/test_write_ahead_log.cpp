/// test_write_ahead_log.cpp — Unit tests

#include "core/WriteAheadLog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("WriteAheadLog: compiles", "[write_ahead_log]")
{
    static_assert(sizeof(WriteAheadLog) > 0);
}
