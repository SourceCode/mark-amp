#include "TaskRunnerService.h"

namespace markamp::core
{

auto TaskRunnerService::register_task_provider(
    const std::string& /*type*/, std::function<std::vector<TaskDefinition>()> /*provider*/) -> bool
{
    // P4 stub: task running is N/A for a Markdown editor.
    return false;
}

auto TaskRunnerService::execute_task(const std::string& /*task_name*/) -> bool
{
    return false;
}

auto TaskRunnerService::get_tasks() const -> std::vector<TaskDefinition>
{
    return {};
}

} // namespace markamp::core
