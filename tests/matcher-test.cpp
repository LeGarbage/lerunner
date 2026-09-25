#include "../src/matcher/matcher.hpp"
#include <gtest/gtest.h>

// NOLINTBEGIN(readability-identifier-length)
namespace {
TEST(TestMatcher, StartsWith) {
    Matcher m("t");
    ASSERT_EQ(m.score("test"), Matcher::SCORE_MATCH + Matcher::SCORE_MATCH_BEGINNING);

    Matcher o("ts");
    ASSERT_EQ(o.score("test"), (2 * Matcher::SCORE_MATCH) + Matcher::SCORE_MATCH_BEGINNING);
}

TEST(TestMatcher, EmptyHaystack) {
    Matcher m("e");
    ASSERT_EQ(m.score(""), -Matcher::NO_MATCH_PENALTY);
}

TEST(TestMatcher, SingleMatch) {
    Matcher m("o");
    ASSERT_EQ(m.score("to"), Matcher::SCORE_MATCH);

    Matcher o("o");
    ASSERT_EQ(o.score("too"), Matcher::SCORE_MATCH);

    Matcher p("s");
    ASSERT_EQ(p.score("matching letters is cool"), Matcher::SCORE_MATCH);
}

TEST(TestMatcher, ConsecutiveMatch) {
    Matcher m("or");
    ASSERT_EQ(m.score("short"), (2 * Matcher::SCORE_MATCH) + Matcher::SCORE_MATCH_CONSECUTIVE);

    Matcher o("st");
    ASSERT_EQ(o.score("test"), (2 * Matcher::SCORE_MATCH) + Matcher::SCORE_MATCH_CONSECUTIVE);

    Matcher p("onsecutive");
    ASSERT_EQ(p.score("consecutive"),
              (10 * Matcher::SCORE_MATCH) + (9 * Matcher::SCORE_MATCH_CONSECUTIVE));
}

TEST(TestMatcher, OutOfOrderMatch) {
    Matcher m("tu");
    ASSERT_EQ(m.score("out"), (2 * Matcher::SCORE_MATCH) - Matcher::MATCH_OUT_OF_ORDER_PENALTY);
}
} // namespace

// NOLINTEND(readability-identifier-length)
