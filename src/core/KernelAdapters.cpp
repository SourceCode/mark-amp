/// @file KernelAdapters.cpp
/// @brief V4 Phase 30 – Multi-Language Kernel Adapters implementation.

#include "core/KernelAdapters.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// PythonAdapter
// ============================================================================

auto PythonAdapter::language() const -> std::string
{
    return "python";
}

auto PythonAdapter::detect_environments() const -> std::vector<std::string>
{
    // In a real implementation, this would scan the filesystem for Python interpreters.
    // For the testable layer, return standard locations.
    return {"/usr/bin/python3", "/usr/local/bin/python3"};
}

auto PythonAdapter::preprocess_code(const std::string& code) const -> std::string
{
    std::istringstream stream(code);
    std::string line;
    std::ostringstream result;
    bool first = true;

    while (std::getline(stream, line))
    {
        if (!first)
        {
            result << '\n';
        }
        first = false;

        // Handle %time magic.
        if (line.find("%time ") == 0)
        {
            std::string inner_code = line.substr(6);
            result << "import time as _t; _s=_t.time(); " << inner_code
                   << "; print(f'Wall time: {_t.time()-_s:.4f}s')";
            continue;
        }

        // Handle %pip magic.
        if (line.find("%pip ") == 0)
        {
            std::string pip_args = line.substr(5);
            result << "import subprocess; subprocess.check_call(['pip', "
                   << "'" << pip_args << "'"
                   << "])";
            continue;
        }

        // Handle %%timeit magic (cell magic — wraps entire remaining code).
        if (line == "%%timeit" || line.find("%%timeit ") == 0)
        {
            result << "import timeit; timeit.timeit(lambda: None, number=1000)";
            continue;
        }

        // Handle %matplotlib magic.
        if (line == "%matplotlib inline" || line.find("%matplotlib ") == 0)
        {
            result << "import matplotlib; matplotlib.use('agg')";
            continue;
        }

        result << line;
    }

    return result.str();
}

auto PythonAdapter::supported_magic_commands() const -> std::vector<std::string>
{
    return {"%time",
            "%timeit",
            "%%timeit",
            "%matplotlib",
            "%pip",
            "%cd",
            "%pwd",
            "%who",
            "%whos",
            "%reset"};
}

auto PythonAdapter::execute_magic(const std::string& magic, const std::string& args) const
    -> std::string
{
    if (magic == "%time")
    {
        return "import time as _t; _s=_t.time(); " + args +
               "; print(f'Wall time: {_t.time()-_s:.4f}s')";
    }
    if (magic == "%pip")
    {
        return "import subprocess; subprocess.check_call(['pip', '" + args + "'])";
    }
    if (magic == "%cd")
    {
        return "import os; os.chdir('" + args + "')";
    }
    if (magic == "%pwd")
    {
        return "import os; print(os.getcwd())";
    }
    return args;
}

auto PythonAdapter::default_kernel_spec() const -> KernelSpec
{
    KernelSpec spec;
    spec.name = "python3";
    spec.display_name = "Python 3";
    spec.language = "python";
    spec.executable = "python3";
    spec.argv = {"-m", "ipykernel_launcher", "-f", "{connection_file}"};
    return spec;
}

// ============================================================================
// RAdapter
// ============================================================================

auto RAdapter::language() const -> std::string
{
    return "r";
}

auto RAdapter::detect_environments() const -> std::vector<std::string>
{
    return {"/usr/bin/R", "/usr/local/bin/R"};
}

auto RAdapter::preprocess_code(const std::string& code) const -> std::string
{
    // R has minimal magic preprocessing.
    return code;
}

auto RAdapter::supported_magic_commands() const -> std::vector<std::string>
{
    return {};
}

auto RAdapter::execute_magic(const std::string& /*magic*/, const std::string& args) const
    -> std::string
{
    return args;
}

auto RAdapter::default_kernel_spec() const -> KernelSpec
{
    KernelSpec spec;
    spec.name = "ir";
    spec.display_name = "R";
    spec.language = "r";
    spec.executable = "R";
    spec.argv = {"--slave", "-e", "IRkernel::main()", "--args", "{connection_file}"};
    return spec;
}

// ============================================================================
// JuliaAdapter
// ============================================================================

auto JuliaAdapter::language() const -> std::string
{
    return "julia";
}

auto JuliaAdapter::detect_environments() const -> std::vector<std::string>
{
    return {"/usr/local/bin/julia"};
}

auto JuliaAdapter::preprocess_code(const std::string& code) const -> std::string
{
    return code;
}

auto JuliaAdapter::supported_magic_commands() const -> std::vector<std::string>
{
    return {};
}

auto JuliaAdapter::execute_magic(const std::string& /*magic*/, const std::string& args) const
    -> std::string
{
    return args;
}

auto JuliaAdapter::default_kernel_spec() const -> KernelSpec
{
    KernelSpec spec;
    spec.name = "julia-1.10";
    spec.display_name = "Julia 1.10";
    spec.language = "julia";
    spec.executable = "julia";
    spec.argv = {"-i", "--startup-file=yes", "{connection_file}"};
    return spec;
}

// ============================================================================
// NodeAdapter
// ============================================================================

auto NodeAdapter::language() const -> std::string
{
    return "javascript";
}

auto NodeAdapter::detect_environments() const -> std::vector<std::string>
{
    return {"/usr/local/bin/node", "/usr/bin/node"};
}

auto NodeAdapter::preprocess_code(const std::string& code) const -> std::string
{
    return code;
}

auto NodeAdapter::supported_magic_commands() const -> std::vector<std::string>
{
    return {};
}

auto NodeAdapter::execute_magic(const std::string& /*magic*/, const std::string& args) const
    -> std::string
{
    return args;
}

auto NodeAdapter::default_kernel_spec() const -> KernelSpec
{
    KernelSpec spec;
    spec.name = "javascript";
    spec.display_name = "Node.js";
    spec.language = "javascript";
    spec.executable = "node";
    return spec;
}

// ============================================================================
// SqlAdapter
// ============================================================================

auto SqlAdapter::language() const -> std::string
{
    return "sql";
}

auto SqlAdapter::detect_environments() const -> std::vector<std::string>
{
    return {"sqlite3", "psql"};
}

auto SqlAdapter::preprocess_code(const std::string& code) const -> std::string
{
    // Wrap SQL in a Python pandas execution helper.
    std::ostringstream wrapped;
    wrapped << "import pandas as pd\n"
            << "import sqlite3\n"
            << "conn = sqlite3.connect(':memory:')\n"
            << "result = pd.read_sql('''" << code << "''', conn)\n"
            << "display(result)";
    return wrapped.str();
}

auto SqlAdapter::supported_magic_commands() const -> std::vector<std::string>
{
    return {"%sql", "%%sql"};
}

auto SqlAdapter::execute_magic(const std::string& magic, const std::string& args) const
    -> std::string
{
    if (magic == "%sql" || magic == "%%sql")
    {
        return preprocess_code(args);
    }
    return args;
}

auto SqlAdapter::default_kernel_spec() const -> KernelSpec
{
    KernelSpec spec;
    spec.name = "sql";
    spec.display_name = "SQL";
    spec.language = "sql";
    spec.executable = "python3"; // Runs via Python kernel.
    spec.argv = {"-m", "ipykernel_launcher", "-f", "{connection_file}"};
    return spec;
}

// ============================================================================
// KernelAdapterRegistry
// ============================================================================

KernelAdapterRegistry::KernelAdapterRegistry()
{
    adapters_.push_back(std::make_unique<PythonAdapter>());
    adapters_.push_back(std::make_unique<RAdapter>());
    adapters_.push_back(std::make_unique<JuliaAdapter>());
    adapters_.push_back(std::make_unique<NodeAdapter>());
    adapters_.push_back(std::make_unique<SqlAdapter>());
}

auto KernelAdapterRegistry::register_adapter(std::unique_ptr<IKernelAdapter> adapter) -> void
{
    adapters_.push_back(std::move(adapter));
}

auto KernelAdapterRegistry::get_adapter(const std::string& lang) const -> const IKernelAdapter*
{
    for (const auto& adapter : adapters_)
    {
        if (adapter->language() == lang)
        {
            return adapter.get();
        }
    }
    return nullptr;
}

auto KernelAdapterRegistry::supported_languages() const -> std::vector<std::string>
{
    std::vector<std::string> languages;
    languages.reserve(adapters_.size());
    for (const auto& adapter : adapters_)
    {
        languages.push_back(adapter->language());
    }
    return languages;
}

auto KernelAdapterRegistry::all_kernel_specs() const -> std::vector<KernelSpec>
{
    std::vector<KernelSpec> specs;
    specs.reserve(adapters_.size());
    for (const auto& adapter : adapters_)
    {
        specs.push_back(adapter->default_kernel_spec());
    }
    return specs;
}

} // namespace markamp::core
