// ============================================================================
// File: src/core/TemplateFunctions.h
// Phase 35: Template Engine — Built-in Sprig-compatible template functions
// ============================================================================
#pragma once

#include "TemplateTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class TemplateEngine;

// Registry of built-in template functions (Sprig-compatible subset).
class TemplateFunctionRegistry
{
public:
    static auto register_all(TemplateEngine& engine) -> void;

private:
    static auto fn_upper(const std::vector<std::string>& args) -> std::string;
    static auto fn_lower(const std::vector<std::string>& args) -> std::string;
    static auto fn_trim(const std::vector<std::string>& args) -> std::string;
    static auto fn_trimPrefix(const std::vector<std::string>& args) -> std::string;
    static auto fn_trimSuffix(const std::vector<std::string>& args) -> std::string;
    static auto fn_replace(const std::vector<std::string>& args) -> std::string;
    static auto fn_contains(const std::vector<std::string>& args) -> std::string;
    static auto fn_hasPrefix(const std::vector<std::string>& args) -> std::string;
    static auto fn_hasSuffix(const std::vector<std::string>& args) -> std::string;
    static auto fn_repeat(const std::vector<std::string>& args) -> std::string;
    static auto fn_nospace(const std::vector<std::string>& args) -> std::string;
    static auto fn_title(const std::vector<std::string>& args) -> std::string;
    static auto fn_quote(const std::vector<std::string>& args) -> std::string;
    static auto fn_squote(const std::vector<std::string>& args) -> std::string;
    static auto fn_default(const std::vector<std::string>& args) -> std::string;
    static auto fn_empty(const std::vector<std::string>& args) -> std::string;
    static auto fn_coalesce(const std::vector<std::string>& args) -> std::string;
    static auto fn_add(const std::vector<std::string>& args) -> std::string;
    static auto fn_sub(const std::vector<std::string>& args) -> std::string;
    static auto fn_mul(const std::vector<std::string>& args) -> std::string;
    static auto fn_div(const std::vector<std::string>& args) -> std::string;
    static auto fn_mod(const std::vector<std::string>& args) -> std::string;
    static auto fn_len(const std::vector<std::string>& args) -> std::string;
    static auto fn_now(const std::vector<std::string>& args) -> std::string;
    static auto fn_b64enc(const std::vector<std::string>& args) -> std::string;
    static auto fn_b64dec(const std::vector<std::string>& args) -> std::string;
    static auto fn_join(const std::vector<std::string>& args) -> std::string;
    static auto fn_toJson(const std::vector<std::string>& args) -> std::string;
};

} // namespace markamp::core
