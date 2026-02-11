#pragma once

#include "EventBus.h"

#include <string>

namespace markamp::core::events
{

// --- Theme events ---
struct ThemeChangedEvent : Event
{
    std::string theme_id;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ThemeChangedEvent";
    }
};

// --- File events ---
struct FileOpenedEvent : Event
{
    std::string file_path;
    std::string content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileOpenedEvent";
    }
};

struct FileContentChangedEvent : Event
{
    std::string file_id;
    std::string new_content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileContentChangedEvent";
    }
};

struct FileSavedEvent : Event
{
    std::string file_path;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileSavedEvent";
    }
};

struct ActiveFileChangedEvent : Event
{
    std::string file_id;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ActiveFileChangedEvent";
    }
};

// --- View events ---
enum class ViewMode
{
    Editor,
    Preview,
    Split
};

enum class WrapMode
{
    None,
    Word,
    Character
};

struct ViewModeChangedEvent : Event
{
    ViewMode mode{ViewMode::Split};

    ViewModeChangedEvent() = default;
    explicit ViewModeChangedEvent(ViewMode m)
        : mode(m)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ViewModeChangedEvent";
    }
};

struct SidebarToggleEvent : Event
{
    bool visible;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SidebarToggleEvent";
    }
};

// --- Editor events ---
struct CursorPositionChangedEvent : Event
{
    int line;
    int column;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CursorPositionChangedEvent";
    }
};

struct EditorContentChangedEvent : Event
{
    std::string content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EditorContentChangedEvent";
    }
};

// --- Application events ---
struct AppReadyEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AppReadyEvent";
    }
};

struct AppShutdownEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AppShutdownEvent";
    }
};

// --- Scroll sync events ---
struct EditorScrollChangedEvent : Event
{
    double scroll_fraction{0.0}; // 0.0 = top, 1.0 = bottom
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EditorScrollChangedEvent";
    }
};

// --- Status bar events ---
struct FileEncodingDetectedEvent : Event
{
    std::string encoding_name; // e.g. "UTF-8", "UTF-8 BOM", "ASCII"
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileEncodingDetectedEvent";
    }
};

struct MermaidRenderStatusEvent : Event
{
    std::string status; // "ACTIVE", "INACTIVE", "RENDERING", "ERROR"
    bool active{false};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MermaidRenderStatusEvent";
    }
};

// --- Accessibility events ---
struct UIScaleChangedEvent : Event
{
    float scale_factor{1.0F};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UIScaleChangedEvent";
    }
};

struct InputModeChangedEvent : Event
{
    bool using_keyboard{false};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InputModeChangedEvent";
    }
};

} // namespace markamp::core::events
