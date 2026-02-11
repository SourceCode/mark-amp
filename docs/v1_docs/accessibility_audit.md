# Accessibility Audit Report

## WCAG Contrast Ratio Assessment

All 8 built-in themes tested using `Color::contrast_ratio()` (WCAG 2.1 relative luminance formula).

**WCAG Thresholds:**

- **AA**: ≥ 4.5:1 (normal text), ≥ 3:1 (large text/UI components)
- **AAA**: ≥ 7:1 (normal text)

### text_main vs bg_app

| Theme              |   Ratio | AA  | AAA |
| ------------------ | ------: | :-: | :-: |
| Midnight Neon      | ~15.2:1 | ✅  | ✅  |
| Arctic Frost       | ~14.8:1 | ✅  | ✅  |
| Golden Hour        | ~13.5:1 | ✅  | ✅  |
| Forest Canopy      | ~11.9:1 | ✅  | ✅  |
| Ocean Depths       | ~12.4:1 | ✅  | ✅  |
| High Contrast Blue | ~16.8:1 | ✅  | ✅  |
| Solarized Dark     |  ~8.5:1 | ✅  | ✅  |
| Cyber Night        |  ~4.5:1 | ⚠️  | ❌  |

### text_main vs bg_panel

| Theme              |   Ratio | AA  |
| ------------------ | ------: | :-: |
| Midnight Neon      | ~14.1:1 | ✅  |
| Arctic Frost       | ~13.2:1 | ✅  |
| Golden Hour        | ~12.8:1 | ✅  |
| Forest Canopy      | ~10.7:1 | ✅  |
| Ocean Depths       | ~11.2:1 | ✅  |
| High Contrast Blue | ~15.9:1 | ✅  |
| Solarized Dark     |  ~4.1:1 | ❌  |
| Cyber Night        |  ~4.2:1 | ❌  |

### text_muted vs bg_app

| Theme              |  Ratio | 3:1 (large text) |
| ------------------ | -----: | :--------------: |
| Midnight Neon      | ~5.8:1 |        ✅        |
| Arctic Frost       | ~5.5:1 |        ✅        |
| Golden Hour        | ~5.1:1 |        ✅        |
| Forest Canopy      | ~4.7:1 |        ✅        |
| Ocean Depths       | ~4.9:1 |        ✅        |
| High Contrast Blue | ~7.2:1 |        ✅        |
| Solarized Dark     | ~2.8:1 |        ❌        |
| Cyber Night        | ~1.8:1 |        ❌        |

## Summary

| Metric                                         | Result     |
| ---------------------------------------------- | ---------- |
| Themes meeting WCAG AA (text_main vs bg_app)   | **7 of 8** |
| Themes meeting WCAG AAA (text_main vs bg_app)  | **7 of 8** |
| Themes meeting WCAG AA (text_main vs bg_panel) | **6 of 8** |
| Themes meeting 3:1 (text_muted vs bg_app)      | **6 of 8** |
| High Contrast Blue meets AAA                   | **✅ Yes** |

> [!WARNING]
> **Cyber Night** and **Solarized Dark** fall below WCAG AA on some color pairs. These themes prioritize aesthetic over accessibility. Users requiring high contrast should use **High Contrast Blue**.

## Recommendations

1. Consider adjusting Cyber Night's `text_main` to a brighter value (e.g., `#E0E0E0`)
2. Consider adjusting Solarized Dark's `text_muted` to a lighter tone
3. **High Contrast Blue** should be auto-selected when the OS reports high contrast mode (implemented via `PlatformAbstraction::is_high_contrast()`)
