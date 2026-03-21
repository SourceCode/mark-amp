/// @file ArtifactRegistry.cpp
/// @brief V20 P01-T01: Unified artifact registry implementation.

#include "ArtifactRegistry.h"

#include "Events.h"
#include "Logger.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

ArtifactRegistry::ArtifactRegistry(EventBus& bus)
    : event_bus_(bus)
{
}

auto ArtifactRegistry::register_artifact(ArtifactRecord record) -> ArtifactId
{
    if (record.id.empty())
    {
        record.id = generate_id();
    }

    const auto id = record.id;
    const auto kind_int = static_cast<int>(record.kind);

    records_.emplace(id.value, std::move(record));

    // Publish registration event
    events::ArtifactRegisteredEvent evt;
    evt.artifact_id = id.value;
    evt.artifact_kind = kind_int;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Artifact registered: {} (kind={})", id.value, kind_int);
    return id;
}

auto ArtifactRegistry::unregister(const ArtifactId& id) -> bool
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        MARKAMP_LOG_WARN("Artifact not found for unregister: {}", id.value);
        return false;
    }

    records_.erase(iter);

    // Clear active if it was the unregistered artifact
    if (active_id_ == id)
    {
        active_id_ = ArtifactId{};
    }

    events::ArtifactUnregisteredEvent evt;
    evt.artifact_id = id.value;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Artifact unregistered: {}", id.value);
    return true;
}

auto ArtifactRegistry::find(const ArtifactId& id) const -> const ArtifactRecord*
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto ArtifactRegistry::find_by_path(const std::string& path) const -> const ArtifactRecord*
{
    if (path.empty())
    {
        return nullptr;
    }

    for (const auto& [key, record] : records_)
    {
        if (record.file_path.has_value() && record.file_path.value() == path)
        {
            return &record;
        }
    }
    return nullptr;
}

auto ArtifactRegistry::update(const ArtifactId& id, ArtifactRecord record) -> bool
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        MARKAMP_LOG_WARN("Artifact not found for update: {}", id.value);
        return false;
    }

    record.id = id;  // Preserve original ID
    iter->second = std::move(record);

    MARKAMP_LOG_DEBUG("Artifact updated: {}", id.value);
    return true;
}

auto ArtifactRegistry::set_state(const ArtifactId& id, ArtifactLifecycleState new_state) -> bool
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        MARKAMP_LOG_WARN("Artifact not found for state change: {}", id.value);
        return false;
    }

    const auto old_state = iter->second.state;
    if (old_state == new_state)
    {
        return true;  // No-op
    }

    iter->second.state = new_state;
    iter->second.modified_at = std::chrono::steady_clock::now();

    events::ArtifactStateChangedEvent evt;
    evt.artifact_id = id.value;
    evt.old_state = static_cast<int>(old_state);
    evt.new_state = static_cast<int>(new_state);
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Artifact state changed: {} ({} -> {})", id.value,
                      static_cast<int>(old_state), static_cast<int>(new_state));
    return true;
}

auto ArtifactRegistry::promote_to_saved(const ArtifactId& id, const std::string& path) -> bool
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        MARKAMP_LOG_WARN("Artifact not found for promotion: {}", id.value);
        return false;
    }

    iter->second.file_path = path;
    iter->second.state = ArtifactLifecycleState::kSaved;
    iter->second.modified_at = std::chrono::steady_clock::now();

    events::ArtifactPromotedEvent evt;
    evt.artifact_id = id.value;
    evt.file_path = path;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Artifact promoted to saved: {} -> {}", id.value, path);
    return true;
}

auto ArtifactRegistry::rename(const ArtifactId& id, const std::string& new_name) -> bool
{
    auto iter = records_.find(id.value);
    if (iter == records_.end())
    {
        MARKAMP_LOG_WARN("Artifact not found for rename: {}", id.value);
        return false;
    }

    const auto old_name = iter->second.display_name;
    iter->second.display_name = new_name;
    iter->second.modified_at = std::chrono::steady_clock::now();

    events::ArtifactRenamedEvent evt;
    evt.artifact_id = id.value;
    evt.old_name = old_name;
    evt.new_name = new_name;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Artifact renamed: {} ('{}' -> '{}')", id.value, old_name, new_name);
    return true;
}

auto ArtifactRegistry::all_artifacts() const -> std::vector<ArtifactRecord>
{
    std::vector<ArtifactRecord> result;
    result.reserve(records_.size());
    for (const auto& [key, record] : records_)
    {
        result.push_back(record);
    }
    return result;
}

auto ArtifactRegistry::artifacts_by_kind(ArtifactKind kind) const -> std::vector<ArtifactRecord>
{
    std::vector<ArtifactRecord> result;
    for (const auto& [key, record] : records_)
    {
        if (record.kind == kind)
        {
            result.push_back(record);
        }
    }
    return result;
}

auto ArtifactRegistry::artifacts_by_state(ArtifactLifecycleState state) const
    -> std::vector<ArtifactRecord>
{
    std::vector<ArtifactRecord> result;
    for (const auto& [key, record] : records_)
    {
        if (record.state == state)
        {
            result.push_back(record);
        }
    }
    return result;
}

void ArtifactRegistry::set_active_artifact(const ArtifactId& id)
{
    const auto previous = active_id_;
    active_id_ = id;

    events::ArtifactActivatedEvent evt;
    evt.artifact_id = id.value;
    evt.previous_artifact_id = previous.value;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Active artifact changed: {} -> {}", previous.value, id.value);
}

auto ArtifactRegistry::count_by_kind(ArtifactKind kind) const -> int
{
    int n = 0;
    for (const auto& [key, record] : records_)
    {
        if (record.kind == kind)
        {
            ++n;
        }
    }
    return n;
}

auto ArtifactRegistry::count_by_state(ArtifactLifecycleState state) const -> int
{
    int n = 0;
    for (const auto& [key, record] : records_)
    {
        if (record.state == state)
        {
            ++n;
        }
    }
    return n;
}

auto ArtifactRegistry::generate_id() -> ArtifactId
{
    // Deterministic counter-based ID (sufficient for single-process use).
    // A production build could substitute a proper UUID generator.
    static int global_counter = 0;
    ++global_counter;

    std::ostringstream oss;
    oss << "artifact-" << global_counter;
    return ArtifactId{oss.str()};
}

void ArtifactRegistry::clear()
{
    records_.clear();
    active_id_ = ArtifactId{};
    MARKAMP_LOG_DEBUG("Artifact registry cleared");
}

} // namespace markamp::core
