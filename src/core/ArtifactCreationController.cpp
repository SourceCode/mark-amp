/// @file ArtifactCreationController.cpp
/// @brief V25 P02-T01: Shell-level artifact creation controller implementation.
#include "core/ArtifactCreationController.h"

#include <algorithm>

namespace markamp::core
{

ArtifactCreationController::ArtifactCreationController(ArtifactCreationService& service)
    : service_(service)
{
}

auto ArtifactCreationController::create_from_surface(const std::string& surface,
                                                      ArtifactKind kind,
                                                      const std::string& name,
                                                      const std::string& language)
    -> ArtifactCreationResult
{
    ArtifactCreationRequest request;
    request.kind = kind;
    request.display_name = name;
    request.language_id = language;
    request.source = surface;

    auto result = service_.create(request);

    CreationRouteRecord record;
    record.source_surface = surface;
    record.kind = kind;
    record.artifact_id = result.id;
    record.success = result.success;
    history_.push_back(std::move(record));

    return result;
}

auto ArtifactCreationController::count_by_surface(const std::string& surface) const -> int
{
    return static_cast<int>(std::count_if(history_.begin(), history_.end(),
        [&](const CreationRouteRecord& r) { return r.source_surface == surface; }));
}

void ArtifactCreationController::record_bypass(const std::string& /*surface*/)
{
    ++bypass_count_;
}

} // namespace markamp::core
