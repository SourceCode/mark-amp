#include "ui/IconRenderer.h"

#include <wx/dcmemory.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>
#include <memory>

namespace markamp::ui
{

auto IconRenderer::RenderIcon(const SvgDocument& doc,
                              const wxSize& logicalSize,
                              const wxColour& color,
                              double scaleFactor) -> wxBitmap
{
    wxSize pixelSize(static_cast<int>(std::round(logicalSize.GetWidth() * scaleFactor)),
                     static_cast<int>(std::round(logicalSize.GetHeight() * scaleFactor)));

    if (pixelSize.GetWidth() <= 0 || pixelSize.GetHeight() <= 0)
    {
        return wxBitmap();
    }

    wxBitmap bitmap(pixelSize, 32);
    bitmap.UseAlpha();

    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);

    // Clear bitmap to transparent
    memDC.SetBackground(*wxTRANSPARENT_BRUSH);
    memDC.Clear();

    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(memDC));
    if (!gc)
    {
        return bitmap; // Fallback if creation fails
    }

    gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);

    // Calculate scaling to map the viewBox to the output pixel size
    float scaleX = static_cast<float>(pixelSize.GetWidth());
    float scaleY = static_cast<float>(pixelSize.GetHeight());

    if (doc.viewBox().is_valid())
    {
        scaleX /= doc.viewBox().width;
        scaleY /= doc.viewBox().height;
        // Translate to viewBox origin
        gc->Translate(-doc.viewBox().x * scaleX, -doc.viewBox().y * scaleY);
    }
    else
    {
        // Guess a default viewBox if missing (most icons are 24x24)
        scaleX /= 24.0f;
        scaleY /= 24.0f;
    }

    gc->Scale(scaleX, scaleY);

    for (const auto& shape_variant : doc.shapes())
    {
        if (const auto* p = std::get_if<SvgPath>(&shape_variant))
        {
            wxGraphicsPath path = gc->CreatePath();

            float currentX = 0.0f;
            float currentY = 0.0f;
            float lastControlX = 0.0f;
            float lastControlY = 0.0f;

            for (const auto& cmd : p->commands)
            {
                int argc = static_cast<int>(cmd.args.size());
                int argIndex = 0;

                while (argIndex < argc ||
                       argc == 0) // Executes at least once for Z/z and arg-less calls
                {
                    switch (cmd.type)
                    {
                        case 'M':
                            if (argIndex + 1 < argc)
                            {
                                currentX = cmd.args[argIndex];
                                currentY = cmd.args[argIndex + 1];
                                path.MoveToPoint(currentX, currentY);
                                argIndex += 2;
                            }
                            break;
                        case 'm':
                            if (argIndex + 1 < argc)
                            {
                                currentX += cmd.args[argIndex];
                                currentY += cmd.args[argIndex + 1];
                                path.MoveToPoint(currentX, currentY);
                                argIndex += 2;
                            }
                            break;
                        case 'L':
                            if (argIndex + 1 < argc)
                            {
                                currentX = cmd.args[argIndex];
                                currentY = cmd.args[argIndex + 1];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 2;
                            }
                            break;
                        case 'l':
                            if (argIndex + 1 < argc)
                            {
                                currentX += cmd.args[argIndex];
                                currentY += cmd.args[argIndex + 1];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 2;
                            }
                            break;
                        case 'H':
                            if (argIndex < argc)
                            {
                                currentX = cmd.args[argIndex];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 1;
                            }
                            break;
                        case 'h':
                            if (argIndex < argc)
                            {
                                currentX += cmd.args[argIndex];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 1;
                            }
                            break;
                        case 'V':
                            if (argIndex < argc)
                            {
                                currentY = cmd.args[argIndex];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 1;
                            }
                            break;
                        case 'v':
                            if (argIndex < argc)
                            {
                                currentY += cmd.args[argIndex];
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 1;
                            }
                            break;
                        case 'C':
                            if (argIndex + 5 < argc)
                            {
                                float cx1 = cmd.args[argIndex];
                                float cy1 = cmd.args[argIndex + 1];
                                float cx2 = cmd.args[argIndex + 2];
                                float cy2 = cmd.args[argIndex + 3];
                                currentX = cmd.args[argIndex + 4];
                                currentY = cmd.args[argIndex + 5];
                                path.AddCurveToPoint(cx1, cy1, cx2, cy2, currentX, currentY);
                                lastControlX = cx2;
                                lastControlY = cy2;
                                argIndex += 6;
                            }
                            break;
                        case 'c':
                            if (argIndex + 5 < argc)
                            {
                                float cx1 = currentX + cmd.args[argIndex];
                                float cy1 = currentY + cmd.args[argIndex + 1];
                                float cx2 = currentX + cmd.args[argIndex + 2];
                                float cy2 = currentY + cmd.args[argIndex + 3];
                                currentX += cmd.args[argIndex + 4];
                                currentY += cmd.args[argIndex + 5];
                                path.AddCurveToPoint(cx1, cy1, cx2, cy2, currentX, currentY);
                                lastControlX = cx2;
                                lastControlY = cy2;
                                argIndex += 6;
                            }
                            break;
                        case 'S':
                            if (argIndex + 3 < argc)
                            {
                                float cx1 = currentX * 2.0f - lastControlX;
                                float cy1 = currentY * 2.0f - lastControlY;
                                float cx2 = cmd.args[argIndex];
                                float cy2 = cmd.args[argIndex + 1];
                                currentX = cmd.args[argIndex + 2];
                                currentY = cmd.args[argIndex + 3];
                                path.AddCurveToPoint(cx1, cy1, cx2, cy2, currentX, currentY);
                                lastControlX = cx2;
                                lastControlY = cy2;
                                argIndex += 4;
                            }
                            break;
                        case 's':
                            if (argIndex + 3 < argc)
                            {
                                float cx1 = currentX * 2.0f - lastControlX;
                                float cy1 = currentY * 2.0f - lastControlY;
                                float cx2 = currentX + cmd.args[argIndex];
                                float cy2 = currentY + cmd.args[argIndex + 1];
                                currentX += cmd.args[argIndex + 2];
                                currentY += cmd.args[argIndex + 3];
                                path.AddCurveToPoint(cx1, cy1, cx2, cy2, currentX, currentY);
                                lastControlX = cx2;
                                lastControlY = cy2;
                                argIndex += 4;
                            }
                            break;
                        case 'Q':
                            if (argIndex + 3 < argc)
                            {
                                float cx = cmd.args[argIndex];
                                float cy = cmd.args[argIndex + 1];
                                currentX = cmd.args[argIndex + 2];
                                currentY = cmd.args[argIndex + 3];
                                path.AddQuadCurveToPoint(cx, cy, currentX, currentY);
                                lastControlX = cx;
                                lastControlY = cy;
                                argIndex += 4;
                            }
                            break;
                        case 'q':
                            if (argIndex + 3 < argc)
                            {
                                float cx = currentX + cmd.args[argIndex];
                                float cy = currentY + cmd.args[argIndex + 1];
                                currentX += cmd.args[argIndex + 2];
                                currentY += cmd.args[argIndex + 3];
                                path.AddQuadCurveToPoint(cx, cy, currentX, currentY);
                                lastControlX = cx;
                                lastControlY = cy;
                                argIndex += 4;
                            }
                            break;
                        case 'T':
                            if (argIndex + 1 < argc)
                            {
                                float cx = currentX * 2.0f - lastControlX;
                                float cy = currentY * 2.0f - lastControlY;
                                currentX = cmd.args[argIndex];
                                currentY = cmd.args[argIndex + 1];
                                path.AddQuadCurveToPoint(cx, cy, currentX, currentY);
                                lastControlX = cx;
                                lastControlY = cy;
                                argIndex += 2;
                            }
                            break;
                        case 't':
                            if (argIndex + 1 < argc)
                            {
                                float cx = currentX * 2.0f - lastControlX;
                                float cy = currentY * 2.0f - lastControlY;
                                currentX += cmd.args[argIndex];
                                currentY += cmd.args[argIndex + 1];
                                path.AddQuadCurveToPoint(cx, cy, currentX, currentY);
                                lastControlX = cx;
                                lastControlY = cy;
                                argIndex += 2;
                            }
                            break;
                        case 'A':
                        case 'a':
                            // wxGraphicsPath lacks AddArcToPoint taking endpoint and radii +
                            // rotation directly as an SVG arc. For a perfectly correct parser, we
                            // must convert endpoint parameterization to center parameterization.
                            // However, strictly drawing lines as a fallback for SVG arcs is
                            // sometimes sufficient for minimal UI. Assuming simple paths mostly for
                            // now. We will just draw a line.
                            if (argIndex + 6 < argc)
                            {
                                if (cmd.type == 'A')
                                {
                                    currentX = cmd.args[argIndex + 5];
                                    currentY = cmd.args[argIndex + 6];
                                }
                                else
                                {
                                    currentX += cmd.args[argIndex + 5];
                                    currentY += cmd.args[argIndex + 6];
                                }
                                path.AddLineToPoint(currentX, currentY);
                                argIndex += 7;
                            }
                            break;
                        case 'Z':
                        case 'z':
                            path.CloseSubpath();
                            break;
                    }

                    if (argc == 0)
                        break; // Break loop if command had no args (like Z)
                }
            }

            // For monotone icons, we use the base color, regardless of what's originally set,
            // or if it was explicitly "none" we don't draw.
            if (p->fill != "none")
            {
                gc->SetBrush(wxBrush(color));
                gc->FillPath(path);
            }
            if (p->stroke != "none" && !p->stroke.empty())
            {
                // Adjust stroke width by our scaled pixels?
                // In wxGraphicsContext, if we scaled the context, pen width is scaled
                // automatically!
                gc->SetPen(wxPen(color, std::max(1.0f, p->stroke_width)));
                gc->StrokePath(path);
            }
        }
        else if (const auto* c = std::get_if<SvgCircle>(&shape_variant))
        {
            wxGraphicsPath path = gc->CreatePath();
            path.AddCircle(c->cx, c->cy, c->r);
            if (c->fill != "none")
            {
                gc->SetBrush(wxBrush(color));
                gc->FillPath(path);
            }
            if (c->stroke != "none" && !c->stroke.empty())
            {
                gc->SetPen(wxPen(color, std::max(1.0f, c->stroke_width)));
                gc->StrokePath(path);
            }
        }
        else if (const auto* r = std::get_if<SvgRect>(&shape_variant))
        {
            wxGraphicsPath path = gc->CreatePath();
            if (r->rx > 0.0f || r->ry > 0.0f)
            {
                path.AddRoundedRectangle(r->x, r->y, r->width, r->height, r->rx);
            }
            else
            {
                path.AddRectangle(r->x, r->y, r->width, r->height);
            }
            if (r->fill != "none")
            {
                gc->SetBrush(wxBrush(color));
                gc->FillPath(path);
            }
            if (r->stroke != "none" && !r->stroke.empty())
            {
                gc->SetPen(wxPen(color, std::max(1.0f, r->stroke_width)));
                gc->StrokePath(path);
            }
        }
        else if (const auto* l = std::get_if<SvgLine>(&shape_variant))
        {
            wxGraphicsPath path = gc->CreatePath();
            path.MoveToPoint(l->x1, l->y1);
            path.AddLineToPoint(l->x2, l->y2);
            if (l->stroke != "none" && !l->stroke.empty())
            {
                gc->SetPen(wxPen(color, std::max(1.0f, l->stroke_width)));
                gc->StrokePath(path);
            }
        }
        else if (const auto* pl = std::get_if<SvgPolyline>(&shape_variant))
        {
            if (pl->points.size() >= 2)
            {
                wxGraphicsPath path = gc->CreatePath();
                path.MoveToPoint(pl->points[0], pl->points[1]);
                for (size_t i = 2; i + 1 < pl->points.size(); i += 2)
                {
                    path.AddLineToPoint(pl->points[i], pl->points[i + 1]);
                }
                if (pl->fill != "none")
                {
                    gc->SetBrush(wxBrush(color));
                    gc->FillPath(path);
                }
                if (pl->stroke != "none" && !pl->stroke.empty())
                {
                    gc->SetPen(wxPen(color, std::max(1.0f, pl->stroke_width)));
                    gc->StrokePath(path);
                }
            }
        }
    }

    memDC.SelectObject(wxNullBitmap);
    return bitmap;
}

} // namespace markamp::ui
