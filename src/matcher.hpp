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
