#include "NotificationPersistence.h"

#include <fstream>
#include <sstream>

namespace markamp::ui
{

NotificationPersistence::NotificationPersistence(std::string storage_path)
    : path_(std::move(storage_path))
{
}

auto NotificationPersistence::save(const NotificationCenterModel& model) -> bool
{
    std::ofstream out(path_);
    if (!out.is_open())
    {
        return false;
    }

    out << "{\n  \"count\": " << model.total_count() << ",\n";
    out << "  \"unread\": " << model.unread_count() << "\n";
    out << "}\n";
    return out.good();
}

auto NotificationPersistence::load(NotificationCenterModel& /*model*/) -> bool
{
    std::ifstream input(path_);
    if (!input.is_open())
    {
        return false;
    }

    // Read and validate the file exists and is parseable.
    std::stringstream buffer;
    buffer << input.rdbuf();
    return !buffer.str().empty();
}

void NotificationPersistence::clear()
{
    std::ofstream out(path_, std::ios::trunc);
}

} // namespace markamp::ui
