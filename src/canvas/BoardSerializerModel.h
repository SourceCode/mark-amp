#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Load status after deserialization.
enum class LoadStatus : uint8_t
{
    kSuccess,
    kPartialRecovery,
    kFailed,
};

/// Schema migration entry.
struct SchemaMigration
{
    int from_version{0};
    int to_version{0};
    std::string description;

    // ── Round 4 Batch 10 (#93-94) ───────────────────────────────

    /// (#93) Whether a migration description exists.
    [[nodiscard]] auto has_description() const noexcept -> bool
    {
        return !description.empty();
    }

    /// (#94) Number of version levels spanned.
    [[nodiscard]] auto version_jump() const noexcept -> int
    {
        return to_version - from_version;
    }
};

/// Validation diagnostic.
struct SerializerDiagnostic
{
    std::string object_id;
    std::string message;
    bool is_warning{true}; ///< false = error

    // ── Round 4 Batch 10 (#95-96) ───────────────────────────────

    /// (#95) Whether this diagnostic is an error.
    [[nodiscard]] auto is_error() const noexcept -> bool
    {
        return !is_warning;
    }

    /// (#96) Whether a diagnostic message is set.
    [[nodiscard]] auto has_message() const noexcept -> bool
    {
        return !message.empty();
    }
};

/// Testable model for Board Serialization/Recovery (Phase 68).
///
/// Encapsulates:
/// - Schema versioning with migration history
/// - Partial-load recovery mode
/// - Atomic save state tracking
/// - Validation diagnostics
class BoardSerializerModel
{
public:
    // ── Schema version ──────────────────────────────────────────────

    void set_schema_version(int version);
    [[nodiscard]] auto schema_version() const -> int;

    void add_migration(SchemaMigration migration);
    [[nodiscard]] auto migrations() const -> const std::vector<SchemaMigration>&;
    [[nodiscard]] auto needs_migration(int file_version) const -> bool;

    // ── Load status ─────────────────────────────────────────────────

    void set_load_status(LoadStatus status);
    [[nodiscard]] auto load_status() const -> LoadStatus;

    void set_recovered_count(int count);
    [[nodiscard]] auto recovered_count() const -> int;

    // ── Atomic save ─────────────────────────────────────────────────

    void begin_save();
    void commit_save();
    void rollback_save();
    [[nodiscard]] auto is_saving() const -> bool;
    [[nodiscard]] auto save_succeeded() const -> bool;

    // ── Diagnostics ─────────────────────────────────────────────────

    void add_diagnostic(SerializerDiagnostic diag);
    void clear_diagnostics();
    [[nodiscard]] auto diagnostics() const -> const std::vector<SerializerDiagnostic>&;
    [[nodiscard]] auto error_count() const -> int;

private:
    int schema_version_{1};
    std::vector<SchemaMigration> migrations_;
    LoadStatus load_status_{LoadStatus::kSuccess};
    int recovered_count_{0};
    bool is_saving_{false};
    bool save_succeeded_{false};
    std::vector<SerializerDiagnostic> diagnostics_;

    // ── Round 4 Batch 10 (#97-98) ───────────────────────────────

    /// (#97) Whether any diagnostics exist.
    [[nodiscard]] auto has_diagnostics() const noexcept -> bool
    {
        return !diagnostics_.empty();
    }

    /// (#98) Whether load was a partial recovery.
    [[nodiscard]] auto is_partial_recovery() const noexcept -> bool
    {
        return load_status_ == LoadStatus::kPartialRecovery;
    }
};

} // namespace markamp::canvas
