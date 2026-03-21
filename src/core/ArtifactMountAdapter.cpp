/// @file ArtifactMountAdapter.cpp
/// @brief V20 P01-T04: Mount adapter service implementation.

#include "ArtifactMountAdapter.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ArtifactMountService::ArtifactMountService(EventBus& bus, ArtifactRegistry& registry)
    : event_bus_(bus)
    , registry_(registry)
{
}

void ArtifactMountService::register_adapter(ArtifactKind kind, IArtifactMountAdapter* adapter)
{
    adapters_[static_cast<int>(kind)] = adapter;
    MARKAMP_LOG_DEBUG("Mount adapter registered for kind={}", static_cast<int>(kind));
}

auto ArtifactMountService::mount_artifact(const ArtifactId& id) -> ArtifactMountResult
{
    const auto* record = registry_.find(id);
    if (record == nullptr)
    {
        MARKAMP_LOG_WARN("Cannot mount: artifact not found: {}", id.value);
        return {false, "Artifact not found"};
    }

    const auto kind_int = static_cast<int>(record->kind);
    auto adapter_iter = adapters_.find(kind_int);
    if (adapter_iter == adapters_.end() || adapter_iter->second == nullptr)
    {
        MARKAMP_LOG_WARN("Cannot mount: no adapter for kind={}", kind_int);
        return {false, "No mount adapter registered for this artifact kind"};
    }

    ArtifactMountRequest request;
    request.artifact_id = id;
    request.target_surface = kind_int;

    auto result = adapter_iter->second->mount(request, *record);

    if (result.success)
    {
        mounted_artifacts_[id.value] = true;

        events::ArtifactMountedEvent evt;
        evt.artifact_id = id.value;
        evt.surface_kind = kind_int;
        event_bus_.publish(evt);

        MARKAMP_LOG_DEBUG("Artifact mounted: {} (kind={})", id.value, kind_int);
    }

    return result;
}

auto ArtifactMountService::unmount_artifact(const ArtifactId& id) -> bool
{
    auto mounted_iter = mounted_artifacts_.find(id.value);
    if (mounted_iter == mounted_artifacts_.end())
    {
        MARKAMP_LOG_WARN("Cannot unmount: artifact not mounted: {}", id.value);
        return false;
    }

    const auto* record = registry_.find(id);
    if (record == nullptr)
    {
        // Artifact was removed from registry but still mounted — clean up
        mounted_artifacts_.erase(mounted_iter);
        return true;
    }

    const auto kind_int = static_cast<int>(record->kind);
    auto adapter_iter = adapters_.find(kind_int);
    if (adapter_iter != adapters_.end() && adapter_iter->second != nullptr)
    {
        (void)adapter_iter->second->unmount(id);
    }

    mounted_artifacts_.erase(mounted_iter);

    events::ArtifactUnmountedEvent evt;
    evt.artifact_id = id.value;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Artifact unmounted: {}", id.value);
    return true;
}

auto ArtifactMountService::is_mounted(const ArtifactId& id) const -> bool
{
    return mounted_artifacts_.contains(id.value);
}

auto ArtifactMountService::has_adapter(ArtifactKind kind) const -> bool
{
    return adapters_.contains(static_cast<int>(kind));
}

} // namespace markamp::core
