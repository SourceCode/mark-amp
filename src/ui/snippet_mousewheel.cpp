void EditorPanel::OnMouseWheel(wxMouseEvent& event)
{
    if (event.CmdDown())
    {
        // Ctrl/Cmd + Wheel for Zoom
        int rotation = event.GetWheelRotation();
        if (rotation > 0)
        {
            ZoomIn();
        }
        else if (rotation < 0)
        {
            ZoomOut();
        }
    }
    else
    {
        event.Skip();
    }
}
