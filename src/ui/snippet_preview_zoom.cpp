void PreviewPanel::SetZoomLevel(int level)
{
    if (zoom_level_ == level)
        return;
    zoom_level_ = std::clamp(level, -5, 10);

    // Clear cache and re-render
    cached_css_.clear();
    if (!last_rendered_content_.empty())
    {
        auto content = last_rendered_content_;
        last_rendered_content_.clear();
        RenderContent(content);
    }
}

void PreviewPanel::OnMouseWheel(wxMouseEvent& event)
{
    if (event.CmdDown())
    {
        int rotation = event.GetWheelRotation();
        if (rotation > 0)
        {
            SetZoomLevel(zoom_level_ + 1);
        }
        else if (rotation < 0)
        {
            SetZoomLevel(zoom_level_ - 1);
        }
    }
    else
    {
        event.Skip();
    }
}

void PreviewPanel::OnKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    bool cmd = event.CmdDown();

    if (cmd && (key == '=' || key == WXK_NUMPAD_ADD || key == '+'))
    {
        SetZoomLevel(zoom_level_ + 1);
        return;
    }

    if (cmd && (key == '-' || key == WXK_NUMPAD_SUBTRACT))
    {
        SetZoomLevel(zoom_level_ - 1);
        return;
    }

    if (cmd && (key == '0' || key == WXK_NUMPAD0))
    {
        SetZoomLevel(0);
        return;
    }

    event.Skip();
}
