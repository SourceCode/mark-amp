#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Inspector section type.
enum class InspectorSection : uint8_t
{
    kTransform,
    kAppearance,
    kText,
    kConnector,
    kMedia,
    kData,
};

/// Property validation result.
enum class ValidationResult : uint8_t
{
    kValid,
    kOutOfRange,
    kInvalidFormat,
};

/// Property value for multi-select (may be mixed).
struct PropertyValue
{
    std::string value;
    bool is_mixed{false};

    // ── Round 5 Batch 1 (#8) ───────────────────────────────────

    /// (#8) Whether a value is set.
    [[nodiscard]] auto has_value() const noexcept -> bool
    {
        return !value.empty();
    }
};

/// Testable model for Canvas Inspector (Phase 60).
///
/// Encapsulates:
/// - Object-type-aware section visibility
/// - Multi-select with mixed-state indicators
/// - Property validation with constraints
/// - Reset-to-default and style copy actions
class InspectorModel
{
public:
    // ── Sections ────────────────────────────────────────────────────

    void set_visible_sections(std::vector<InspectorSection> sections);
    [[nodiscard]] auto visible_sections() const -> const std::vector<InspectorSection>&;
    [[nodiscard]] auto is_section_visible(InspectorSection section) const -> bool;

    // ── Selection ───────────────────────────────────────────────────

    void set_selected_count(int count);
    [[nodiscard]] auto selected_count() const -> int;
    [[nodiscard]] auto is_multi_select() const -> bool;

    // ── Properties ──────────────────────────────────────────────────

    void set_property(const std::string& key, PropertyValue value);
    [[nodiscard]] auto property(const std::string& key) const -> PropertyValue;
    void reset_property(const std::string& key);

    // ── Validation ──────────────────────────────────────────────────

    void set_constraint(const std::string& key, double min_val, double max_val);
    [[nodiscard]] auto validate(const std::string& key, double value) const -> ValidationResult;

    // ── Style copy ──────────────────────────────────────────────────

    void copy_style();
    [[nodiscard]] auto has_copied_style() const -> bool;

private:
    std::vector<InspectorSection> visible_sections_;
    int selected_count_{0};

    struct PropertyEntry
    {
        PropertyValue value;
        std::string default_value;
    };

    struct Constraint
    {
        double min_val{0.0};
        double max_val{1000.0};
    };

    std::vector<std::pair<std::string, PropertyEntry>> properties_;
    std::vector<std::pair<std::string, Constraint>> constraints_;
    bool has_copied_style_{false};

    // ── Round 5 Batch 1 (#9-10) ─────────────────────────────────

    /// (#9) Whether objects are selected.
    [[nodiscard]] auto has_selection() const noexcept -> bool
    {
        return selected_count_ > 0;
    }

    /// (#10) Number of visible sections.
    [[nodiscard]] auto section_count() const noexcept -> size_t
    {
        return visible_sections_.size();
    }
};

} // namespace markamp::canvas
