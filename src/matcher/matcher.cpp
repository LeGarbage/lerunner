#include "matcher.hpp"

/*
 * Fuzzy matcher algorithm
 *
 * Given a needle and a haystack
 *
 * For each letter in needle:
 *
 * letter in haystack +1
 * haystack starts with letter +4
 * word in haystack starts with letter +4
 * for each previous letter that also matched +1
 * letter not in haystack -1
 */

#include <algorithm>
#include <iterator>
#include <ranges>
#include <set>
#include <utility>

namespace {
template <std::input_iterator I, std::sentinel_for<I> S, typename T>
I find_with_exclusions(I first, S last, const T &value, const std::set<I> &excluded_its) {
    for (auto it = first; it != last; ++it) {
        if (*it == value && !excluded_its.contains(it)) { return it; }
    }

    return last;
}
} // namespace

Matcher::Matcher(std::string pattern)
    : m_pattern(std::move(pattern)) {}

int Matcher::score(const std::string &haystack) {
    int score = 0;
    std::set<std::string::const_iterator> matched_its;
    /// The character to begin matching at
    std::string::const_iterator begin_it = haystack.begin();
    bool found_begin_it = false;

    for (auto [idx, letter] : m_pattern | std::views::enumerate) {
        std::string::const_iterator match_it = haystack.end();
        // Prioritize looking for the next character after the marker
        match_it = find_with_exclusions(begin_it, haystack.end(), letter, matched_its);

        if (match_it == haystack.end()) {
            match_it = find_with_exclusions(haystack.begin(), begin_it, letter, matched_its);

            if (match_it == begin_it) { match_it = haystack.end(); }
        }
        if (match_it == haystack.end()) {
            score -= NO_MATCH_PENALTY;
            continue;
        }

        if (!found_begin_it) {
            begin_it = match_it;
            found_begin_it = true;
        }

        score += SCORE_MATCH;

        if (idx == 0 && match_it == haystack.begin()) {
            score += SCORE_MATCH_BEGINNING;
        } else {
            const auto prev_it = std::ranges::prev(match_it);

            if (std::isspace(*prev_it) != 0) { score += SCORE_MATCH_BEGINNING_WORD; }
            if (matched_its.contains(prev_it)) { score += SCORE_MATCH_CONSECUTIVE; }
        }

        auto out_of_order_it = std::ranges::find_if(
            matched_its, [match_it](const auto matched_it) { return match_it < matched_it; });
        if (out_of_order_it != matched_its.end()) { score -= MATCH_OUT_OF_ORDER_PENALTY; }

        matched_its.insert(match_it);
    }

    return score;
}

std::vector<std::pair<std::string, int>>
Matcher::score_list(std::span<const std::string> haystacks) {
    return haystacks
           | std::views::transform(
               [this](const auto &haystack) { return std::pair(haystack, score(haystack)); })
           | std::ranges::to<std::vector>();
}
