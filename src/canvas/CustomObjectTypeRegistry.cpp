// ============================================================================
// File: src/canvas/CustomObjectTypeRegistry.cpp
// Phase 14: Canvas Extensibility — custom object type registration
// ============================================================================
#include "canvas/CustomObjectTypeRegistry.h"

#include "core/Events.h"

#include <algorithm>

namespace markamp::canvas
{

CustomObjectTypeRegistry::CustomObjectTypeRegistry(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Registration ──────────────────────────────────────────────────

auto CustomObjectTypeRegistry::register_type(const CustomObjectTypeDescriptor& descriptor)
    -> TypeRegistrationResult
{
    if (descriptor.type_id.empty())
    {
        return {false, "Type ID must not be empty"};
    }

    if (types_.contains(descriptor.type_id))
    {
        return {false, "Type already registered: " + descriptor.type_id};
    }

    if (!descriptor.factory)
    {
        return {false, "Factory callback must not be null"};
    }

    types_.emplace(descriptor.type_id, descriptor);

    core::events::CanvasAppRegisteredEvent evt;
    evt.app_id = descriptor.type_id;
    evt.app_name = descriptor.display_name;
    evt.extension_id = descriptor.extension_id;
    event_bus_.publish(evt);

    return {true, ""};
}

auto CustomObjectTypeRegistry::unregister_type(const std::string& type_id) -> bool
{
    return types_.erase(type_id) > 0;
}

auto CustomObjectTypeRegistry::unregister_extension(const std::string& extension_id) -> size_t
{
    size_t removed = 0;
    for (auto iter = types_.begin(); iter != types_.end();)
    {
        if (iter->second.extension_id == extension_id)
        {
            iter = types_.erase(iter);
            ++removed;
        }
        else
        {
            ++iter;
        }
    }
    return removed;
}

// ── Query ─────────────────────────────────────────────────────────

auto CustomObjectTypeRegistry::find_type(const std::string& type_id) const
    -> const CustomObjectTypeDescriptor*
{
    auto iter = types_.find(type_id);
    if (iter != types_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto CustomObjectTypeRegistry::registered_types() const
    -> const std::unordered_map<std::string, CustomObjectTypeDescriptor>&
{
    return types_;
}

auto CustomObjectTypeRegistry::types_for_extension(const std::string& extension_id) const
    -> std::vector<const CustomObjectTypeDescriptor*>
{
    std::vector<const CustomObjectTypeDescriptor*> result;
    for (const auto& [tid, desc] : types_)
    {
        if (desc.extension_id == extension_id)
        {
            result.push_back(&desc);
        }
    }
    return result;
}

auto CustomObjectTypeRegistry::types_in_category(const std::string& category) const
    -> std::vector<const CustomObjectTypeDescriptor*>
{
    std::vector<const CustomObjectTypeDescriptor*> result;
    for (const auto& [tid, desc] : types_)
    {
        if (desc.category == category)
        {
            result.push_back(&desc);
        }
    }
    return result;
}

auto CustomObjectTypeRegistry::is_registered(const std::string& type_id) const -> bool
{
    return types_.contains(type_id);
}

auto CustomObjectTypeRegistry::type_count() const -> size_t
{
    return types_.size();
}

// ── Factory ───────────────────────────────────────────────────────

auto CustomObjectTypeRegistry::create_object(const std::string& type_id, Point2D position) const
    -> std::unique_ptr<CanvasObject>
{
    const auto* descriptor = find_type(type_id);
    if (descriptor == nullptr || !descriptor->factory)
    {
        return nullptr;
    }

    return descriptor->factory(position, descriptor->default_size);
}

auto CustomObjectTypeRegistry::create_object(const std::string& type_id,
                                             Point2D position,
                                             Size2D size) const -> std::unique_ptr<CanvasObject>
{
    const auto* descriptor = find_type(type_id);
    if (descriptor == nullptr || !descriptor->factory)
    {
        return nullptr;
    }

    return descriptor->factory(position, size);
}

// ── Cleanup ───────────────────────────────────────────────────────

auto CustomObjectTypeRegistry::clear() -> void
{
    types_.clear();
}

} // namespace markamp::canvas
