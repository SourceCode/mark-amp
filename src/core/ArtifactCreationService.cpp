/// @file ArtifactCreationService.cpp
/// @brief V20 P01-T02: Artifact creation service implementation.

#include "ArtifactCreationService.h"

#include "ArtifactNamingPolicy.h"
#include "Config.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ArtifactCreationService::ArtifactCreationService(EventBus& bus, ArtifactRegistry& registry,
                                                   Config& cfg)
    : event_bus_(bus)
    , registry_(registry)
    , config_(cfg)
{
}

auto ArtifactCreationService::create(const ArtifactCreationRequest& request)
    -> ArtifactCreationResult
{
    ArtifactCreationResult result;

    // Build the artifact record
    ArtifactRecord record;
    record.id = ArtifactRegistry::generate_id();
    record.kind = request.kind;
    record.state = ArtifactLifecycleState::kUnsaved;
    record.language_id = request.language_id;
    record.source = request.source;

    // Set display name — use provided name or generate a default
    if (!request.display_name.empty())
    {
        record.display_name = request.display_name;
    }
    else
    {
        ArtifactNamingPolicy naming;
        record.display_name = naming.generate_name(request.kind);
    }

    // Set target directory if provided (but don't persist yet)
    if (request.target_directory.has_value())
    {
        // Record the intended directory for future first-save resolution
        // The artifact remains unsaved until an explicit save
    }

    // Set default language if not specified
    if (record.language_id.empty())
    {
        ArtifactNamingPolicy naming;
        record.language_id = naming.default_language(request.kind);
    }

    // Register in the artifact registry
    const auto id = registry_.register_artifact(std::move(record));

    // Publish creation event
    events::ArtifactCreatedEvent evt;
    evt.artifact_id = id.value;
    evt.artifact_kind = static_cast<int>(request.kind);
    evt.display_name = request.display_name.empty()
                           ? registry_.find(id)->display_name
                           : request.display_name;
    evt.source = request.source;
    event_bus_.publish(evt);

    // Update counters
    ++creation_count_;
    switch (request.kind)
    {
        case ArtifactKind::kTextFile:
            ++text_file_count_;
            break;
        case ArtifactKind::kNotebook:
            ++notebook_count_;
            break;
        case ArtifactKind::kCanvas:
            ++canvas_count_;
            break;
    }

    result.success = true;
    result.id = id;

    MARKAMP_LOG_INFO("Artifact created: {} kind={} source='{}'", id.value,
                     static_cast<int>(request.kind), request.source);
    return result;
}

auto ArtifactCreationService::create_text_file(const std::string& name,
                                                 const std::string& language,
                                                 const std::string& source)
    -> ArtifactCreationResult
{
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = name;
    req.language_id = language;
    req.source = source;
    return create(req);
}

auto ArtifactCreationService::create_notebook(const std::string& name, const std::string& source)
    -> ArtifactCreationResult
{
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kNotebook;
    req.display_name = name;
    req.language_id = "notebook";
    req.source = source;
    return create(req);
}

auto ArtifactCreationService::create_canvas(const std::string& name, const std::string& source)
    -> ArtifactCreationResult
{
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kCanvas;
    req.display_name = name;
    req.language_id = "canvas";
    req.source = source;
    return create(req);
}

auto ArtifactCreationService::creation_count_by_kind(ArtifactKind kind) const noexcept -> int
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return text_file_count_;
        case ArtifactKind::kNotebook:
            return notebook_count_;
        case ArtifactKind::kCanvas:
            return canvas_count_;
    }
    return 0;
}

} // namespace markamp::core
