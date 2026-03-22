#include "core/DesignSystemAudit.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DesignSystemAudit registers components", "[v22][audit]")
{
    DesignSystemAudit audit;
    REQUIRE(audit.component_count() == 0);

    DesignSystemAudit::ComponentEntry entry;
    entry.name = "TestButton";
    entry.row_height = 32;
    entry.icon_size = 16;
    entry.padding_h = 8;
    entry.padding_v = 4;
    audit.register_component(entry);

    REQUIRE(audit.component_count() == 1);
}

TEST_CASE("DesignSystemAudit spacing consistency", "[v22][audit]")
{
    DesignSystemAudit audit;

    SECTION("Valid spacing produces no findings")
    {
        DesignSystemAudit::ComponentEntry entry;
        entry.name = "GoodComponent";
        entry.row_height = 32;  // Multiple of 4
        entry.padding_h = 8;    // Valid spacing token
        entry.padding_v = 4;    // Valid spacing token
        audit.register_component(entry);

        auto findings = audit.audit_spacing_consistency();
        REQUIRE(findings.empty());
    }

    SECTION("Non-grid row height produces warning")
    {
        DesignSystemAudit::ComponentEntry entry;
        entry.name = "BadRowHeight";
        entry.row_height = 25;  // Not a multiple of 4
        entry.padding_h = 8;
        entry.padding_v = 4;
        audit.register_component(entry);

        auto findings = audit.audit_spacing_consistency();
        REQUIRE(findings.size() >= 1);
        REQUIRE(findings[0].severity == DesignSystemAudit::Severity::kWarning);
    }

    SECTION("Non-standard padding produces warning")
    {
        DesignSystemAudit::ComponentEntry entry;
        entry.name = "BadPadding";
        entry.row_height = 32;
        entry.padding_h = 7;  // Not a valid spacing token
        entry.padding_v = 4;
        audit.register_component(entry);

        auto findings = audit.audit_spacing_consistency();
        REQUIRE(findings.size() >= 1);
    }
}

TEST_CASE("DesignSystemAudit corner radius consistency", "[v22][audit]")
{
    DesignSystemAudit audit;

    DesignSystemAudit::ComponentEntry entry;
    entry.name = "BorderedNonRounded";
    entry.border = BorderWeightToken::kThin;
    entry.corner = CornerRadiusToken::kNone;
    audit.register_component(entry);

    auto findings = audit.audit_corner_radius_consistency();
    REQUIRE(findings.size() >= 1);
    REQUIRE(findings[0].severity == DesignSystemAudit::Severity::kInfo);
}

TEST_CASE("DesignSystemAudit icon metric consistency", "[v22][audit]")
{
    DesignSystemAudit audit;

    SECTION("Standard icon size produces no findings")
    {
        DesignSystemAudit::ComponentEntry entry;
        entry.name = "GoodIcon";
        entry.icon_size = 16;
        audit.register_component(entry);

        auto findings = audit.audit_icon_metric_consistency();
        REQUIRE(findings.empty());
    }

    SECTION("Non-standard icon size produces warning")
    {
        DesignSystemAudit::ComponentEntry entry;
        entry.name = "BadIcon";
        entry.icon_size = 18;  // Not a standard size
        audit.register_component(entry);

        auto findings = audit.audit_icon_metric_consistency();
        REQUIRE(findings.size() >= 1);
    }
}

TEST_CASE("DesignSystemAudit audit_all combines all checks", "[v22][audit]")
{
    DesignSystemAudit audit;

    // Register a component with multiple issues
    DesignSystemAudit::ComponentEntry entry;
    entry.name = "ProblematicComponent";
    entry.row_height = 25;  // Bad grid alignment
    entry.padding_h = 7;    // Bad spacing token
    entry.icon_size = 18;   // Bad icon size
    audit.register_component(entry);

    auto findings = audit.audit_all();
    REQUIRE(findings.size() >= 3);
    REQUIRE(audit.last_finding_count() == findings.size());
}

TEST_CASE("DesignSystemAudit generates component atlas", "[v22][audit]")
{
    DesignSystemAudit audit;

    DesignSystemAudit::ComponentEntry e1;
    e1.name = "Button";
    e1.row_height = 32;
    audit.register_component(e1);

    DesignSystemAudit::ComponentEntry e2;
    e2.name = "Input";
    e2.row_height = 28;
    audit.register_component(e2);

    auto atlas = audit.generate_component_atlas();
    REQUIRE(atlas.size() == 2);
    REQUIRE(atlas[0].name == "Button");
    REQUIRE(atlas[1].name == "Input");
}
