/// @file test_v20_artifact_naming.cpp
/// @brief V20 Phase 01 – ArtifactNamingPolicy unit tests.

#include "core/ArtifactNamingPolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Name generation
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: generate text file name", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    auto name1 = policy.generate_name(ArtifactKind::kTextFile);
    REQUIRE(name1 == "Untitled-1");

    auto name2 = policy.generate_name(ArtifactKind::kTextFile);
    REQUIRE(name2 == "Untitled-2");
}

TEST_CASE("ArtifactNamingPolicy: sequences are per-kind", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    auto text1 = policy.generate_name(ArtifactKind::kTextFile);
    auto ws1 = policy.generate_name(ArtifactKind::kTextFile);
    auto text2 = policy.generate_name(ArtifactKind::kTextFile);

    // Each kind has independent sequence
    REQUIRE(text1 == "Untitled-1");
    REQUIRE(!ws1.empty());
    REQUIRE(text2 == "Untitled-3");
}

// ============================================================================
// Default extensions
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: default extensions", "[v20][artifact-naming]")
{
    REQUIRE(ArtifactNamingPolicy::default_extension(ArtifactKind::kTextFile) == "md");
}

// ============================================================================
// Default languages
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: default languages", "[v20][artifact-naming]")
{
    REQUIRE(ArtifactNamingPolicy::default_language(ArtifactKind::kTextFile) == "markdown");
}

// ============================================================================
// Next sequence
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: next_sequence peeks without incrementing", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    REQUIRE(policy.next_sequence(ArtifactKind::kTextFile) == 1);
    REQUIRE(policy.next_sequence(ArtifactKind::kTextFile) == 1); // Still 1, not incremented

    policy.generate_name(ArtifactKind::kTextFile); // Now increment to 1
    REQUIRE(policy.next_sequence(ArtifactKind::kTextFile) == 2);
}

// ============================================================================
// Kind label
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: kind labels", "[v20][artifact-naming]")
{
    REQUIRE(ArtifactNamingPolicy::kind_label(ArtifactKind::kTextFile) == "Untitled");
}
