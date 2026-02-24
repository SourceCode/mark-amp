#include "SidebarSkeletonPlaceholder.h"

namespace markamp::ui
{

SidebarSkeletonPlaceholder::SidebarSkeletonPlaceholder(wxWindow* parent,
                                                       core::ThemeEngine& theme_engine,
                                                       Style style)
    : wxPanel(parent, wxID_ANY)
    , loader_(new animation::SkeletonLoader(this, theme_engine))
    , style_(style)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(loader_, 1, wxEXPAND | wxALL, 0);
    SetSizer(sizer);

    Bind(wxEVT_SIZE, &SidebarSkeletonPlaceholder::OnSize, this);
}

void SidebarSkeletonPlaceholder::ShowAndAnimate()
{
    GenerateBlocks();
    loader_->start_animation();
    Show();
}

void SidebarSkeletonPlaceholder::HideAndStop()
{
    loader_->stop_animation();
    Hide();
}

void SidebarSkeletonPlaceholder::OnSize(wxSizeEvent& event)
{
    event.Skip();
    if (IsShown())
    {
        GenerateBlocks();
    }
}

void SidebarSkeletonPlaceholder::GenerateBlocks()
{
    if (loader_ == nullptr)
    {
        return;
    }

    loader_->clear_blocks();

    // We get the size of the loader window itself
    const auto kExtents = loader_->GetSize();
    if (kExtents.GetWidth() == 0 || kExtents.GetHeight() == 0)
    {
        return;
    }

    if (style_ == Style::kList)
    {
        // Draw tree-like lines
        int pos_y = 10;
        const int kRowHeight = 24;
        while (pos_y < kExtents.GetHeight())
        {
            // Indent pattern (just simple varying indents)
            const int kIndent = (pos_y % 100 < 50) ? 16 : 32;

            // Icon block
            loader_->add_block(wxRect(kIndent, pos_y + 4, 16, 16), 2);
            // Text block (varying width)
            int text_width = kExtents.GetWidth() - kIndent - 40 - (pos_y % 40);
            if (text_width < 50)
            {
                text_width = 50;
            }
            loader_->add_block(wxRect(kIndent + 24, pos_y + 6, text_width, 12), 2);

            pos_y += kRowHeight;
        }
    }
    else if (style_ == Style::kCards)
    {
        // Draw extension-like cards
        int pos_y = 10;
        const int kCardHeight = 80;
        const int kPadding = 10;

        while (pos_y < kExtents.GetHeight())
        {
            // Icon
            loader_->add_block(wxRect(kPadding, pos_y + kPadding, 50, 50), 4);
            // Title
            loader_->add_block(wxRect(kPadding + 60, pos_y + kPadding, 120, 16), 2);
            // Publisher
            loader_->add_block(wxRect(kPadding + 60, pos_y + kPadding + 20, 80, 12), 2);
            // Description (two lines)
            int desc_width = kExtents.GetWidth() - (kPadding * 2) - 60;
            if (desc_width < 50)
            {
                desc_width = 50;
            }
            loader_->add_block(wxRect(kPadding + 60, pos_y + kPadding + 36, desc_width, 10), 2);
            loader_->add_block(
                wxRect(
                    kPadding + 60, pos_y + kPadding + 50, static_cast<int>(desc_width * 0.7), 10),
                2);

            pos_y += kCardHeight;
        }
    }
}

} // namespace markamp::ui
