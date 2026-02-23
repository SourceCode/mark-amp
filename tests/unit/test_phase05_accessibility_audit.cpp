#include "core/BuiltInThemes.h"
#include "core/ThemeEngine.h"
#include "ui/ComponentSizeResolver.h"
#include "ui/accessibility/AccessibilityAuditor.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::ui;
using namespace markamp::ui::accessibility;

TEST_CASE(
    "Phase 05 Task 22: Component Size Resolver Output Validates Hit Targets against WCAG 2.5.5",
    "[accessibility][phase05]")
{
    auto& resolver = ComponentSizeResolver::get();

    SECTION("Compact Profile returns metrics >= 24px where applicable")
    {
        resolver.set_density(DensityProfile::kCompact);

        // Buttons must be at least 24
        auto btn_metrics = resolver.resolve(ComponentKind::kButton);
        REQUIRE(btn_metrics.height >= 24);
        REQUIRE(btn_metrics.min_width >= 24);

        // Toolbar Buttons must be at least 24
        auto tb_metrics = resolver.resolve(ComponentKind::kToolbarButton);
        REQUIRE(tb_metrics.height >= 24);
        REQUIRE(tb_metrics.min_width >= 24);

        // Items with min_width == 0 (like TreeRow) still have height >= 24
        auto tree_metrics = resolver.resolve(ComponentKind::kTreeRow);
        REQUIRE(tree_metrics.height >= 24);
        REQUIRE(tree_metrics.min_width == 0); // Unconstrained
    }

    SECTION("Default Profile returns metrics >= 24px")
    {
        resolver.set_density(DensityProfile::kDefault);
        auto btn_metrics = resolver.resolve(ComponentKind::kButton);
        REQUIRE(btn_metrics.height >= 24);
        REQUIRE(btn_metrics.min_width >= 24);
    }

    SECTION("Comfortable Profile returns metrics >= 24px")
    {
        resolver.set_density(DensityProfile::kComfortable);
        auto btn_metrics = resolver.resolve(ComponentKind::kButton);
        REQUIRE(btn_metrics.height >= 24);
        REQUIRE(btn_metrics.min_width >= 24);
    }
}

TEST_CASE("Phase 05 Task 24: Default Themes Pass Accessibility Contrast Audits",
          "[accessibility][phase05]")
{
    SECTION("MarkAmp High Contrast Blue Theme complies with 100% AAA contrast")
    {
        const auto& themes = core::get_builtin_themes();
        auto it = std::find_if(themes.begin(),
                               themes.end(),
                               [](const core::Theme& t) { return t.id == "high-contrast-blue"; });

        REQUIRE(it != themes.end());

        auto report = AccessibilityAuditor::run_contrast_audit(*it);

        // We expect HC Blue to have exactly 0 failures, achieving 100% compliance.
        if (report.failures > 0)
        {
            for (const auto& f : report.failure_details)
            {
                UNSCOPED_INFO("Contrast Failure HC - Fg: " << f.foreground_token
                                                           << ", Bg: " << f.background_token
                                                           << ", Ratio: " << f.ratio);
            }
        }

        REQUIRE(report.failures == 0);
        REQUIRE(report.is_fully_aa_compliant() == true);
    }

    SECTION("MarkAmp Default Dark Theme complies with AA contrast")
    {
        const auto& default_theme = core::get_default_theme();
        auto report = AccessibilityAuditor::run_contrast_audit(default_theme);

        if (report.failures > 0)
        {
            for (const auto& f : report.failure_details)
            {
                UNSCOPED_INFO("Contrast Failure Default - Fg: " << f.foreground_token
                                                                << ", Bg: " << f.background_token
                                                                << ", Ratio: " << f.ratio);
            }
        }

        // Depending on specific shade tweaks, it should ideally have 0 failures for core UI
        // We ensure `failures` is 0 to ensure our color palette is fully accessible.
        REQUIRE(report.failures == 0);
        REQUIRE(report.is_fully_aa_compliant() == true);
    }
}
