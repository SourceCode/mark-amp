/// @file EditorMetadataPublisher.cpp
/// @brief P04-T05: Publishes active editor metadata to status bar and shell.

#include "EditorMetadataPublisher.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

EditorMetadataPublisher::EditorMetadataPublisher(EventBus& bus)
    : event_bus_(bus)
{
}

void EditorMetadataPublisher::update(const EditorMetadata& metadata)
{
    current_ = metadata;
    populated_ = true;
    publish_stats();
}

void EditorMetadataPublisher::clear()
{
    current_ = EditorMetadata{};
    populated_ = false;
    publish_stats();
    MARKAMP_LOG_DEBUG("Editor metadata cleared");
}

void EditorMetadataPublisher::publish_stats()
{
    events::EditorStatsChangedEvent evt;
    evt.line_count = current_.line_count;
    evt.word_count = current_.word_count;
    evt.char_count = current_.file_size_bytes;
    evt.selection_length = current_.selection_count;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Editor metadata published: L{} C{}, {} lines, lang={}",
                      current_.cursor_line,
                      current_.cursor_column,
                      current_.line_count,
                      current_.language);
}

} // namespace markamp::core
