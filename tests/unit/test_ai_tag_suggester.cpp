// test_ai_tag_suggester.cpp — 10 tests for AI tag suggestion types
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

using namespace markamp::core;

TEST_CASE("TagSuggestion default values", "[ai][tag]")
{
    TagSuggestion tag;
    CHECK(tag.tag.empty());
    CHECK(tag.confidence == 0.0);
    CHECK_FALSE(tag.is_existing);
    CHECK(tag.reason.empty());
}

TEST_CASE("TagSuggestion high confidence tag", "[ai][tag]")
{
    TagSuggestion tag;
    tag.tag = "machine-learning";
    tag.confidence = 0.95;
    tag.is_existing = true;
    tag.reason = "Frequently discussed topic";
    CHECK(tag.confidence >= 0.9);
    CHECK(tag.is_existing);
}

TEST_CASE("TagSuggestion new tag creation", "[ai][tag]")
{
    TagSuggestion tag;
    tag.tag = "quantum-computing";
    tag.confidence = 0.72;
    tag.is_existing = false;
    tag.reason = "Novel topic detected";
    CHECK_FALSE(tag.is_existing);
    CHECK(tag.confidence > 0.7);
}

TEST_CASE("TagSuggestion collection sorting", "[ai][tag]")
{
    std::vector<TagSuggestion> tags;
    TagSuggestion t1;
    t1.tag = "low";
    t1.confidence = 0.3;
    TagSuggestion t2;
    t2.tag = "high";
    t2.confidence = 0.9;
    tags.push_back(t1);
    tags.push_back(t2);
    std::sort(tags.begin(),
              tags.end(),
              [](const TagSuggestion& lhs, const TagSuggestion& rhs)
              { return lhs.confidence > rhs.confidence; });
    CHECK(tags[0].tag == "high");
}

TEST_CASE("TagSuggestion filtering by threshold", "[ai][tag]")
{
    std::vector<TagSuggestion> all_tags;
    for (int idx = 0; idx < 10; ++idx)
    {
        TagSuggestion tag;
        tag.tag = "tag-" + std::to_string(idx);
        tag.confidence = 0.1 * static_cast<double>(idx);
        all_tags.push_back(tag);
    }
    std::vector<TagSuggestion> above_threshold;
    for (const auto& tag : all_tags)
    {
        if (tag.confidence >= 0.5)
        {
            above_threshold.push_back(tag);
        }
    }
    CHECK(above_threshold.size() == 5);
}

TEST_CASE("TagSuggestion hyphenated tag names", "[ai][tag]")
{
    TagSuggestion tag;
    tag.tag = "deep-learning-models";
    CHECK(tag.tag.find('-') != std::string::npos);
}

TEST_CASE("TagSuggestion with empty reason", "[ai][tag]")
{
    TagSuggestion tag;
    tag.tag = "testing";
    tag.confidence = 0.6;
    CHECK(tag.reason.empty());
}

TEST_CASE("TagSuggestion duplicate detection", "[ai][tag]")
{
    std::vector<TagSuggestion> tags;
    TagSuggestion t1;
    t1.tag = "python";
    t1.confidence = 0.9;
    TagSuggestion t2;
    t2.tag = "python";
    t2.confidence = 0.8;
    tags.push_back(t1);
    tags.push_back(t2);
    CHECK(tags[0].tag == tags[1].tag);
}

TEST_CASE("TagSuggestion existing vs new comparison", "[ai][tag]")
{
    TagSuggestion existing;
    existing.tag = "existing-tag";
    existing.is_existing = true;
    TagSuggestion novel;
    novel.tag = "novel-tag";
    novel.is_existing = false;
    CHECK(existing.is_existing != novel.is_existing);
}

TEST_CASE("TagSuggestion confidence boundary", "[ai][tag]")
{
    TagSuggestion tag;
    tag.confidence = 0.0;
    CHECK(tag.confidence >= 0.0);
    tag.confidence = 1.0;
    CHECK(tag.confidence <= 1.0);
}
