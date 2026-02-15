#include "core/BlockID.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>

namespace markamp::core
{

auto BlockIDGenerator::generate() -> std::string
{
    return generate(std::chrono::system_clock::now());
}

auto BlockIDGenerator::generate(std::chrono::system_clock::time_point timestamp) -> std::string
{
    std::string result;
    result.reserve(TOTAL_LEN);
    result += format_timestamp(timestamp);
    result += '-';
    result += random_alphanumeric(RANDOM_LEN);
    return result;
}

auto BlockIDGenerator::parse_timestamp(const std::string& block_id)
    -> std::optional<std::chrono::system_clock::time_point>
{
    if (!is_valid(block_id))
    {
        return std::nullopt;
    }
    return parse_timestamp_string(block_id.substr(0, TIMESTAMP_LEN));
}

auto BlockIDGenerator::is_valid(const std::string& block_id) -> bool
{
    if (static_cast<int>(block_id.size()) != TOTAL_LEN)
    {
        return false;
    }
    if (block_id[TIMESTAMP_LEN] != '-')
    {
        return false;
    }
    // Check timestamp: first 14 chars must be digits
    for (std::size_t i = 0; i < static_cast<std::size_t>(TIMESTAMP_LEN); ++i)
    {
        if (block_id[i] < '0' || block_id[i] > '9')
        {
            return false;
        }
    }
    // Check suffix: last 7 chars must be in CHARSET (0-9, a-z)
    for (std::size_t i = static_cast<std::size_t>(TIMESTAMP_LEN + 1);
         i < static_cast<std::size_t>(TOTAL_LEN);
         ++i)
    {
        char ch = block_id[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z')))
        {
            return false;
        }
    }
    return true;
}

auto BlockIDGenerator::timestamp_prefix(const std::string& block_id) -> std::string
{
    if (!is_valid(block_id))
    {
        return "";
    }
    return block_id.substr(0, TIMESTAMP_LEN);
}

auto BlockIDGenerator::random_suffix(const std::string& block_id) -> std::string
{
    if (!is_valid(block_id))
    {
        return "";
    }
    return block_id.substr(TIMESTAMP_LEN + 1, RANDOM_LEN);
}

auto BlockIDGenerator::current_timestamp_string() -> std::string
{
    return format_timestamp(std::chrono::system_clock::now());
}

auto BlockIDGenerator::format_timestamp(std::chrono::system_clock::time_point tp) -> std::string
{
    auto time_t_val = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_val{};
#if defined(_WIN32)
    localtime_s(&tm_val, &time_t_val);
#else
    localtime_r(&time_t_val, &tm_val);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_val, "%Y%m%d%H%M%S");
    return oss.str();
}

auto BlockIDGenerator::parse_timestamp_string(const std::string& ts)
    -> std::optional<std::chrono::system_clock::time_point>
{
    if (ts.size() != 14)
    {
        return std::nullopt;
    }
    for (char ch : ts)
    {
        if (ch < '0' || ch > '9')
        {
            return std::nullopt;
        }
    }

    std::tm tm_val{};
    std::istringstream iss(ts);
    iss >> std::get_time(&tm_val, "%Y%m%d%H%M%S");
    if (iss.fail())
    {
        return std::nullopt;
    }
    tm_val.tm_isdst = -1; // Let mktime determine DST
    auto time_t_val = std::mktime(&tm_val);
    if (time_t_val == static_cast<std::time_t>(-1))
    {
        return std::nullopt;
    }
    return std::chrono::system_clock::from_time_t(time_t_val);
}

auto BlockIDGenerator::random_alphanumeric(int length) -> std::string
{
    // Thread-local RNG — no mutex needed
    thread_local static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist(0, CHARSET.size() - 1);

    std::string result;
    result.reserve(static_cast<std::size_t>(length));
    for (int i = 0; i < length; ++i)
    {
        result += CHARSET[dist(rng)];
    }
    return result;
}

} // namespace markamp::core
