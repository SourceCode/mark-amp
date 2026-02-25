#pragma once

#include "ui/ThemeAwareWindow.h"

#include <wx/sizer.h>

#include <string>

namespace markamp::core
{
class EventBus;
class Config;
} // namespace markamp::core

namespace markamp::ui
{
struct DesignSystemContext;
class IconManager;

class SidebarSection : public ThemeAwareWindow
{
public:
    SidebarSection(wxWindow* parent,
                   DesignSystemContext& ds,
                   IconManager& icon_manager,
                   core::EventBus& event_bus,
                   core::Config* config,
                   const std::string& title,
                   const std::string& persistence_id = "");

    void set_content(wxWindow* content_window);
    [[nodiscard]] auto get_content() const -> wxWindow*
    {
        return content_;
    }
    void set_expanded(bool expanded);
    [[nodiscard]] auto is_expanded() const -> bool
    {
        return is_expanded_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;
    void OnSize(wxSizeEvent& event);

private:
    class SectionHeader : public ThemeAwareWindow
    {
    public:
        SectionHeader(SidebarSection* parent,
                      DesignSystemContext& ds,
                      IconManager& icon_manager,
                      const std::string& title);

        void UpdateMetrics();

        [[nodiscard]] auto get_title() const -> const std::string&
        {
            return title_;
        }

        bool AcceptsFocus() const override
        {
            return true;
        }

    protected:
        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnMotion(wxMouseEvent& event);
        void OnLeave(wxMouseEvent& event);
        void OnSetFocus(wxFocusEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnThemeChanged(const core::Theme& new_theme) override;

    private:
        SidebarSection* section_parent_;
        DesignSystemContext& ds_;
        IconManager& icon_manager_;
        std::string title_;
        int height_{22};
        bool is_hovered_{false};
        bool is_pressed_{false};

        wxDECLARE_EVENT_TABLE();
    };

    DesignSystemContext& ds_;
    IconManager& icon_manager_;
    core::Config* config_{nullptr};
    std::string persistence_id_;

    SectionHeader* header_{nullptr};
    wxWindow* content_{nullptr};
    wxBoxSizer* main_sizer_{nullptr};

    bool is_expanded_{true};

    // Private helpers
    void ToggleExpanded();

    friend class SectionHeader;
};

} // namespace markamp::ui
