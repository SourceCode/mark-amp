#include "TerminalService.h"

namespace markamp::core
{

auto TerminalService::create_terminal(const std::string& /*name*/,
                                      const std::string& /*shell_path*/) -> std::optional<int>
{
    // P4 stub: terminal integration is N/A for a Markdown editor.
    return std::nullopt;
}

auto TerminalService::send_text(int /*terminal_id*/, const std::string& /*text*/) -> bool
{
    return false;
}

auto TerminalService::terminals() const -> const std::vector<Terminal>&
{
    return terminals_;
}

auto TerminalService::close_terminal(int /*terminal_id*/) -> bool
{
    return false;
}

} // namespace markamp::core
