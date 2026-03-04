#include "core/LogLevel.h"
#include "core/OutputChannelService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── LogLevel tests ──

TEST_CASE("LogLevel string conversions", "[output_panel_v2]")
{
    REQUIRE(std::string(log_level_name(LogLevel::kTrace)) == "TRACE");
    REQUIRE(std::string(log_level_name(LogLevel::kDebug)) == "DEBUG");
    REQUIRE(std::string(log_level_name(LogLevel::kInfo)) == "INFO");
    REQUIRE(std::string(log_level_name(LogLevel::kWarning)) == "WARN");
    REQUIRE(std::string(log_level_name(LogLevel::kError)) == "ERROR");
    REQUIRE(std::string(log_level_name(LogLevel::kFatal)) == "FATAL");
}

TEST_CASE("LogLevel from string", "[output_panel_v2]")
{
    REQUIRE(log_level_from_string("TRACE") == LogLevel::kTrace);
    REQUIRE(log_level_from_string("DEBUG") == LogLevel::kDebug);
    REQUIRE(log_level_from_string("INFO") == LogLevel::kInfo);
    REQUIRE(log_level_from_string("WARN") == LogLevel::kWarning);
    REQUIRE(log_level_from_string("WARNING") == LogLevel::kWarning);
    REQUIRE(log_level_from_string("ERROR") == LogLevel::kError);
    REQUIRE(log_level_from_string("FATAL") == LogLevel::kFatal);
    REQUIRE(log_level_from_string("unknown") == LogLevel::kInfo); // Default
    REQUIRE(log_level_from_string(nullptr) == LogLevel::kInfo);   // Null safety
}

TEST_CASE("LogLevel comparison operators", "[output_panel_v2]")
{
    REQUIRE(LogLevel::kTrace < LogLevel::kDebug);
    REQUIRE(LogLevel::kDebug < LogLevel::kInfo);
    REQUIRE(LogLevel::kInfo < LogLevel::kWarning);
    REQUIRE(LogLevel::kWarning < LogLevel::kError);
    REQUIRE(LogLevel::kError < LogLevel::kFatal);

    REQUIRE(LogLevel::kFatal > LogLevel::kTrace);
    REQUIRE(LogLevel::kWarning >= LogLevel::kWarning);
    REQUIRE(LogLevel::kError >= LogLevel::kWarning);
    REQUIRE(LogLevel::kTrace <= LogLevel::kTrace);
    REQUIRE(LogLevel::kTrace <= LogLevel::kFatal);
}

// ── OutputChannel line-level tests ──

TEST_CASE("OutputChannel append_line with log level", "[output_panel_v2]")
{
    OutputChannel channel("test");

    channel.append_line("info message");
    channel.append_line("warning message", LogLevel::kWarning);
    channel.append_line("error message", LogLevel::kError);

    REQUIRE(channel.line_count() == 3);
    REQUIRE(channel.lines()[0].level == LogLevel::kInfo); // Default
    REQUIRE(channel.lines()[1].level == LogLevel::kWarning);
    REQUIRE(channel.lines()[2].level == LogLevel::kError);
}

TEST_CASE("OutputChannel lines_filtered", "[output_panel_v2]")
{
    OutputChannel channel("test");

    channel.append_line("trace msg", LogLevel::kTrace);
    channel.append_line("debug msg", LogLevel::kDebug);
    channel.append_line("info msg", LogLevel::kInfo);
    channel.append_line("warning msg", LogLevel::kWarning);
    channel.append_line("error msg", LogLevel::kError);

    auto all = channel.lines_filtered(LogLevel::kTrace);
    REQUIRE(all.size() == 5);

    auto warnings_up = channel.lines_filtered(LogLevel::kWarning);
    REQUIRE(warnings_up.size() == 2);

    auto errors_only = channel.lines_filtered(LogLevel::kError);
    REQUIRE(errors_only.size() == 1);
    REQUIRE(errors_only[0].text == "error msg");
}

TEST_CASE("OutputChannel clear resets lines", "[output_panel_v2]")
{
    OutputChannel channel("test");
    channel.append_line("line 1");
    channel.append_line("line 2");
    REQUIRE(channel.line_count() == 2);

    channel.clear();
    REQUIRE(channel.line_count() == 0);
    REQUIRE(channel.lines().empty());
    REQUIRE(channel.content().empty());
}

TEST_CASE("OutputChannel auto-reveal defaults to true", "[output_panel_v2]")
{
    OutputChannel channel("test");
    REQUIRE(channel.auto_reveal() == true);

    channel.set_auto_reveal(false);
    REQUIRE(channel.auto_reveal() == false);
}

TEST_CASE("OutputChannel unread count", "[output_panel_v2]")
{
    OutputChannel channel("test");
    REQUIRE(channel.unread_count() == 0);

    channel.append_line("msg 1");
    channel.append_line("msg 2");
    REQUIRE(channel.unread_count() == 2);

    channel.mark_read();
    REQUIRE(channel.unread_count() == 0);

    channel.append_line("msg 3");
    REQUIRE(channel.unread_count() == 1);
}

// ── OutputChannelService tests ──

TEST_CASE("OutputChannelService create_default_channels", "[output_panel_v2]")
{
    OutputChannelService service;
    service.create_default_channels();

    auto names = service.channel_names();
    REQUIRE(names.size() == 5);

    REQUIRE(service.get_channel("Build") != nullptr);
    REQUIRE(service.get_channel("Git") != nullptr);
    REQUIRE(service.get_channel("Tasks") != nullptr);
    REQUIRE(service.get_channel("Extension Host") != nullptr);
    REQUIRE(service.get_channel("Log") != nullptr);

    REQUIRE(service.active_channel() == "Log");
}

TEST_CASE("OutputChannelService ensure_channel", "[output_panel_v2]")
{
    OutputChannelService service;

    auto* channel1 = service.ensure_channel("TestChannel");
    REQUIRE(channel1 != nullptr);
    REQUIRE(channel1->name() == "TestChannel");

    // Ensure returns existing channel, not new one
    auto* channel2 = service.ensure_channel("TestChannel");
    REQUIRE(channel2 == channel1);
}

TEST_CASE("OutputLine timestamps are populated", "[output_panel_v2]")
{
    OutputChannel channel("test");
    auto before = std::chrono::system_clock::now();
    channel.append_line("test line", LogLevel::kInfo);
    auto after = std::chrono::system_clock::now();

    REQUIRE(channel.line_count() == 1);
    REQUIRE(channel.lines()[0].timestamp >= before);
    REQUIRE(channel.lines()[0].timestamp <= after);
}
