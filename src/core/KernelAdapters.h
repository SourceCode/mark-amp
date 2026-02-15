/// @file KernelAdapters.h
/// @brief V4 Phase 30 – Multi-Language Kernel Adapters.
/// Language-specific kernel adapters with environment detection,
/// magic command preprocessing, and a registry.

#pragma once

#include "core/KernelManager.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

/// Base interface for language-specific kernel behavior.
class IKernelAdapter
{
public:
    virtual ~IKernelAdapter() = default;

    /// Language identifier (e.g., "python", "r", "julia").
    [[nodiscard]] virtual auto language() const -> std::string = 0;

    /// Detect available environments for this language.
    [[nodiscard]] virtual auto detect_environments() const -> std::vector<std::string> = 0;

    /// Preprocess code (handle magic commands, etc.).
    [[nodiscard]] virtual auto preprocess_code(const std::string& code) const -> std::string = 0;

    /// List supported magic commands.
    [[nodiscard]] virtual auto supported_magic_commands() const -> std::vector<std::string> = 0;

    /// Execute a magic command and return its result/replacement.
    [[nodiscard]] virtual auto execute_magic(const std::string& magic,
                                             const std::string& args) const -> std::string = 0;

    /// Default kernel spec for this language.
    [[nodiscard]] virtual auto default_kernel_spec() const -> KernelSpec = 0;
};

/// Python language adapter.
class PythonAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic, const std::string& args) const
        -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// R language adapter.
class RAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic, const std::string& args) const
        -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// Julia language adapter.
class JuliaAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic, const std::string& args) const
        -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// Node.js language adapter.
class NodeAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic, const std::string& args) const
        -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// SQL language adapter.
class SqlAdapter : public IKernelAdapter
{
public:
    [[nodiscard]] auto language() const -> std::string override;
    [[nodiscard]] auto detect_environments() const -> std::vector<std::string> override;
    [[nodiscard]] auto preprocess_code(const std::string& code) const -> std::string override;
    [[nodiscard]] auto supported_magic_commands() const -> std::vector<std::string> override;
    [[nodiscard]] auto execute_magic(const std::string& magic, const std::string& args) const
        -> std::string override;
    [[nodiscard]] auto default_kernel_spec() const -> KernelSpec override;
};

/// Registry of kernel adapters.
class KernelAdapterRegistry
{
public:
    /// Constructor registers all built-in adapters.
    KernelAdapterRegistry();

    /// Register a custom adapter.
    auto register_adapter(std::unique_ptr<IKernelAdapter> adapter) -> void;

    /// Get adapter for a language. Returns nullptr if not found.
    [[nodiscard]] auto get_adapter(const std::string& lang) const -> const IKernelAdapter*;

    /// List all supported languages.
    [[nodiscard]] auto supported_languages() const -> std::vector<std::string>;

    /// Collect all kernel specs from all adapters.
    [[nodiscard]] auto all_kernel_specs() const -> std::vector<KernelSpec>;

private:
    std::vector<std::unique_ptr<IKernelAdapter>> adapters_;
};

} // namespace markamp::core
