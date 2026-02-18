/// @file KeyManager.h
/// @brief V9 Phase 44 — Encryption key lifecycle and rotation management.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A managed encryption key entry.
struct KeyEntry
{
    std::string key_id;
    std::string algorithm{"AES-256-GCM"};
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point expires_at;
    int usage_count{0};
    bool is_active{true};
    bool is_revoked{false};
};

/// Policy controlling automatic key rotation.
struct KeyRotationPolicy
{
    int rotation_interval_days{90};
    int max_key_age_days{365};
    bool auto_rotate{false};
};

/// Manages encryption keys, rotation, and revocation.
class KeyManager
{
public:
    KeyManager() = default;

    // ── Key lifecycle ─────────────────────────────────────────────────
    auto generate_key(const std::string& algorithm = "AES-256-GCM") -> std::string;
    auto revoke_key(const std::string& key_id) -> bool;
    [[nodiscard]] auto find_key(const std::string& key_id) const -> const KeyEntry*;

    // ── Rotation ──────────────────────────────────────────────────────
    auto rotate_key(const std::string& key_id) -> std::string;
    [[nodiscard]] auto keys_needing_rotation() const -> std::vector<const KeyEntry*>;

    // ── Policy ────────────────────────────────────────────────────────
    void set_rotation_policy(KeyRotationPolicy policy);
    [[nodiscard]] auto get_rotation_policy() const -> KeyRotationPolicy;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto active_keys() const -> std::vector<const KeyEntry*>;
    [[nodiscard]] auto key_count() const -> int;
    [[nodiscard]] auto revoked_count() const -> int;
    void clear_all();

private:
    std::vector<KeyEntry> keys_;
    KeyRotationPolicy rotation_policy_;
    int next_id_{1};
};

} // namespace markamp::core
