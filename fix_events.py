import re

with open('src/ui/StatusBarPanel.cpp', 'r') as f:
    content = f.read()

# Fix EditorStatsChangedEvent
bad_content_sub = """    content_sub_ = event_bus_.subscribe<core::events::DocumentStatsEvent>(
        [this](const core::events::DocumentStatsEvent& evt)
        {
            set_stats(evt.word_count, evt.char_count, evt.line_count, evt.selection_len);
        });"""
good_content_sub = """    content_sub_ = event_bus_.subscribe<core::events::EditorStatsChangedEvent>(
        [this](const core::events::EditorStatsChangedEvent& evt)
        { set_stats(evt.word_count, evt.char_count, evt.line_count, evt.selection_length); });"""
if bad_content_sub in content:
    content = content.replace(bad_content_sub, good_content_sub)

# Fix EncodingChangedEvent
bad_encoding_sub = """    encoding_sub_ = event_bus_.subscribe<core::events::EncodingChangedEvent>(
        [this](const core::events::EncodingChangedEvent& evt)
        {
            set_encoding(evt.encoding);
        });"""
good_encoding_sub = """    encoding_sub_ = event_bus_.subscribe<core::events::FileEncodingDetectedEvent>(
        [this](const core::events::FileEncodingDetectedEvent& evt)
        {
            encoding_ = evt.encoding_name;
            RebuildItems();
            Refresh();
        });"""
if bad_encoding_sub in content:
    content = content.replace(bad_encoding_sub, good_encoding_sub)

# Fix MermaidStatusEvent
bad_mermaid_sub = """    mermaid_sub_ = event_bus_.subscribe<core::events::MermaidStatusEvent>(
        [this](const core::events::MermaidStatusEvent& evt)
        {
            set_mermaid_status(evt.status, evt.active);
        });"""
good_mermaid_sub = """    mermaid_sub_ = event_bus_.subscribe<core::events::MermaidRenderStatusEvent>(
        [this](const core::events::MermaidRenderStatusEvent& evt)
        {
            mermaid_status_ = evt.status;
            mermaid_active_ = evt.active;
            RebuildItems();
            Refresh();
        });"""
if bad_mermaid_sub in content:
    content = content.replace(bad_mermaid_sub, good_mermaid_sub)

# Also fix the ViewModeChangedEvent because it might have been wrong
bad_view_mode_sub = """    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt)
        {
            set_view_mode(evt.new_mode);
        });"""
good_view_mode_sub = """    view_mode_sub_ = event_bus_.subscribe<core::events::ViewModeChangedEvent>(
        [this](const core::events::ViewModeChangedEvent& evt)
        {
            view_mode_ = evt.mode;
            RebuildItems();
            Refresh();
        });"""
if bad_view_mode_sub in content:
    content = content.replace(bad_view_mode_sub, good_view_mode_sub)

# Also fix ThemeInfoEvent vs ThemeChangeEvent
bad_theme_sub = """    theme_name_sub_ = event_bus_.subscribe<core::events::ThemeInfoEvent>(
        [this](const core::events::ThemeInfoEvent& evt)
        {
            theme_name_ = evt.name;
            RebuildItems();
            Refresh();
        });"""
good_theme_sub = """    theme_name_sub_ = ds_.theme.subscribe_theme_change(
        [this](const std::string& /*theme_id*/)
        {
            theme_name_ = ds_.theme.current_theme().name;
            RebuildItems();
            Refresh();
        });"""
if bad_theme_sub in content:
    content = content.replace(bad_theme_sub, good_theme_sub)

with open('src/ui/StatusBarPanel.cpp', 'w') as f:
    f.write(content)
