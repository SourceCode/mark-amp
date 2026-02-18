/// @file PrivacyManager.cpp
/// @brief V9 Phase 44 — PrivacyManager implementation.

#include "PrivacyManager.h"

#include <algorithm>

namespace markamp::core
{

auto privacy_level_name(PrivacyLevel level) -> std::string
{
    switch (level)
    {
        case PrivacyLevel::kPublic:
            return "public";
        case PrivacyLevel::kInternal:
            return "internal";
        case PrivacyLevel::kConfidential:
            return "confidential";
        case PrivacyLevel::kSecret:
            return "secret";
    }
    return "unknown";
}

auto PrivacyManager::classify_document(const std::string& doc_id, PrivacyLevel level) -> bool
{
    // Update if already classified
    for (auto& cls : classifications_)
    {
        if (cls.document_id == doc_id)
        {
            cls.level = level;
            cls.classified_at = std::chrono::system_clock::now();
            return true;
        }
    }
    PrivacyClassification cls;
    cls.document_id = doc_id;
    cls.level = level;
    cls.classified_at = std::chrono::system_clock::now();
    classifications_.push_back(std::move(cls));
    return true;
}

auto PrivacyManager::get_classification(const std::string& doc_id) const
    -> const PrivacyClassification*
{
    for (const auto& cls : classifications_)
    {
        if (cls.document_id == doc_id)
        {
            return &cls;
        }
    }
    return nullptr;
}

auto PrivacyManager::remove_classification(const std::string& doc_id) -> bool
{
    auto iter =
        std::remove_if(classifications_.begin(),
                       classifications_.end(),
                       [&](const PrivacyClassification& cls) { return cls.document_id == doc_id; });
    if (iter == classifications_.end())
    {
        return false;
    }
    classifications_.erase(iter, classifications_.end());
    return true;
}

void PrivacyManager::set_policy(PrivacyPolicy policy)
{
    policy_ = policy;
}

auto PrivacyManager::get_policy() const -> PrivacyPolicy
{
    return policy_;
}

void PrivacyManager::apply_policy_defaults()
{
    for (auto& cls : classifications_)
    {
        if (cls.level == PrivacyLevel::kPublic)
        {
            cls.level = policy_.default_level;
        }
    }
}

auto PrivacyManager::documents_at_level(PrivacyLevel level) const
    -> std::vector<const PrivacyClassification*>
{
    std::vector<const PrivacyClassification*> result;
    for (const auto& cls : classifications_)
    {
        if (cls.level == level)
        {
            result.push_back(&cls);
        }
    }
    return result;
}

auto PrivacyManager::classification_count() const -> int
{
    return static_cast<int>(classifications_.size());
}

void PrivacyManager::clear_all()
{
    classifications_.clear();
}

} // namespace markamp::core
