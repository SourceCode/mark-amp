#include "canvas/StickyAggregator.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::canvas;

namespace
{

auto make_note(const std::string& text) -> StickyNote
{
    StickyNote note;
    note.set_text(text);
    return note;
}

} // anonymous namespace

TEST_CASE("StickyAggregator: identical text -> similarity 1.0", "[sticky_aggregator]")
{
    StickyAggregator aggregator;
    const double sim = aggregator.text_similarity("hello world", "hello world");
    REQUIRE_THAT(sim, Catch::Matchers::WithinAbs(1.0, 0.001));
}

TEST_CASE("StickyAggregator: similar text > 0.7", "[sticky_aggregator]")
{
    StickyAggregator aggregator;
    // 5 out of 6 words shared -> Jaccard = 5/6 = 0.833.
    const double sim = aggregator.text_similarity("the quick brown fox jumps high",
                                                  "the quick brown fox jumps far");
    REQUIRE(sim > 0.7);
}

TEST_CASE("StickyAggregator: different text < 0.3", "[sticky_aggregator]")
{
    StickyAggregator aggregator;
    const double sim =
        aggregator.text_similarity("apples oranges bananas", "cars trucks buses motorcycles");
    REQUIRE(sim < 0.3);
}

TEST_CASE("StickyAggregator: threshold filtering", "[sticky_aggregator]")
{
    auto note_a = make_note("identical text here");
    auto note_b = make_note("identical text here");
    auto note_c = make_note("completely different content");

    std::vector<std::pair<ObjectId, const StickyNote*>> notes = {
        {1, &note_a}, {2, &note_b}, {3, &note_c}};

    StickyAggregator aggregator;
    auto results = aggregator.find_duplicates(notes, 0.9);

    // Only the identical pair should pass 0.9 threshold.
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].note_a == 1);
    REQUIRE(results[0].note_b == 2);
    REQUIRE(results[0].reason == "Identical text");
}
