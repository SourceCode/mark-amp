#include "TextEditorService.h"

#include <algorithm>

namespace markamp::core
{

// ── EditBuilder ──

void EditBuilder::insert(TextPosition position, const std::string& text)
{
    edits_.push_back({Edit::Type::kInsert, position, {}, text});
}

void EditBuilder::replace(TextRange range, const std::string& text)
{
    edits_.push_back({Edit::Type::kReplace, {}, range, text});
}

void EditBuilder::delete_range(TextRange range)
{
    edits_.push_back({Edit::Type::kDelete, {}, range, {}});
}

auto EditBuilder::edits() const -> const std::vector<Edit>&
{
    return edits_;
}

void EditBuilder::clear()
{
    edits_.clear();
}

// ── TextEditorService ──

auto TextEditorService::active_editor() -> TextEditor*
{
    if (active_editor_.has_value())
    {
        return &active_editor_.value();
    }
    return nullptr;
}

auto TextEditorService::active_editor() const -> const TextEditor*
{
    if (active_editor_.has_value())
    {
        return &active_editor_.value();
    }
    return nullptr;
}

void TextEditorService::set_active_editor(TextEditor editor)
{
    active_editor_ = std::move(editor);

    // Fire listeners
    for (const auto& entry : listeners_)
    {
        if (entry.is_editor_change && entry.editor_callback)
        {
            entry.editor_callback(active_editor_ ? &active_editor_.value() : nullptr);
        }
    }
}

void TextEditorService::clear_active_editor()
{
    active_editor_.reset();

    // Fire listeners with nullptr
    for (const auto& entry : listeners_)
    {
        if (entry.is_editor_change && entry.editor_callback)
        {
            entry.editor_callback(nullptr);
        }
    }
}

auto TextEditorService::visible_editors() const -> const std::vector<TextEditor>&
{
    return visible_editors_;
}

void TextEditorService::add_visible_editor(TextEditor editor)
{
    visible_editors_.push_back(std::move(editor));
}

void TextEditorService::remove_visible_editor(const std::string& uri)
{
    visible_editors_.erase(std::remove_if(visible_editors_.begin(),
                                          visible_editors_.end(),
                                          [&uri](const TextEditor& editor)
                                          { return editor.uri == uri; }),
                           visible_editors_.end());
}

auto TextEditorService::show_document(const std::string& uri, const ShowDocumentOptions& options)
    -> TextEditor*
{
    TextEditor editor;
    editor.uri = uri;
    editor.selections = {options.selection};

    if (!options.preserve_focus)
    {
        set_active_editor(editor);
        return active_editor();
    }

    add_visible_editor(std::move(editor));
    return &visible_editors_.back();
}

auto TextEditorService::on_did_change_active_editor(EditorCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back({listener_id, true, std::move(callback), nullptr});
    return listener_id;
}

auto TextEditorService::on_did_change_selection(SelectionCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back({listener_id, false, nullptr, std::move(callback)});
    return listener_id;
}

void TextEditorService::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const ListenerEntry& entry)
                                    { return entry.listener_id == listener_id; }),
                     listeners_.end());
}

void TextEditorService::fire_selection_change()
{
    if (!active_editor_.has_value())
    {
        return;
    }

    for (const auto& entry : listeners_)
    {
        if (!entry.is_editor_change && entry.selection_callback)
        {
            entry.selection_callback(active_editor_.value());
        }
    }
}

} // namespace markamp::core
