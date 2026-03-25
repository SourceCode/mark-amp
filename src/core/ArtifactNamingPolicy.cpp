/// @file ArtifactNamingPolicy.cpp
/// @brief V20 P01-T05: Naming policy implementation.

#include "ArtifactNamingPolicy.h"

namespace markamp::core
{

auto ArtifactNamingPolicy::generate_name(ArtifactKind kind) -> std::string
{
    const auto seq = increment_sequence(kind);
    return kind_label(kind) + "-" + std::to_string(seq);
}

auto ArtifactNamingPolicy::default_extension(ArtifactKind kind) -> std::string
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return "md";
    }
    return "md";
}

auto ArtifactNamingPolicy::default_language(ArtifactKind kind) -> std::string
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return "markdown";
    }
    return "markdown";
}

auto ArtifactNamingPolicy::next_sequence(ArtifactKind kind) const -> int
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return text_file_seq_ + 1;
    }
    return 1;
}

auto ArtifactNamingPolicy::kind_label(ArtifactKind kind) -> std::string
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return "Untitled";
    }
    return "Untitled";
}

auto ArtifactNamingPolicy::increment_sequence(ArtifactKind kind) -> int
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return ++text_file_seq_;
    }
    return 1;
}

} // namespace markamp::core
