#pragma once

/*
 * Fuzzy matcher algorithm
 *
 * Given a needle and a haystack
 *
 * For each letter in needle:
 *
 * first letter in needle matches more
 * letter in haystack +1
 * letter matches case +1
 * haystack starts with letter +4
 * word in haystack starts with letter +4
 * for each previous letter that also matched +1
 */

#include <span>
#include <string>
#include <utility>
#include <vector>

class Matcher {
    public:
    Matcher(std::string pattern);

    /// Scores the `haystack` against the matcher
    /// @param haystack the haystack to score
    int score(const std::string &haystack);
    std::vector<std::pair<std::string, int>> score_list(std::span<const std::string> haystacks);

    static constexpr int SCORE_MATCH = 1;
    static constexpr int SCORE_MATCH_BEGINNING = 5;
    static constexpr int SCORE_MATCH_BEGINNING_WORD = 2;
    static constexpr int SCORE_MATCH_CONSECUTIVE = 3;
    static constexpr int NO_MATCH_PENALTY = 1;
    static constexpr int MATCH_OUT_OF_ORDER_PENALTY = 1;

    private:
    std::string m_pattern;
};
