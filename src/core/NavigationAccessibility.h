/// @file NavigationAccessibility.h
/// @brief V9 Phase 17 Task 17 — Screen reader announcements for navigation.
///
/// Header-only, pure logic. Generates descriptive text strings for
/// assistive technology when navigation events occur.

#pragma once

#include "SurfaceLink.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides screen reader announcements for navigation operations.
///
/// All methods return descriptive text suitable for ARIA live regions
/// or platform accessibility APIs.
class NavigationAccessibility
{
public:
    NavigationAccessibility() = default;

    /// Announce a navigation from one location to another.
    [[nodiscard]] static auto announce_navigation(SurfaceKind from_surface,
                                                  const std::string& from_doc,
                                                  SurfaceKind to_surface,
                                                  const std::string& to_doc) -> std::string
    {
        std::string announcement = "Navigated from ";
        announcement += std::string(SurfaceLinkRouter::surface_name(from_surface));

        if (!from_doc.empty())
        {
            announcement += " " + extract_filename(from_doc);
        }

        announcement += " to ";
        announcement += std::string(SurfaceLinkRouter::surface_name(to_surface));

        if (!to_doc.empty())
        {
            announcement += " " + extract_filename(to_doc);
        }

        return announcement;
    }

    /// Announce a back/forward navigation.
    [[nodiscard]] static auto announce_back_forward(bool is_forward,
                                                    const std::string& destination_doc)
        -> std::string
    {
        std::string announcement = is_forward ? "Forward to " : "Back to ";

        if (!destination_doc.empty())
        {
            announcement += extract_filename(destination_doc);
        }
        else
        {
            announcement += "previous location";
        }

        return announcement;
    }

    /// Announce a broken link detection.
    [[nodiscard]] static auto announce_broken_link(const std::string& link_text,
                                                   const std::string& reason) -> std::string
    {
        std::string announcement = "Broken link: " + link_text;
        if (!reason.empty())
        {
            announcement += ". " + reason;
        }
        return announcement;
    }

    /// Announce the number of links in a document.
    [[nodiscard]] static auto announce_link_count(int total_links, int broken_links) -> std::string
    {
        std::string announcement;
        announcement += std::to_string(total_links);
        announcement += (total_links == 1) ? " link" : " links";

        if (broken_links > 0)
        {
            announcement += ", " + std::to_string(broken_links) + " broken";
        }

        return announcement;
    }

    /// Describe a breadcrumb trail for screen readers.
    [[nodiscard]] static auto
    describe_breadcrumb_trail(const std::vector<std::string>& trail_labels) -> std::string
    {
        if (trail_labels.empty())
        {
            return "No breadcrumb trail";
        }

        std::string description = "Breadcrumb trail: ";
        for (std::size_t idx = 0; idx < trail_labels.size(); ++idx)
        {
            if (idx > 0)
            {
                description += " then ";
            }
            description += trail_labels[idx];
        }

        description += ". " + std::to_string(trail_labels.size());
        description += (trail_labels.size() == 1) ? " step" : " steps";
        description += " deep";

        return description;
    }

    /// Announce a heading jump.
    [[nodiscard]] static auto announce_heading_jump(const std::string& heading_text,
                                                    int heading_level) -> std::string
    {
        std::string announcement = "Jumped to heading level ";
        announcement += std::to_string(heading_level);
        announcement += ": " + heading_text;
        return announcement;
    }

    /// Announce a block reference navigation.
    [[nodiscard]] static auto announce_block_ref(const std::string& block_id,
                                                 const std::string& document) -> std::string
    {
        std::string announcement = "Navigated to block reference " + block_id;
        if (!document.empty())
        {
            announcement += " in " + extract_filename(document);
        }
        return announcement;
    }

    /// Announce a deep link copy.
    [[nodiscard]] static auto announce_deep_link_copied(const std::string& uri) -> std::string
    {
        return "Deep link copied: " + uri;
    }

private:
    /// Extract filename from path.
    [[nodiscard]] static auto extract_filename(const std::string& path) -> std::string
    {
        auto slash_pos = path.rfind('/');
        if (slash_pos != std::string::npos)
        {
            return path.substr(slash_pos + 1);
        }
        return path;
    }
};

} // namespace markamp::core
