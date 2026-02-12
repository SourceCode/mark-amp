#include "PreviewPanel.h"

#include "BevelPanel.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/IMermaidRenderer.h"
#include "core/Profiler.h"
#include "core/ServiceRegistry.h"
#include "rendering/HtmlRenderer.h"

#include <wx/clipbrd.h>
#include <wx/sizer.h>

#include <fmt/format.h>
#include <fstream>
#include <spdlog/spdlog.h>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

PreviewPanel::PreviewPanel(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           core::Config* config)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
    , render_timer_(this)
    , resize_timer_(this)
    , scroll_sync_timer_(this)
{
    // Load config
    if (config)
    {
        render_debounce_ms_ = config->get_int("preview.render_debounce_ms", 300);
    }

    // Layout: single wxHtmlWindow filling the panel
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    html_view_ = new wxHtmlWindow(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO | wxHW_NO_SELECTION);
    sizer->Add(html_view_, 1, wxEXPAND);
    SetSizer(sizer);

    // Bevel overlay (sunken effect, non-interactive)
    // NOTE: hidden because it blocks the html_view_ on macOS.
    bevel_overlay_ = new BevelPanel(this, wxID_ANY, BevelPanel::Style::Sunken);
    bevel_overlay_->SetBackgroundColour(wxColour(0, 0, 0, 0));
    bevel_overlay_->Enable(false); // Non-interactive: won't steal events
    bevel_overlay_->Hide();        // Disabled: blocks preview on macOS

    // Apply initial theme
    const auto& t = theme();
    auto bg = t.colors.bg_app.to_wx_colour();
    html_view_->SetBackgroundColour(bg);
    SetBackgroundColour(bg);

    // Timer for debounced rendering
    Bind(wxEVT_TIMER, &PreviewPanel::OnRenderTimer, this, render_timer_.GetId());

    // Timer for debounced resize re-render (Improvement 24)
    Bind(wxEVT_TIMER, &PreviewPanel::OnResizeTimer, this, resize_timer_.GetId());

    // Timer for scroll sync debounce
    Bind(wxEVT_TIMER, &PreviewPanel::OnScrollSyncTimer, this, scroll_sync_timer_.GetId());

    // Link click handling
    html_view_->Bind(wxEVT_HTML_LINK_CLICKED, &PreviewPanel::OnLinkClicked, this);

    // Resize handling
    Bind(wxEVT_SIZE, &PreviewPanel::OnSize, this);

    // Zoom / Input handling
    html_view_->Bind(wxEVT_MOUSEWHEEL, &PreviewPanel::OnMouseWheel, this);
    html_view_->Bind(wxEVT_KEY_DOWN, &PreviewPanel::OnKeyDown, this);
    Bind(wxEVT_MOUSEWHEEL, &PreviewPanel::OnMouseWheel, this);
    Bind(wxEVT_KEY_DOWN, &PreviewPanel::OnKeyDown, this);

    // Subscribe to editor content changes (debounced)
    content_changed_sub_ = event_bus_.subscribe<core::events::EditorContentChangedEvent>(
        [this](const core::events::EditorContentChangedEvent& evt)
        {
            pending_content_ = evt.content;
            render_timer_.Start(render_debounce_ms_, wxTIMER_ONE_SHOT);
        });

    // Subscribe to active file changes (immediate render + scroll to top)
    active_file_sub_ = event_bus_.subscribe<core::events::ActiveFileChangedEvent>(
        [this](const core::events::ActiveFileChangedEvent& /*evt*/) { ScrollToTop(); });

    // Subscribe to editor scroll changes for scroll sync
    scroll_sync_sub_ = event_bus_.subscribe<core::events::EditorScrollChangedEvent>(
        [this](const core::events::EditorScrollChangedEvent& evt)
        {
            if (scroll_sync_enabled_)
            {
                pending_scroll_fraction_ = evt.scroll_fraction;
                scroll_sync_timer_.Start(kScrollSyncDebounceMs, wxTIMER_ONE_SHOT);
            }
        });

    spdlog::debug("PreviewPanel created");
}

// ═══════════════════════════════════════════════════════
// Content
// ═══════════════════════════════════════════════════════

void PreviewPanel::SetMarkdownContent(const std::string& markdown)
{
    // Cancel any pending debounced render
    render_timer_.Stop();
    pending_content_.clear();

    RenderContent(markdown);
}

void PreviewPanel::Clear()
{
    render_timer_.Stop();
    pending_content_.clear();
    last_rendered_content_.clear();
    if (html_view_ != nullptr)
    {
        html_view_->SetPage("<html><body></body></html>");
    }
}

// ═══════════════════════════════════════════════════════
// Scrolling
// ═══════════════════════════════════════════════════════

void PreviewPanel::ScrollToTop()
{
    if (html_view_ != nullptr)
    {
        html_view_->Scroll(0, 0);
    }
}

// ═══════════════════════════════════════════════════════
// CSS generation
// ═══════════════════════════════════════════════════════

auto PreviewPanel::GenerateCSS() const -> std::string
{
    // Improvement 4: return cached CSS if theme hasn't changed
    if (!cached_css_.empty())
    {
        return cached_css_;
    }

    const auto& t = theme();
    const auto& c = t.colors;

    // Accent at various opacities for backgrounds
    auto accent_bg_5 = c.accent_primary.with_alpha(0.05f).to_rgba_string();
    auto accent_bg_20 = c.accent_primary.with_alpha(0.20f).to_rgba_string();
    auto accent_bg_30 = c.accent_primary.with_alpha(0.30f).to_rgba_string();
    auto border_30 = c.border_light.with_alpha(0.30f).to_rgba_string();
    auto code_bg = c.bg_app.blend(core::Color{0, 0, 0, 255}, 0.3f).to_rgba_string();
    auto text_main_90 = c.text_main.with_alpha(0.90f).to_rgba_string();

    auto result = fmt::format(R"(
* {{
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}}
::selection {{
    background: {accent_bg_30};
}}
body {{
    background-color: {bg_app};
    color: {text_main};
    font-family: 'Rajdhani', -apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif;
    font-family: 'Rajdhani', -apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif;
    font-size: {}px;
    line-height: 1.6;
    padding: 24px;
    word-wrap: break-word;
}}
h1 {{
    font-size: 28px;
    font-weight: bold;
    color: {accent};
    border-bottom: 1px solid {border};
    padding-bottom: 8px;
    margin-bottom: 24px;
    margin-top: 0;
}}
h2 {{
    font-size: 22px;
    font-weight: 600;
    color: {accent};
    margin-top: 32px;
    margin-bottom: 16px;
}}
h3 {{
    font-size: 18px;
    font-weight: 500;
    color: {text_main};
    margin-top: 24px;
    margin-bottom: 12px;
}}
h4, h5, h6 {{
    font-size: 16px;
    font-weight: 500;
    color: {text_main};
    margin-top: 20px;
    margin-bottom: 8px;
}}
p {{
    margin-bottom: 16px;
    line-height: 1.7;
    color: {text_main_90};
}}
a {{
    color: {accent};
    text-decoration: none;
}}
a:hover {{
    text-decoration: underline;
    text-underline-offset: 4px;
}}
blockquote {{
    border-left: 4px solid {accent};
    padding-left: 16px;
    margin: 16px 0;
    font-style: italic;
    background-color: {accent_bg_5};
    padding: 12px 16px;
    color: {text_muted};
}}
ul, ol {{
    margin-bottom: 16px;
    padding-left: 24px;
}}
li {{
    margin-bottom: 4px;
    color: {text_main};
}}
li::marker {{
    color: {accent};
}}
hr {{
    border: none;
    border-top: 1px solid {border};
    margin: 32px 0;
}}
table {{
    border-collapse: collapse;
    width: 100%;
    margin-bottom: 16px;
    border: 1px solid {border};
}}
th {{
    background-color: {bg_panel};
    padding: 8px 12px;
    border-bottom: 2px solid {border};
    font-weight: 600;
    color: {accent};
    text-align: left;
}}
td {{
    padding: 8px 12px;
    border-bottom: 1px solid {border_30};
}}
code {{
    background-color: {accent_bg_20};
    color: {accent};
    padding: 2px 6px;
    border-radius: 4px;
    font-family: 'SF Mono', 'Menlo', 'Monaco', 'Courier New', monospace;
    font-size: 13px;
}}
pre {{
    background-color: {code_bg};
    padding: 16px;
    margin-bottom: 16px;
    overflow-x: auto;
    border: 1px solid {border};
    border-radius: 4px;
}}
pre code {{
    background-color: transparent;
    color: {text_main};
    padding: 0;
    border-radius: 0;
}}
.code-block-wrapper {{
    position: relative;
    margin: 24px 0;
}}
.code-block-header {{
    position: absolute;
    right: 0;
    top: 0;
    z-index: 1;
    padding: 4px 8px;
}}
.language-label {{
    font-size: 11px;
    color: {text_muted};
    opacity: 0.6;
    font-family: 'SF Mono', monospace;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}}
.code-block {{
    background-color: {code_bg};
    padding: 16px;
    border-radius: 4px;
    border: 1px solid {border};
    overflow-x: auto;
    margin: 0;
}}
.code-block code {{
    background-color: transparent;
    color: {text_main};
    padding: 0;
    font-family: 'JetBrains Mono', 'SF Mono', 'Menlo', monospace;
    font-size: 13px;
    line-height: 1.5;
}}
.token-keyword {{ color: {accent}; font-weight: 600; }}
.token-string {{ color: {accent_secondary}; }}
.token-number {{ color: {accent_secondary}; }}
.token-comment {{ color: {text_muted}; opacity: 0.6; font-style: italic; }}
.token-operator {{ color: {text_main}; }}
.token-function {{ color: {accent}; opacity: 0.85; }}
.token-type {{ color: {accent_secondary}; }}
.token-constant {{ color: {accent}; }}
.token-preprocessor {{ color: {text_muted}; }}
.token-tag {{ color: {accent}; }}
.token-property {{ color: {accent_secondary}; }}
.token-variable {{ color: {text_main}; }}
.token-punctuation {{ color: {text_muted}; }}
.token-attribute {{ color: {accent_secondary}; font-style: italic; }}
img {{
    max-width: 100%;
    height: auto;
}}
.image-missing {{
    border: 1px dashed {border};
    padding: 16px;
    text-align: center;
    color: {text_muted};
    font-size: 13px;
    margin: 16px 0;
    background-color: {accent_bg_5};
    border-radius: 4px;
}}
del {{
    text-decoration: line-through;
    color: {text_muted};
}}
.mermaid-block {{
    background-color: {bg_panel};
    border: 1px solid {border};
    padding: 16px;
    margin: 16px 0;
    text-align: center;
    color: {text_muted};
    font-style: italic;
}}
.mermaid-container {{
    margin: 16px 0;
    display: flex;
    justify-content: center;
    background-color: {bg_panel};
    padding: 16px;
    border-radius: 4px;
    border: 1px solid {border};
}}
.mermaid-container img {{
    max-width: 100%;
    height: auto;
}}
.mermaid-error {{
    padding: 16px;
    border: 1px solid rgba(255, 0, 0, 0.5);
    background-color: rgba(153, 0, 0, 0.2);
    color: #f87171;
    font-family: 'SF Mono', monospace;
    font-size: 14px;
    border-radius: 4px;
    margin: 16px 0;
}}
.mermaid-unavailable {{
    padding: 16px;
    border: 1px solid {border};
    background-color: {bg_panel};
    color: {text_muted};
    font-size: 14px;
    border-radius: 4px;
    margin: 16px 0;
    text-align: center;
}}
.error-overlay {{
    background-color: rgba(255, 100, 100, 0.1);
    border: 1px solid rgba(255, 100, 100, 0.3);
    color: #ff6464;
    padding: 12px 16px;
    margin: 16px 0;
    font-family: 'SF Mono', monospace;
    font-size: 13px;
}}
.table-wrapper {{
    overflow-x: auto;
    margin: 24px 0;
    border: 1px solid {border};
    border-radius: 4px;
}}
.table-wrapper table {{
    margin-bottom: 0;
    border: none;
}}
.task-list {{
    list-style: none;
    padding-left: 8px;
}}
.task-item {{
    margin-bottom: 6px;
}}
.task-item input[type="checkbox"] {{
    margin-right: 8px;
    vertical-align: middle;
}}
.footnotes {{
    margin-top: 48px;
    font-size: 12px;
    color: {text_muted};
}}
.footnotes hr {{
    margin-bottom: 16px;
}}
.footnotes ol {{
    padding-left: 20px;
}}
.footnotes li {{
    margin-bottom: 8px;
    color: {text_muted};
}}
.footnote-ref a {{
    color: {accent};
    font-size: 11px;
    text-decoration: none;
}}
.footnote-backref {{
    color: {accent_secondary};
    text-decoration: none;
    margin-left: 4px;
}}
/* ── Phase 4: Preview CSS Improvements ── */
/* Item 31: Smooth scroll */
html {{
    scroll-behavior: smooth;
}}
/* Item 32: Heading anchor links */
h1, h2, h3, h4, h5, h6 {{
    position: relative;
}}
.heading-anchor {{
    opacity: 0;
    text-decoration: none;
    color: {text_muted};
    margin-left: 8px;
    font-weight: normal;
    font-size: 0.8em;
    transition: opacity 0.15s ease;
}}
h1:hover .heading-anchor,
h2:hover .heading-anchor,
h3:hover .heading-anchor,
h4:hover .heading-anchor,
h5:hover .heading-anchor,
h6:hover .heading-anchor {{
    opacity: 0.6;
}}
.heading-anchor:hover {{
    opacity: 1 !important;
    color: {accent};
}}
/* Item 34: Code block line numbers */
.code-block-numbered {{
    counter-reset: line;
}}
.code-block-numbered .code-line {{
    counter-increment: line;
    display: block;
}}
.code-block-numbered .code-line::before {{
    content: counter(line);
    display: inline-block;
    width: 2em;
    margin-right: 12px;
    text-align: right;
    color: {text_muted};
    opacity: 0.4;
    font-size: 12px;
    user-select: none;
}}
/* Item 36: Dark mode image contrast adjustment */
.dark-theme img {{
    filter: brightness(0.88) contrast(1.05);
}}
/* Item 37: Table zebra striping and hover */
tr:nth-child(even) {{
    background-color: {accent_bg_5};
}}
tr:hover {{
    background-color: {accent_bg_20};
}}
/* Item 38: Collapsible details/summary */
details {{
    border: 1px solid {border};
    border-radius: 4px;
    padding: 8px 16px;
    margin: 16px 0;
    background-color: {accent_bg_5};
}}
details[open] {{
    padding-bottom: 12px;
}}
summary {{
    cursor: pointer;
    font-weight: 600;
    color: {accent};
    padding: 4px 0;
    outline: none;
}}
summary:hover {{
    opacity: 0.85;
}}
summary::marker {{
    color: {accent};
}}
/* Item 39: KaTeX / math placeholder */
.math-inline {{
    font-family: 'SF Mono', 'Menlo', monospace;
    background-color: {accent_bg_5};
    padding: 2px 6px;
    border-radius: 3px;
    color: {accent_secondary};
    font-size: 13px;
}}
.math-block {{
    font-family: 'SF Mono', 'Menlo', monospace;
    background-color: {accent_bg_5};
    padding: 16px;
    margin: 16px 0;
    border-radius: 4px;
    border: 1px solid {border};
    color: {accent_secondary};
    text-align: center;
    font-size: 14px;
}}
/* ── Phase 6B: Line highlight ── */
.line-highlight {{
    background-color: {accent_bg_20};
    display: inline-block;
    width: 100%;
    border-left: 3px solid {accent};
    padding-left: 4px;
    margin-left: -4px;
}}
/* ── Phase 6C: Copy button ── */
.copy-btn {{
    font-size: 12px;
    color: {text_muted};
    text-decoration: none;
    margin-left: 12px;
    padding: 2px 6px;
    border-radius: 3px;
    opacity: 0.5;
    cursor: pointer;
}}
.copy-btn:hover {{
    opacity: 1;
    background-color: {accent_bg_20};
    color: {accent};
}}
/* ── Phase 6E: Enhanced inline vs block code contrast ── */
.code-block-wrapper {{
    position: relative;
    margin: 24px 0;
    border-left: 3px solid {accent};
}}
code {{
    background-color: {accent_bg_20};
    color: {accent};
    padding: 2px 6px;
    border-radius: 4px;
    font-family: 'SF Mono', 'Menlo', 'Monaco', 'Courier New', monospace;
    font-size: 12px;
    border: 1px solid {border_30};
}}
)",
                              fmt::arg("bg_app", c.bg_app.to_hex()),
                              fmt::arg("bg_panel", c.bg_panel.to_hex()),
                              fmt::arg("text_main", c.text_main.to_hex()),
                              fmt::arg("text_main_90", text_main_90),
                              fmt::arg("text_muted", c.text_muted.to_hex()),
                              fmt::arg("accent", c.accent_primary.to_hex()),
                              fmt::arg("accent_secondary", c.accent_secondary.to_hex()),
                              fmt::arg("border", c.border_light.to_hex()),
                              fmt::arg("accent_bg_5", accent_bg_5),
                              fmt::arg("accent_bg_20", accent_bg_20),
                              fmt::arg("accent_bg_30", accent_bg_30),
                              fmt::arg("border_30", border_30),
                              fmt::arg("code_bg", code_bg),
                              14 + zoom_level_ * 2);
    // Improvement 4: cache the generated CSS string
    cached_css_ = std::move(result);
    return cached_css_;
}

// ═══════════════════════════════════════════════════════
// Full HTML generation
// ═══════════════════════════════════════════════════════

auto PreviewPanel::GenerateFullHtml(const std::string& body_html) const -> std::string
{
    const auto& col = theme().colors;

    // wxHtmlWindow has LIMITED CSS support.  The <style> block provides
    // progressive enhancement, but the critical colors MUST be set via
    // legacy <body> attributes (bgcolor, text, link) that wxHtmlWindow
    // reliably honours.
    return fmt::format(R"(<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
{}
</style>
</head>
<body bgcolor="{}" text="{}" link="{}">
{}
</body>
</html>)",
                       GenerateCSS(),
                       col.bg_app.to_hex(),
                       col.text_main.to_hex(),
                       col.accent_primary.to_hex(),
                       body_html);
}

// ═══════════════════════════════════════════════════════
// Rendering pipeline
// ═══════════════════════════════════════════════════════

void PreviewPanel::RenderContent(const std::string& markdown)
{
    MARKAMP_PROFILE_SCOPE("PreviewPanel::RenderContent");
    if (markdown == last_rendered_content_)
    {
        return; // No change
    }

    // Save scroll position
    int scroll_x = 0;
    int scroll_y = 0;
    if (html_view_ != nullptr)
    {
        html_view_->GetViewStart(&scroll_x, &scroll_y);
    }

    // Pre-process footnotes (Improvement 12: reuse member)
    auto footnote_result = footnote_proc_.process(markdown);

    // Parse (using footnote-processed markdown)
    auto doc_result = parser_.parse(footnote_result.processed_markdown);
    if (!doc_result.has_value())
    {
        DisplayError(doc_result.error());
        return;
    }

    // Improvement 11: configure reused renderer member
    auto mermaid = core::ServiceRegistry::instance().get<core::IMermaidRenderer>();
    if (mermaid)
    {
        renderer_.set_mermaid_renderer(mermaid.get());
    }

    // Set base path for local image resolution
    if (!base_path_.empty())
    {
        renderer_.set_base_path(base_path_);
    }

    // Render with footnotes
    std::string body_html;
    if (footnote_result.has_footnotes)
    {
        body_html =
            renderer_.render_with_footnotes(*doc_result, footnote_result.footnote_section_html);
    }
    else
    {
        body_html = renderer_.render(*doc_result);
    }

    // Sanitize HTML output (defense-in-depth)
    auto safe_html = sanitizer_.sanitize(body_html);

    // Improvement 25: cache for DisplayError reuse
    last_rendered_html_ = safe_html;

    // Generate full HTML and display
    auto full_html = GenerateFullHtml(safe_html);
    if (html_view_ != nullptr)
    {
        // Freeze to avoid flicker during content replacement
        html_view_->Freeze();
        html_view_->SetPage(full_html);

        // Restore scroll position
        html_view_->Scroll(scroll_x, scroll_y);
        html_view_->Thaw();
    }

    last_rendered_content_ = markdown;
}

void PreviewPanel::DisplayError(const std::string& error_message)
{
    auto error_html = fmt::format(R"(<div class="error-overlay">⚠ Markdown parse error: {}</div>)",
                                  error_message);

    // Show error overlay but keep last successful content if available
    // Improvement 25: use cached HTML body instead of re-parsing
    std::string body = error_html + last_rendered_html_;

    auto full_html = GenerateFullHtml(body);
    if (html_view_ != nullptr)
    {
        html_view_->SetPage(full_html);
    }

    spdlog::warn("Markdown parse error: {}", error_message);
}

// ═══════════════════════════════════════════════════════
// Event handlers
// ═══════════════════════════════════════════════════════

void PreviewPanel::OnRenderTimer(wxTimerEvent& /*event*/)
{
    if (!pending_content_.empty())
    {
        RenderContent(pending_content_);
        pending_content_.clear();
    }
}

void PreviewPanel::OnLinkClicked(wxHtmlLinkEvent& event)
{
    auto href = event.GetLinkInfo().GetHref().ToStdString();

    // Phase 6C: Copy code block to clipboard
    if (href.starts_with("markamp://copy/"))
    {
        auto block_id_str = href.substr(15); // after "markamp://copy/"
        int block_id = std::stoi(block_id_str);
        auto source = renderer_.code_renderer().get_block_source(block_id);
        if (!source.empty())
        {
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(source));
                wxTheClipboard->Close();
            }
        }
        return;
    }

    // External link: open in system browser
    if (href.starts_with("http://") || href.starts_with("https://"))
    {
        wxLaunchDefaultBrowser(href);
        return;
    }

    // Anchor link: scroll to heading (wxHtmlWindow handles this natively)
    if (href.starts_with("#"))
    {
        if (html_view_ != nullptr)
        {
            html_view_->LoadPage(href);
        }
        return;
    }

    // Relative file link: publish ActiveFileChangedEvent
    if (href.ends_with(".md") || href.ends_with(".markdown"))
    {
        core::events::ActiveFileChangedEvent evt;
        evt.file_id = href;
        event_bus_.publish(evt);
        return;
    }

    // Default: let wxHtmlWindow handle it
    event.Skip();
}

void PreviewPanel::OnSize(wxSizeEvent& event)
{
    event.Skip();

    // Bevel overlay disabled — it blocks the preview on macOS
    // (bevel_overlay_ is hidden, no repositioning needed)

    // Improvement 24: debounce content re-render during resize drag
    if (!last_rendered_content_.empty())
    {
        resize_timer_.Start(kResizeDebounceMs, wxTIMER_ONE_SHOT);
    }
}

void PreviewPanel::OnResizeTimer(wxTimerEvent& /*event*/)
{
    if (!last_rendered_content_.empty())
    {
        auto content = last_rendered_content_;
        last_rendered_content_.clear(); // Force re-render
        RenderContent(content);
    }
}

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void PreviewPanel::OnThemeChanged(const core::Theme& new_theme)
{
    auto bg = new_theme.colors.bg_app.to_wx_colour();
    SetBackgroundColour(bg);
    if (html_view_ != nullptr)
    {
        html_view_->SetBackgroundColour(bg);
    }

    // Invalidate cached CSS on theme change
    cached_css_.clear();

    // Re-render with new theme CSS (immediate, not debounced)
    if (!last_rendered_content_.empty())
    {
        auto content = last_rendered_content_;
        last_rendered_content_.clear(); // Force re-render
        RenderContent(content);
    }
}

// ═══════════════════════════════════════════════════════
// Zoom support
// ═══════════════════════════════════════════════════════

void PreviewPanel::SetZoomLevel(int level)
{
    if (zoom_level_ == level)
    {
        return;
    }
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
        // Ctrl/Cmd + Wheel for Zoom
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

    // Item 12: Scroll To Top (Home / Cmd+Up)
    if (key == WXK_HOME || (cmd && key == WXK_UP))
    {
        ScrollToTop();
        return;
    }

    // Scroll To Bottom (End / Cmd+Down)
    if (key == WXK_END || (cmd && key == WXK_DOWN))
    {
        if (html_view_ != nullptr)
        {
            int x = 0, y = 0;
            html_view_->GetVirtualSize(&x, &y);
            html_view_->Scroll(0, y);
        }
        return;
    }

    event.Skip();
}

// ═══════════════════════════════════════════════════════
// Scroll synchronization
// ═══════════════════════════════════════════════════════

void PreviewPanel::SetScrollFraction(double fraction)
{
    if (html_view_ == nullptr)
    {
        return;
    }

    // Clamp fraction to [0, 1]
    fraction = std::max(0.0, std::min(1.0, fraction));

    // Get the virtual height of the rendered content
    int virt_width = 0;
    int virt_height = 0;
    html_view_->GetVirtualSize(&virt_width, &virt_height);

    int client_height = html_view_->GetClientSize().GetHeight();
    int scroll_range = std::max(1, virt_height - client_height);
    int scroll_pos = static_cast<int>(fraction * scroll_range);

    int ppu_x = 1;
    int ppu_y = 1;
    html_view_->GetScrollPixelsPerUnit(&ppu_x, &ppu_y);
    if (ppu_y > 0)
    {
        html_view_->Scroll(0, scroll_pos / ppu_y);
    }
}

void PreviewPanel::OnScrollSyncTimer(wxTimerEvent& /*event*/)
{
    SetScrollFraction(pending_scroll_fraction_);
}

void PreviewPanel::set_scroll_sync_enabled(bool enabled)
{
    scroll_sync_enabled_ = enabled;
}

// ═══════════════════════════════════════════════════════
// Export
// ═══════════════════════════════════════════════════════

auto PreviewPanel::ExportHtml(const std::filesystem::path& output_path) const -> bool
{
    if (last_rendered_content_.empty())
    {
        return false;
    }

    // Re-render to get the full HTML
    rendering::FootnotePreprocessor footnote_proc;
    auto footnote_result = footnote_proc.process(last_rendered_content_);

    core::MarkdownParser parser;
    auto doc_result = parser.parse(footnote_result.processed_markdown);
    if (!doc_result.has_value())
    {
        return false;
    }

    rendering::HtmlRenderer renderer;
    if (!base_path_.empty())
    {
        renderer.set_base_path(base_path_);
    }

    std::string body_html;
    if (footnote_result.has_footnotes)
    {
        body_html =
            renderer.render_with_footnotes(*doc_result, footnote_result.footnote_section_html);
    }
    else
    {
        body_html = renderer.render(*doc_result);
    }

    auto full_html = GenerateFullHtml(body_html);

    std::ofstream file(output_path);
    if (!file.is_open())
    {
        return false;
    }
    file << full_html;
    return file.good();
}

// ═══════════════════════════════════════════════════════
// Base path
// ═══════════════════════════════════════════════════════

void PreviewPanel::set_base_path(const std::filesystem::path& base_path)
{
    base_path_ = base_path;
}

} // namespace markamp::ui
