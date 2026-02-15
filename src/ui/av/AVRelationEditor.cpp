#include "AVRelationEditor.h"

namespace markamp::ui::av
{

void AVRelationEditor::set_state(AVRelationEditorState new_state)
{
    state_ = std::move(new_state);
}

} // namespace markamp::ui::av
