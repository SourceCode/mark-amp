#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Interface for Mermaid diagram rendering.
/// Must use an embedded JS engine (QuickJS) — NOT an external CLI tool,
/// as App Sandbox prohibits spawning child processes.
class IMermaidRenderer
{
public:
    virtual ~IMermaidRenderer() = default;
    [[nodiscard]] virtual auto render(std::string_view mermaid_source)
        -> std::expected<std::string, std::string> = 0;
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

} // namespace markamp::core
