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

TEST_CASE("ArtifactNamingPolicy: generate notebook name", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    auto name1 = policy.generate_name(ArtifactKind::kNotebook);
    REQUIRE(name1 == "Notebook-1");

    auto name2 = policy.generate_name(ArtifactKind::kNotebook);
    REQUIRE(name2 == "Notebook-2");
}

TEST_CASE("ArtifactNamingPolicy: generate canvas name", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    auto name1 = policy.generate_name(ArtifactKind::kCanvas);
    REQUIRE(name1 == "Board-1");

    auto name2 = policy.generate_name(ArtifactKind::kCanvas);
    REQUIRE(name2 == "Board-2");
}

TEST_CASE("ArtifactNamingPolicy: sequences are per-kind", "[v20][artifact-naming]")
{
    ArtifactNamingPolicy policy;

    auto text1 = policy.generate_name(ArtifactKind::kTextFile);
    auto nb1 = policy.generate_name(ArtifactKind::kNotebook);
    auto canvas1 = policy.generate_name(ArtifactKind::kCanvas);
    auto text2 = policy.generate_name(ArtifactKind::kTextFile);

    // Each kind has independent sequence
    REQUIRE(text1 == "Untitled-1");
    REQUIRE(nb1 == "Notebook-1");
    REQUIRE(canvas1 == "Board-1");
    REQUIRE(text2 == "Untitled-2");
}

// ============================================================================
// Default extensions
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: default extensions", "[v20][artifact-naming]")
{
    REQUIRE(ArtifactNamingPolicy::default_extension(ArtifactKind::kTextFile) == "md");
    REQUIRE(ArtifactNamingPolicy::default_extension(ArtifactKind::kNotebook) == "markamp-nb");
    REQUIRE(ArtifactNamingPolicy::default_extension(ArtifactKind::kCanvas) == "markamp-canvas");
}

// ============================================================================
// Default languages
// ============================================================================

TEST_CASE("ArtifactNamingPolicy: default languages", "[v20][artifact-naming]")
{
    REQUIRE(ArtifactNamingPolicy::default_language(ArtifactKind::kTextFile) == "markdown");
    REQUIRE(ArtifactNamingPolicy::default_language(ArtifactKind::kNotebook) == "notebook");
    REQUIRE(ArtifactNamingPolicy::default_language(ArtifactKind::kCanvas) == "canvas");
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
    REQUIRE(ArtifactNamingPolicy::kind_label(ArtifactKind::kNotebook) == "Notebook");
    REQUIRE(ArtifactNamingPolicy::kind_label(ArtifactKind::kCanvas) == "Board");
}
