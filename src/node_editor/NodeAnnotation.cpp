#include "NodeAnnotation.h"

namespace markamp::node_editor
{

auto NodeAnnotation::generate_id() -> FrameId
{
    return FrameId(next_id_++);
}

auto NodeAnnotation::add_frame(Rect bounds, const std::string& label) -> FrameId
{
    const auto kAnnotationId = generate_id();
    Annotation annotation;
    annotation.frame_id = kAnnotationId;
    annotation.type = AnnotationType::Frame;
    annotation.bounds = bounds;
    annotation.label = label;
    annotations_[kAnnotationId] = annotation;
    return kAnnotationId;
}

auto NodeAnnotation::add_comment(Vec2 position, const std::string& text) -> FrameId
{
    const auto kAnnotationId = generate_id();
    Annotation annotation;
    annotation.frame_id = kAnnotationId;
    annotation.type = AnnotationType::Comment;
    annotation.bounds = {position.x, position.y, 200.0F, 60.0F};
    annotation.label = text;
    annotations_[kAnnotationId] = annotation;
    return kAnnotationId;
}

auto NodeAnnotation::add_text_label(Vec2 position, const std::string& text) -> FrameId
{
    const auto kAnnotationId = generate_id();
    Annotation annotation;
    annotation.frame_id = kAnnotationId;
    annotation.type = AnnotationType::TextLabel;
    annotation.bounds = {position.x, position.y, 120.0F, 24.0F};
    annotation.label = text;
    annotations_[kAnnotationId] = annotation;
    return kAnnotationId;
}

auto NodeAnnotation::remove(FrameId annotation_id) -> bool
{
    return annotations_.erase(annotation_id) > 0;
}

auto NodeAnnotation::find(FrameId annotation_id) const -> const Annotation*
{
    const auto iter = annotations_.find(annotation_id);
    return (iter != annotations_.end()) ? &iter->second : nullptr;
}

auto NodeAnnotation::find_mut(FrameId annotation_id) -> Annotation*
{
    auto iter = annotations_.find(annotation_id);
    return (iter != annotations_.end()) ? &iter->second : nullptr;
}

auto NodeAnnotation::count() const -> std::size_t
{
    return annotations_.size();
}

auto NodeAnnotation::all_ids() const -> std::vector<FrameId>
{
    std::vector<FrameId> ids;
    ids.reserve(annotations_.size());
    for (const auto& [annotation_id, annotation] : annotations_)
    {
        ids.push_back(annotation_id);
    }
    return ids;
}

auto NodeAnnotation::frames_at(Vec2 world_pos) const -> std::vector<FrameId>
{
    std::vector<FrameId> hits;
    for (const auto& [annotation_id, annotation] : annotations_)
    {
        if (annotation.visible && annotation.bounds.contains(world_pos))
        {
            hits.push_back(annotation_id);
        }
    }
    return hits;
}

void NodeAnnotation::update_containment(FrameId annotation_id, const std::vector<NodeId>& contained)
{
    auto* annotation = find_mut(annotation_id);
    if (annotation != nullptr)
    {
        annotation->contained_nodes = contained;
    }
}

auto NodeAnnotation::containing_frame(Vec2 node_center) const -> FrameId
{
    for (const auto& [annotation_id, annotation] : annotations_)
    {
        if (annotation.type == AnnotationType::Frame && annotation.visible &&
            annotation.bounds.contains(node_center))
        {
            return annotation_id;
        }
    }
    return {};
}

void NodeAnnotation::clear()
{
    annotations_.clear();
}

} // namespace markamp::node_editor
