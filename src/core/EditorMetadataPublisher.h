/// @file EditorMetadataPublisher.h
/// @brief P04-T05: Publishes active editor metadata to status bar and shell.
///
/// Collects cursor position, language, file size, encoding, EOL, indent,
/// zoom, and git branch from the active editor and workspace state, and
/// publishes EditorStatsChangedEvent for the status bar.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Metadata snapshot for the active editor.
struct EditorMetadata
{
    int cursor_line{1};
    int cursor_column{1};
    int selection_count{0};
    std::string language;
    std::string encoding{"UTF-8"};
    std::string eol_mode{"LF"};
    std::string indent_mode{"Spaces: 4"};
    int file_size_bytes{0};
    int line_count{0};
    int word_count{0};
    std::string git_branch;
    int zoom_percent{100};
};

/// Publisher that keeps editor metadata in sync with the status bar.
class EditorMetadataPublisher
{
public:
    explicit EditorMetadataPublisher(EventBus& bus);

    /// Update metadata from active editor state and publish.
    void update(const EditorMetadata& metadata);

    /// Clear metadata (no active editor).
    void clear();

    /// Get the current metadata.
    [[nodiscard]] auto current() const -> const EditorMetadata& { return current_; }

    /// Check if metadata is populated.
    [[nodiscard]] auto has_metadata() const -> bool { return populated_; }

private:
    void publish_stats();

    EventBus& event_bus_;
    EditorMetadata current_;
    bool populated_{false};
};

} // namespace markamp::core
