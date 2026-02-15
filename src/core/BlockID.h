#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace markamp::core
{

// Thread-safe, globally-unique block ID generator compatible with SiYuan's
// ID format: YYYYMMDDHHmmss-xxxxxxx (22 characters total).
//
// The timestamp prefix enables:
//   - Chronological sorting via lexicographic comparison
//   - Creation time extraction without database lookup
//   - Collision avoidance across devices (timestamp + random)
//
// The random suffix uses charset: 0123456789abcdefghijklmnopqrstuvwxyz
// giving 36^7 = ~78 billion possible suffixes per second.
class BlockIDGenerator
{
public:
    // Generate a new block ID using the current system clock.
    [[nodiscard]] static auto generate() -> std::string;

    // Generate a new block ID with a specific timestamp.
    [[nodiscard]] static auto generate(std::chrono::system_clock::time_point timestamp)
        -> std::string;

    // Parse the creation timestamp from a block ID.
    [[nodiscard]] static auto parse_timestamp(const std::string& block_id)
        -> std::optional<std::chrono::system_clock::time_point>;

    // Validate that a string is a well-formed block ID.
    [[nodiscard]] static auto is_valid(const std::string& block_id) -> bool;

    // Extract the 14-character timestamp prefix from a block ID.
    [[nodiscard]] static auto timestamp_prefix(const std::string& block_id) -> std::string;

    // Extract the 7-character random suffix from a block ID.
    [[nodiscard]] static auto random_suffix(const std::string& block_id) -> std::string;

    // Get the current time formatted as a 14-character timestamp string.
    [[nodiscard]] static auto current_timestamp_string() -> std::string;

    // Format a time_point as a 14-character SiYuan timestamp string.
    [[nodiscard]] static auto format_timestamp(std::chrono::system_clock::time_point tp)
        -> std::string;

    // Parse a 14-character timestamp string back to a time_point.
    [[nodiscard]] static auto parse_timestamp_string(const std::string& ts)
        -> std::optional<std::chrono::system_clock::time_point>;

private:
    static constexpr int TIMESTAMP_LEN = 14;
    static constexpr int RANDOM_LEN = 7;
    static constexpr int TOTAL_LEN = TIMESTAMP_LEN + 1 + RANDOM_LEN; // 22

    static constexpr std::string_view CHARSET = "0123456789abcdefghijklmnopqrstuvwxyz";

    [[nodiscard]] static auto random_alphanumeric(int length) -> std::string;
};

} // namespace markamp::core
