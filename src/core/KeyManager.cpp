/// @file KeyManager.cpp
/// @brief V9 Phase 44 — KeyManager implementation.

#include "KeyManager.h"

#include <algorithm>

namespace markamp::core
{

auto KeyManager::generate_key(const std::string& algorithm) -> std::string
{
    KeyEntry entry;
    entry.key_id = "key_" + std::to_string(next_id_++);
    entry.algorithm = algorithm;
    entry.created_at = std::chrono::system_clock::now();
    entry.expires_at =
        entry.created_at + std::chrono::hours(24 * rotation_policy_.max_key_age_days);
    entry.is_active = true;

    auto kid = entry.key_id;
    keys_.push_back(std::move(entry));
    return kid;
}

auto KeyManager::revoke_key(const std::string& key_id) -> bool
{
    for (auto& key : keys_)
    {
        if (key.key_id == key_id && !key.is_revoked)
        {
            key.is_revoked = true;
            key.is_active = false;
            return true;
        }
    }
    return false;
}

auto KeyManager::find_key(const std::string& key_id) const -> const KeyEntry*
{
    for (const auto& key : keys_)
    {
        if (key.key_id == key_id)
        {
            return &key;
        }
    }
    return nullptr;
}

auto KeyManager::rotate_key(const std::string& key_id) -> std::string
{
    KeyEntry* old_key = nullptr;
    for (auto& key : keys_)
    {
        if (key.key_id == key_id)
        {
            old_key = &key;
            break;
        }
    }
    if (old_key == nullptr || old_key->is_revoked)
    {
        return "";
    }
    old_key->is_active = false;
    return generate_key(old_key->algorithm);
}

auto KeyManager::keys_needing_rotation() const -> std::vector<const KeyEntry*>
{
    std::vector<const KeyEntry*> result;
    auto now = std::chrono::system_clock::now();
    for (const auto& key : keys_)
    {
        if (key.is_active && !key.is_revoked && now >= key.expires_at)
        {
            result.push_back(&key);
        }
    }
    return result;
}

void KeyManager::set_rotation_policy(KeyRotationPolicy policy)
{
    rotation_policy_ = policy;
}

auto KeyManager::get_rotation_policy() const -> KeyRotationPolicy
{
    return rotation_policy_;
}

auto KeyManager::active_keys() const -> std::vector<const KeyEntry*>
{
    std::vector<const KeyEntry*> result;
    for (const auto& key : keys_)
    {
        if (key.is_active && !key.is_revoked)
        {
            result.push_back(&key);
        }
    }
    return result;
}

auto KeyManager::key_count() const -> int
{
    return static_cast<int>(keys_.size());
}

auto KeyManager::revoked_count() const -> int
{
    return static_cast<int>(std::count_if(
        keys_.begin(), keys_.end(), [](const KeyEntry& key) { return key.is_revoked; }));
}

void KeyManager::clear_all()
{
    keys_.clear();
}

} // namespace markamp::core
