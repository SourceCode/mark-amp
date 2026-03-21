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
        case ArtifactKind::kNotebook:
            return "markamp-nb";
        case ArtifactKind::kCanvas:
            return "markamp-canvas";
    }
    return "md";
}

auto ArtifactNamingPolicy::default_language(ArtifactKind kind) -> std::string
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return "markdown";
        case ArtifactKind::kNotebook:
            return "notebook";
        case ArtifactKind::kCanvas:
            return "canvas";
    }
    return "markdown";
}

auto ArtifactNamingPolicy::next_sequence(ArtifactKind kind) const -> int
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return text_file_seq_ + 1;
        case ArtifactKind::kNotebook:
            return notebook_seq_ + 1;
        case ArtifactKind::kCanvas:
            return canvas_seq_ + 1;
    }
    return 1;
}

auto ArtifactNamingPolicy::kind_label(ArtifactKind kind) -> std::string
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return "Untitled";
        case ArtifactKind::kNotebook:
            return "Notebook";
        case ArtifactKind::kCanvas:
            return "Board";
    }
    return "Untitled";
}

auto ArtifactNamingPolicy::increment_sequence(ArtifactKind kind) -> int
{
    switch (kind)
    {
        case ArtifactKind::kTextFile:
            return ++text_file_seq_;
        case ArtifactKind::kNotebook:
            return ++notebook_seq_;
        case ArtifactKind::kCanvas:
            return ++canvas_seq_;
    }
    return 1;
}

} // namespace markamp::core
