#include "BuiltinThemes.h"

namespace markamp::core
{

static auto make_builtin_themes() -> std::vector<Theme>
{
    std::vector<Theme> themes;
    themes.reserve(8);

    // 1. Midnight Neon
    themes.push_back(Theme{
        .id = "midnight-neon",
        .name = "Midnight Neon",
        .colors =
            {
                .bg_app = Color(0x05, 0x05, 0x10),
                .bg_panel = Color(0x0a, 0x0a, 0x1f),
                .bg_header = Color(0x0f, 0x0f, 0x2d),
                .bg_input = Color(0x05, 0x05, 0x10),
                .text_main = Color(0x00, 0xff, 0xea),
                .text_muted = Color(0x5e, 0x6a, 0xd2),
                .accent_primary = Color(0xff, 0x00, 0x55),
                .accent_secondary = Color(0x00, 0xff, 0xea),
                .border_light = Color(255, 0, 85, 51), // rgba(255, 0, 85, 0.2)
                .border_dark = Color(0x00, 0x00, 0x00),
                // Editor
                .editor_bg = Color(0x05, 0x05, 0x10),
                .editor_fg = Color(0x00, 0xff, 0xea),
                .editor_selection = Color(255, 0, 85, 51),
                .editor_line_number = Color(0x5e, 0x6a, 0xd2),
                .editor_cursor = Color(0xff, 0x00, 0x55),
                .editor_gutter = Color(0x05, 0x05, 0x10),
                // UI
                .list_hover = Color(255, 0, 85, 25), // ~10%
                .list_selected = Color(255, 0, 85, 51),
                .scrollbar_thumb = Color(0x00, 0xff, 0xea),
                .scrollbar_track = Color(0x0a, 0x0a, 0x1f),
            },
    });

    // 2. Cyber Night
    themes.push_back(Theme{
        .id = "cyber-night",
        .name = "Cyber Night",
        .colors =
            {
                .bg_app = Color(0x1a, 0x1a, 0x2e),
                .bg_panel = Color(0x16, 0x21, 0x3e),
                .bg_header = Color(0x0f, 0x34, 0x60),
                .bg_input = Color(0x0f, 0x34, 0x60),
                .text_main = Color(0xe9, 0x45, 0x60),
                .text_muted = Color(0x53, 0x34, 0x83),
                .accent_primary = Color(0xe9, 0x45, 0x60),
                .accent_secondary = Color(0x53, 0x34, 0x83),
                .border_light = Color(255, 255, 255, 26), // rgba(255, 255, 255, 0.1)
                .border_dark = Color(0, 0, 0, 128),       // rgba(0, 0, 0, 0.5)
                // Editor
                .editor_bg = Color(0x0f, 0x34, 0x60),
                .editor_fg = Color(0xe9, 0x45, 0x60),
                .editor_selection = Color(233, 69, 96, 51),
                .editor_line_number = Color(0x53, 0x34, 0x83),
                .editor_cursor = Color(0xe9, 0x45, 0x60),
                .editor_gutter = Color(0x0f, 0x34, 0x60),
                // UI
                .list_hover = Color(233, 69, 96, 25),
                .list_selected = Color(233, 69, 96, 51),
                .scrollbar_thumb = Color(0x53, 0x34, 0x83),
                .scrollbar_track = Color(0x16, 0x21, 0x3e),
            },
    });

    // 3. Solarized Dark
    themes.push_back(Theme{
        .id = "solarized-dark",
        .name = "Solarized Dark",
        .colors =
            {
                .bg_app = Color(0x00, 0x2b, 0x36),
                .bg_panel = Color(0x07, 0x36, 0x42),
                .bg_header = Color(0x00, 0x2b, 0x36),
                .bg_input = Color(0x00, 0x2b, 0x36),
                .text_main = Color(0x83, 0x94, 0x96),
                .text_muted = Color(0x58, 0x6e, 0x75),
                .accent_primary = Color(0xb5, 0x89, 0x00),
                .accent_secondary = Color(0x2a, 0xa1, 0x98),
                .border_light = Color(0x58, 0x6e, 0x75),
                .border_dark = Color(0x00, 0x1e, 0x26),
                // Editor
                .editor_bg = Color(0x00, 0x2b, 0x36),
                .editor_fg = Color(0x83, 0x94, 0x96),
                .editor_selection = Color(0x07, 0x36, 0x42), // distinct selection in solarized
                .editor_line_number = Color(0x58, 0x6e, 0x75),
                .editor_cursor = Color(0xb5, 0x89, 0x00),
                .editor_gutter = Color(0x00, 0x2b, 0x36),
                // UI
                .list_hover = Color(0x07, 0x36, 0x42),
                .list_selected = Color(0x58, 0x6e, 0x75),
                .scrollbar_thumb = Color(0x2a, 0xa1, 0x98),
                .scrollbar_track = Color(0x07, 0x36, 0x42),
            },
    });

    // 4. Classic Mono (light theme)
    themes.push_back(Theme{
        .id = "classic-mono",
        .name = "Classic Mono",
        .colors =
            {
                .bg_app = Color(0xff, 0xff, 0xff),
                .bg_panel = Color(0xf3, 0xf3, 0xf3),
                .bg_header = Color(0xe5, 0xe5, 0xe5),
                .bg_input = Color(0xff, 0xff, 0xff),
                .text_main = Color(0x11, 0x11, 0x11),
                .text_muted = Color(0x66, 0x66, 0x66),
                .accent_primary = Color(0x00, 0x00, 0x00),
                .accent_secondary = Color(0x44, 0x44, 0x44),
                .border_light = Color(0xcc, 0xcc, 0xcc),
                .border_dark = Color(0x99, 0x99, 0x99),
                // Editor
                .editor_bg = Color(0xff, 0xff, 0xff),
                .editor_fg = Color(0x11, 0x11, 0x11),
                .editor_selection = Color(0xcc, 0xcc, 0xcc),
                .editor_line_number = Color(0x99, 0x99, 0x99),
                .editor_cursor = Color(0x00, 0x00, 0x00),
                .editor_gutter = Color(0xf3, 0xf3, 0xf3),
                // UI
                .list_hover = Color(0xe5, 0xe5, 0xe5),
                .list_selected = Color(0xcc, 0xcc, 0xcc),
                .scrollbar_thumb = Color(0x99, 0x99, 0x99),
                .scrollbar_track = Color(0xf3, 0xf3, 0xf3),
            },
    });

    // 5. High Contrast Blue
    themes.push_back(Theme{
        .id = "high-contrast-blue",
        .name = "High Contrast Blue",
        .colors =
            {
                .bg_app = Color(0x00, 0x00, 0x80),
                .bg_panel = Color(0x00, 0x00, 0xa0),
                .bg_header = Color(0x00, 0x00, 0x80),
                .bg_input = Color(0x00, 0x00, 0x00),
                .text_main = Color(0xff, 0xff, 0xff),
                .text_muted = Color(0x00, 0xff, 0xff),
                .accent_primary = Color(0xff, 0xff, 0x00),
                .accent_secondary = Color(0x00, 0xff, 0x00),
                .border_light = Color(0xff, 0xff, 0xff),
                .border_dark = Color(0x00, 0x00, 0x40),
                // Editor
                .editor_bg = Color(0x00, 0x00, 0x00),
                .editor_fg = Color(0xff, 0xff, 0xff),
                .editor_selection = Color(0x00, 0x00, 0x80),
                .editor_line_number = Color(0x00, 0xff, 0xff),
                .editor_cursor = Color(0xff, 0xff, 0x00),
                .editor_gutter = Color(0x00, 0x00, 0x00),
                // UI
                .list_hover = Color(0x00, 0x00, 0x80),
                .list_selected = Color(0x00, 0x00, 0xa0),
                .scrollbar_thumb = Color(0x00, 0xff, 0xff),
                .scrollbar_track = Color(0x00, 0x00, 0x80),
            },
    });

    // 6. Matrix Core
    themes.push_back(Theme{
        .id = "matrix-core",
        .name = "Matrix Core",
        .colors =
            {
                .bg_app = Color(0x0d, 0x11, 0x17),
                .bg_panel = Color(0x00, 0x00, 0x00),
                .bg_header = Color(0x16, 0x1b, 0x22),
                .bg_input = Color(0x0d, 0x11, 0x17),
                .text_main = Color(0x00, 0xff, 0x41),
                .text_muted = Color(0x00, 0x8f, 0x11),
                .accent_primary = Color(0x00, 0xff, 0x41),
                .accent_secondary = Color(0x00, 0x3b, 0x00),
                .border_light = Color(0x22, 0x22, 0x22),
                .border_dark = Color(0x00, 0x00, 0x00),
                // Editor
                .editor_bg = Color(0x0d, 0x11, 0x17),
                .editor_fg = Color(0x00, 0xff, 0x41),
                .editor_selection = Color(0x00, 0x3b, 0x00),
                .editor_line_number = Color(0x00, 0x8f, 0x11),
                .editor_cursor = Color(0x00, 0xff, 0x41),
                .editor_gutter = Color(0x0d, 0x11, 0x17),
                // UI
                .list_hover = Color(0x16, 0x1b, 0x22),
                .list_selected = Color(0x22, 0x22, 0x22),
                .scrollbar_thumb = Color(0x00, 0x3b, 0x00),
                .scrollbar_track = Color(0x00, 0x00, 0x00),
            },
    });

    // 7. Classic Amp (Winamp-inspired)
    themes.push_back(Theme{
        .id = "winamp-classic",
        .name = "Classic Amp",
        .colors =
            {
                .bg_app = Color(0x28, 0x28, 0x28),
                .bg_panel = Color(0x19, 0x19, 0x19),
                .bg_header = Color(0x33, 0x33, 0x33),
                .bg_input = Color(0x00, 0x00, 0x00),
                .text_main = Color(0x00, 0xFF, 0x00),
                .text_muted = Color(0xaa, 0xaa, 0xaa),
                .accent_primary = Color(0xd4, 0xaf, 0x37),
                .accent_secondary = Color(0x88, 0x88, 0x88),
                .border_light = Color(0x55, 0x55, 0x55),
                .border_dark = Color(0x11, 0x11, 0x11),
                // Editor
                .editor_bg = Color(0x00, 0x00, 0x00),
                .editor_fg = Color(0x00, 0xFF, 0x00),
                .editor_selection = Color(0x33, 0x33, 0x33),
                .editor_line_number = Color(0xaa, 0xaa, 0xaa),
                .editor_cursor = Color(0xd4, 0xaf, 0x37),
                .editor_gutter = Color(0x00, 0x00, 0x00),
                // UI
                .list_hover = Color(0x33, 0x33, 0x33),
                .list_selected = Color(0x44, 0x44, 0x44),
                .scrollbar_thumb = Color(0x88, 0x88, 0x88),
                .scrollbar_track = Color(0x19, 0x19, 0x19),
            },
    });

    // 8. Vapor Wave
    themes.push_back(Theme{
        .id = "vapor-wave",
        .name = "Vapor Wave",
        .colors =
            {
                .bg_app = Color(0x2b, 0x10, 0x55),
                .bg_panel = Color(0x3d, 0x2c, 0x8d),
                .bg_header = Color(0x75, 0x62, 0xe0),
                .bg_input = Color(0x1c, 0x0c, 0x3f),
                .text_main = Color(0x00, 0xff, 0xff),
                .text_muted = Color(0xb8, 0xc0, 0xff),
                .accent_primary = Color(0xff, 0x00, 0xff),
                .accent_secondary = Color(0x91, 0x6b, 0xbf),
                .border_light = Color(255, 255, 255, 51), // rgba(255, 255, 255, 0.2)
                .border_dark = Color(0, 0, 0, 153),       // rgba(0, 0, 0, 0.6)
                // Editor
                .editor_bg = Color(0x2b, 0x10, 0x55),
                .editor_fg = Color(0x00, 0xff, 0xff),
                .editor_selection = Color(117, 98, 224, 100),
                .editor_line_number = Color(0xb8, 0xc0, 0xff),
                .editor_cursor = Color(0xff, 0x00, 0xff),
                .editor_gutter = Color(0x2b, 0x10, 0x55),
                // UI
                .list_hover = Color(117, 98, 224, 60),
                .list_selected = Color(117, 98, 224, 100),
                .scrollbar_thumb = Color(0x91, 0x6b, 0xbf),
                .scrollbar_track = Color(0x3d, 0x2c, 0x8d),
            },
    });

    return themes;
}

auto get_builtin_themes() -> const std::vector<Theme>&
{
    static const auto themes = make_builtin_themes();
    return themes;
}

auto get_default_theme() -> const Theme&
{
    return get_builtin_themes().front(); // Midnight Neon
}

} // namespace markamp::core
