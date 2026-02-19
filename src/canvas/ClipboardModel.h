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
};

/// A board template.
struct BoardTemplate
{
    std::string template_id;
    std::string name;
    int object_count{0};
    std::string serialized_board;
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
};

} // namespace markamp::canvas
