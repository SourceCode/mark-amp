#pragma once

#include "../../core/av/AVRelationService.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVRelationEditorState
{
    std::string av_id;
    std::string block_id;
    std::string key_id;
    std::string search_query;
    std::vector<markamp::core::av::ResolvedRelation> selected_relations;
};

/// Stub: Relation cell editor (block picker popup).
class AVRelationEditor
{
public:
    AVRelationEditor() = default;
    ~AVRelationEditor() = default;

    void set_state(AVRelationEditorState new_state);
    [[nodiscard]] auto state() const -> const AVRelationEditorState&
    {
        return state_;
    }

private:
    AVRelationEditorState state_;
};

} // namespace markamp::ui::av
