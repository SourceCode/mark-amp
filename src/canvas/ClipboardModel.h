#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Paste mode.
enum class PasteMode : uint8_t
{
    kFull,        ///< Full object graph
    kStyleOnly,   ///< Apply source style to target
    kContentOnly, ///< Paste content without style
};

/// Duplicate mode.
enum class DuplicateMode : uint8_t
{
    kInPlace,
    kOffset,
};

/// A clipboard entry.
struct ClipboardEntry
{
    std::string object_id;
    std::string object_type;
    std::string serialized_data;

    // ── Round 4 Batch 5 (#46-47) ────────────────────────────────

    /// (#46) Whether serialized data is present.
    [[nodiscard]] auto has_data() const noexcept -> bool
    {
        return !serialized_data.empty();
    }

    /// (#47) Whether an object type is set.
    [[nodiscard]] auto has_type() const noexcept -> bool
    {
        return !object_type.empty();
    }
};

/// A board template.
struct BoardTemplate
{
    std::string template_id;
    std::string name;
    int object_count{0};
    std::string serialized_board;

    // ── Round 4 Batch 5 (#48-50) ────────────────────────────────

    /// (#48) Whether a name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#49) Whether serialized data is present.
    [[nodiscard]] auto has_data() const noexcept -> bool
    {
        return !serialized_board.empty();
    }

    /// (#50) Whether the template contains no objects.
    [[nodiscard]] auto is_empty_template() const noexcept -> bool
    {
        return object_count == 0;
    }
};

/// Testable model for Clipboard/Duplicate/Template (Phase 66).
///
/// Encapsulates:
/// - Clipboard with internal reference preservation
/// - Duplicate in-place vs offset
/// - Paste variants (full/style-only/content-only)
/// - Board template save/apply
class ClipboardModel
{
public:
    // ── Clipboard ───────────────────────────────────────────────────

    void copy(std::vector<ClipboardEntry> entries);
    [[nodiscard]] auto clipboard() const -> const std::vector<ClipboardEntry>&;
    [[nodiscard]] auto has_clipboard() const -> bool;
    void clear_clipboard();

    // ── Paste mode ──────────────────────────────────────────────────

    void set_paste_mode(PasteMode mode);
    [[nodiscard]] auto paste_mode() const -> PasteMode;

    // ── Duplicate ───────────────────────────────────────────────────

    void set_duplicate_mode(DuplicateMode mode);
    [[nodiscard]] auto duplicate_mode() const -> DuplicateMode;

    void set_duplicate_offset(double offset_x, double offset_y);
    [[nodiscard]] auto offset_x() const -> double;
    [[nodiscard]] auto offset_y() const -> double;

    // ── Templates ───────────────────────────────────────────────────

    void save_template(BoardTemplate tmpl);
    void remove_template(const std::string& template_id);
    [[nodiscard]] auto templates() const -> const std::vector<BoardTemplate>&;

private:
    std::vector<ClipboardEntry> clipboard_;
    PasteMode paste_mode_{PasteMode::kFull};
    DuplicateMode dup_mode_{DuplicateMode::kOffset};
    double offset_x_{20.0};
    double offset_y_{20.0};
    std::vector<BoardTemplate> templates_;

    // ── Round 4 Batch 6 (#51-55) ────────────────────────────────

    /// (#51) Number of clipboard entries.
    [[nodiscard]] auto clipboard_count() const noexcept -> size_t
    {
        return clipboard_.size();
    }

    /// (#52) Whether paste mode is style-only.
    [[nodiscard]] auto is_style_paste() const noexcept -> bool
    {
        return paste_mode_ == PasteMode::kStyleOnly;
    }

    /// (#53) Whether paste mode is content-only.
    [[nodiscard]] auto is_content_paste() const noexcept -> bool
    {
        return paste_mode_ == PasteMode::kContentOnly;
    }

    /// (#54) Whether duplicate mode is offset.
    [[nodiscard]] auto is_offset_dup() const noexcept -> bool
    {
        return dup_mode_ == DuplicateMode::kOffset;
    }

    /// (#55) Number of saved templates.
    [[nodiscard]] auto template_count() const noexcept -> size_t
    {
        return templates_.size();
    }
};

} // namespace markamp::canvas
