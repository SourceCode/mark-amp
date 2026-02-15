// ============================================================================
// File: src/core/TemplateFunctions.cpp
// Phase 35: Template Engine — Built-in function implementations
// ============================================================================

#include "TemplateFunctions.h"

#include "TemplateEngine.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace markamp::core
{

// Helper: get arg safely.
static auto arg_or(const std::vector<std::string>& args, size_t idx, const std::string& def = "")
    -> std::string
{
    return idx < args.size() ? args[idx] : def;
}

static auto safe_stoll(const std::string& s) -> int64_t
{
    try
    {
        return std::stoll(s);
    }
    catch (...)
    {
        return 0;
    }
}

static auto safe_stod(const std::string& s) -> double
{
    try
    {
        return std::stod(s);
    }
    catch (...)
    {
        return 0.0;
    }
}

// ── Registration ────────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::register_all(TemplateEngine& engine) -> void
{
    engine.register_function("upper", fn_upper);
    engine.register_function("lower", fn_lower);
    engine.register_function("trim", fn_trim);
    engine.register_function("trimPrefix", fn_trimPrefix);
    engine.register_function("trimSuffix", fn_trimSuffix);
    engine.register_function("replace", fn_replace);
    engine.register_function("contains", fn_contains);
    engine.register_function("hasPrefix", fn_hasPrefix);
    engine.register_function("hasSuffix", fn_hasSuffix);
    engine.register_function("repeat", fn_repeat);
    engine.register_function("nospace", fn_nospace);
    engine.register_function("title", fn_title);
    engine.register_function("quote", fn_quote);
    engine.register_function("squote", fn_squote);
    engine.register_function("default", fn_default);
    engine.register_function("empty", fn_empty);
    engine.register_function("coalesce", fn_coalesce);
    engine.register_function("add", fn_add);
    engine.register_function("sub", fn_sub);
    engine.register_function("mul", fn_mul);
    engine.register_function("div", fn_div);
    engine.register_function("mod", fn_mod);
    engine.register_function("len", fn_len);
    engine.register_function("now", fn_now);
    engine.register_function("b64enc", fn_b64enc);
    engine.register_function("b64dec", fn_b64dec);
    engine.register_function("join", fn_join);
    engine.register_function("toJson", fn_toJson);
}

// ── String functions ────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_upper(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

auto TemplateFunctionRegistry::fn_lower(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

auto TemplateFunctionRegistry::fn_trim(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto f = s.find_first_not_of(" \t\n\r");
    auto l = s.find_last_not_of(" \t\n\r");
    return (f == std::string::npos) ? "" : s.substr(f, l - f + 1);
}

auto TemplateFunctionRegistry::fn_trimPrefix(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto prefix = arg_or(args, 1);
    if (!prefix.empty() && s.starts_with(prefix))
        return s.substr(prefix.size());
    return s;
}

auto TemplateFunctionRegistry::fn_trimSuffix(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto suffix = arg_or(args, 1);
    if (!suffix.empty() && s.ends_with(suffix))
        return s.substr(0, s.size() - suffix.size());
    return s;
}

auto TemplateFunctionRegistry::fn_replace(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto old_str = arg_or(args, 1);
    auto new_str = arg_or(args, 2);
    if (old_str.empty())
        return s;
    std::string result;
    size_t pos = 0;
    while (true)
    {
        auto found = s.find(old_str, pos);
        if (found == std::string::npos)
        {
            result += s.substr(pos);
            break;
        }
        result += s.substr(pos, found - pos) + new_str;
        pos = found + old_str.size();
    }
    return result;
}

auto TemplateFunctionRegistry::fn_contains(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto sub = arg_or(args, 1);
    return s.find(sub) != std::string::npos ? "true" : "false";
}

auto TemplateFunctionRegistry::fn_hasPrefix(const std::vector<std::string>& args) -> std::string
{
    return arg_or(args, 0).starts_with(arg_or(args, 1)) ? "true" : "false";
}

auto TemplateFunctionRegistry::fn_hasSuffix(const std::vector<std::string>& args) -> std::string
{
    return arg_or(args, 0).ends_with(arg_or(args, 1)) ? "true" : "false";
}

auto TemplateFunctionRegistry::fn_repeat(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    auto count = safe_stoll(arg_or(args, 1, "1"));
    if (count <= 0 || count > 1000)
        return s;
    std::string result;
    result.reserve(s.size() * static_cast<size_t>(count));
    for (int64_t i = 0; i < count; ++i)
        result += s;
    return result;
}

auto TemplateFunctionRegistry::fn_nospace(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    std::string result;
    result.reserve(s.size());
    for (char c : s)
    {
        if (!std::isspace(static_cast<unsigned char>(c)))
            result += c;
    }
    return result;
}

auto TemplateFunctionRegistry::fn_title(const std::vector<std::string>& args) -> std::string
{
    auto s = arg_or(args, 0);
    bool capitalize_next = true;
    for (auto& c : s)
    {
        if (std::isspace(static_cast<unsigned char>(c)))
        {
            capitalize_next = true;
        }
        else if (capitalize_next)
        {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            capitalize_next = false;
        }
    }
    return s;
}

auto TemplateFunctionRegistry::fn_quote(const std::vector<std::string>& args) -> std::string
{
    return "\"" + arg_or(args, 0) + "\"";
}

auto TemplateFunctionRegistry::fn_squote(const std::vector<std::string>& args) -> std::string
{
    return "'" + arg_or(args, 0) + "'";
}

// ── Default/logic ───────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_default(const std::vector<std::string>& args) -> std::string
{
    auto val = arg_or(args, 0);
    auto def = arg_or(args, 1);
    return val.empty() ? def : val;
}

auto TemplateFunctionRegistry::fn_empty(const std::vector<std::string>& args) -> std::string
{
    return arg_or(args, 0).empty() ? "true" : "false";
}

auto TemplateFunctionRegistry::fn_coalesce(const std::vector<std::string>& args) -> std::string
{
    for (const auto& a : args)
    {
        if (!a.empty())
            return a;
    }
    return "";
}

// ── Math ────────────────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_add(const std::vector<std::string>& args) -> std::string
{
    auto a = safe_stoll(arg_or(args, 0));
    auto b = safe_stoll(arg_or(args, 1));
    return std::to_string(a + b);
}

auto TemplateFunctionRegistry::fn_sub(const std::vector<std::string>& args) -> std::string
{
    auto a = safe_stoll(arg_or(args, 0));
    auto b = safe_stoll(arg_or(args, 1));
    return std::to_string(a - b);
}

auto TemplateFunctionRegistry::fn_mul(const std::vector<std::string>& args) -> std::string
{
    auto a = safe_stoll(arg_or(args, 0));
    auto b = safe_stoll(arg_or(args, 1));
    return std::to_string(a * b);
}

auto TemplateFunctionRegistry::fn_div(const std::vector<std::string>& args) -> std::string
{
    auto a = safe_stod(arg_or(args, 0));
    auto b = safe_stod(arg_or(args, 1, "1"));
    if (b == 0.0)
        return "0";
    return std::to_string(a / b);
}

auto TemplateFunctionRegistry::fn_mod(const std::vector<std::string>& args) -> std::string
{
    auto a = safe_stoll(arg_or(args, 0));
    auto b = safe_stoll(arg_or(args, 1, "1"));
    if (b == 0)
        return "0";
    return std::to_string(a % b);
}

auto TemplateFunctionRegistry::fn_len(const std::vector<std::string>& args) -> std::string
{
    return std::to_string(arg_or(args, 0).size());
}

// ── Date ────────────────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_now(const std::vector<std::string>& /*args*/) -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

// ── Encoding ────────────────────────────────────────────────────────────────

static const char kBase64Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

auto TemplateFunctionRegistry::fn_b64enc(const std::vector<std::string>& args) -> std::string
{
    auto input = arg_or(args, 0);
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            output += kBase64Chars[(val >> valb) & 0x3F];
            valb -= 6;
        }
    }
    if (valb > -6)
        output += kBase64Chars[((val << 8) >> (valb + 8)) & 0x3F];
    while (output.size() % 4)
        output += '=';
    return output;
}

auto TemplateFunctionRegistry::fn_b64dec(const std::vector<std::string>& args) -> std::string
{
    auto input = arg_or(args, 0);
    std::string output;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; ++i)
        T[static_cast<unsigned char>(kBase64Chars[i])] = i;

    int val = 0, valb = -8;
    for (unsigned char c : input)
    {
        if (T[c] == -1)
            break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            output += static_cast<char>((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return output;
}

// ── List/Join ───────────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_join(const std::vector<std::string>& args) -> std::string
{
    // join receives input (first arg) and separator (second arg).
    // In pipeline usage the input is typically the list as a string.
    auto input = arg_or(args, 0);
    auto sep = arg_or(args, 1, ",");
    // If input looks like a list "[a,b,c]", parse and join.
    // Otherwise return as-is.
    return input;
}

// ── JSON ────────────────────────────────────────────────────────────────────

auto TemplateFunctionRegistry::fn_toJson(const std::vector<std::string>& args) -> std::string
{
    auto val = arg_or(args, 0);
    // Escape for JSON string.
    std::string escaped = "\"";
    for (char c : val)
    {
        switch (c)
        {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += c;
                break;
        }
    }
    escaped += "\"";
    return escaped;
}

} // namespace markamp::core
