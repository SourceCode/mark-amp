#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Object layer state.
enum class LayerState : uint8_t
{
    kNormal,
    kLocked,
    kHidden,
};

/// Layer entry for an object.
struct LayerEntry
{
    std::string object_id;
    std::string label;
    int z_order{0};
    LayerState state{LayerState::kNormal};

    // ── Round 5 Batch 8 (#71-74) ────────────────────────────────

    /// (#71) Whether this layer is locked.
    [[nodiscard]] auto is_locked() const noexcept -> bool
    {
        return state == LayerState::kLocked;
    }

    /// (#72) Whether this layer is hidden.
    [[nodiscard]] auto is_hidden() const noexcept -> bool
    {
        return state == LayerState::kHidden;
    }

    /// (#73) Whether this layer is in normal state.
    [[nodiscard]] auto is_normal() const noexcept -> bool
    {
        return state == LayerState::kNormal;
    }

    /// (#74) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }
};

/// Testable model for Layering, Z-Order & Locking (Phase 50).
///
/// Encapsulates:
/// - Z-order management (bring forward/backward, front/back)
/// - Lock/hidden state per object
/// - Selection filtering (skip hidden/locked)
/// - Layer stack query
class LayeringModel
{
public:
    void set_layers(std::vector<LayerEntry> layers);
    [[nodiscard]] auto layers() const -> const std::vector<LayerEntry>&;

    // ── Z-order ─────────────────────────────────────────────────────

    void bring_forward(const std::string& object_id);
    void send_backward(const std::string& object_id);
    void bring_to_front(const std::string& object_id);
    void send_to_back(const std::string& object_id);

    // ── Lock / Hidden ───────────────────────────────────────────────

    void set_state(const std::string& object_id, LayerState state);
    [[nodiscard]] auto state_of(const std::string& object_id) const -> LayerState;

    [[nodiscard]] auto is_locked(const std::string& object_id) const -> bool;
    [[nodiscard]] auto is_hidden(const std::string& object_id) const -> bool;

    // ── Selection filtering ─────────────────────────────────────────

    /// Return only selectable objects (not hidden, optionally not locked).
    [[nodiscard]] auto selectable(bool include_locked = false) const -> std::vector<LayerEntry>;

private:
    std::vector<LayerEntry> layers_;

    auto find_index(const std::string& object_id) -> int;

    // ── Round 5 Batch 8 (#75) ───────────────────────────────────

    /// (#75) Number of layers.
    [[nodiscard]] auto layer_count() const noexcept -> size_t
    {
        return layers_.size();
    }
};

} // namespace markamp::canvas
