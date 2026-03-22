#include "Spelling.h"
#include "Character.h"
#include <vector>
#include <string>

namespace bamboo { namespace engine {

    using u32sv = std::u32string_view;

    // Ported from spelling.go
    static const std::vector<std::vector<std::u32string>> FIRST_CONSONANT_SEQS = {
        {U"b", U"d", U"\u0111", U"g", U"gh", U"m", U"n", U"nh", U"p", U"ph", U"r", U"s", U"t", U"tr", U"v", U"z"},
        {U"c", U"h", U"k", U"kh", U"kr", U"qu", U"th"},
        {U"ch", U"gi", U"l", U"ng", U"ngh", U"x"},
        {U"b", U"\u0111", U"l"},
        {U"h"}
    };

    static const std::vector<std::vector<std::u32string>> VOWEL_SEQS = {
        {U"\u00ea", U"i", U"ua", U"u\u00ea", U"uy", U"y"},
        {U"a", U"i\u00ea", U"oa", U"uy\u00ea", U"y\u00ea"},
        {U"\u00e2", U"\u0103", U"e", U"o", U"oo", U"\u00f4", U"\u01a1", U"oe", U"u", U"\u01b0", U"u\u00e2", U"u\u00f4", U"\u01b0\u01a1"},
        {U"o\u0103"},
        {U"u\u01a1"},
        {U"ai", U"ao", U"au", U"\u00e2u", U"ay", U"\u00e2y", U"eo", U"\u00eau", U"ia", U"i\u00eau", U"iu", U"oai", U"oao", U"oay", U"oeo", U"oi", U"\u00f4i", U"\u01a1i", U"\u01b0a", U"u\u00e2y", U"ui", U"\u01b0i", U"u\u00f4i", U"\u01b0\u01a1i", U"\u01b0\u01a1u", U"\u01b0u", U"uya", U"uyu", U"u\u00eau", U"y\u00eau"},
        {U"\u0103", U"u"},
        {U"i"}
    };

    static const std::vector<std::vector<std::u32string>> LAST_CONSONANT_SEQS = {
        {U"ch", U"nh"},
        {U"c", U"ng"},
        {U"m", U"n", U"p", U"t"},
        {U"k"},
        {U"c"}
    };

    static const std::vector<std::vector<int>> CV_MATRIX = {
        {0, 1, 2, 5},
        {0, 1, 2, 3, 4, 5},
        {0, 1, 2, 3, 5},
        {6},
        {7}
    };

    static const std::vector<std::vector<int>> VC_MATRIX = {
        {0, 2},
        {0, 1, 2},
        {1, 2},
        {1, 2},
        {},
        {},
        {3},
        {4}
    };

    static std::vector<int> Lookup(const std::vector<std::vector<std::u32string>>& seq, u32sv input, bool inputIsFull, bool inputIsComplete) {
        std::vector<int> ret;
        for (size_t index = 0; index < seq.size(); ++index) {
            for (const auto& canvas : seq[index]) {
                if (canvas.length() < input.length() || (inputIsFull && canvas.length() > input.length())) {
                    continue;
                }
                bool isMatch = true;
                for (size_t k = 0; k < input.length(); ++k) {
                    char32_t ic = input[k];
                    char32_t cc = canvas[k];
                    // Strip tone/mark from canvas char if not complete
                    if (ic != cc && !(!inputIsComplete && AddMarkToTonelessChar(cc, 0) == ic)) {
                        isMatch = false;
                        break;
                    }
                }
                if (isMatch) {
                    ret.push_back((int)index);
                    break;
                }
            }
        }
        return ret;
    }

    static bool IsValidCV(const std::vector<int>& fcIndexes, const std::vector<int>& voIndexes) {
        for (int fc : fcIndexes) {
            for (int c : CV_MATRIX[fc]) {
                for (int vo : voIndexes) {
                    if (c == vo) return true;
                }
            }
        }
        return false;
    }

    static bool IsValidVC(const std::vector<int>& voIndexes, const std::vector<int>& lcIndexes) {
        for (int vo : voIndexes) {
            for (int c : VC_MATRIX[vo]) {
                for (int lc : lcIndexes) {
                    if (c == lc) return true;
                }
            }
        }
        return false;
    }

    bool IsValidCVC(u32sv fc, u32sv vo, u32sv lc, bool inputIsFullComplete) {
        std::vector<int> fcIndexes, voIndexes, lcIndexes;

        if (!fc.empty()) {
            fcIndexes = Lookup(FIRST_CONSONANT_SEQS, fc, inputIsFullComplete || !vo.empty(), true);
            if (fcIndexes.empty()) return false;
        }

        if (!vo.empty()) {
            voIndexes = Lookup(VOWEL_SEQS, vo, inputIsFullComplete || !lc.empty(), inputIsFullComplete);
            if (voIndexes.empty()) return false;
        }

        if (!lc.empty()) {
            lcIndexes = Lookup(LAST_CONSONANT_SEQS, lc, inputIsFullComplete, true);
            if (lcIndexes.empty()) return false;
        }

        if (voIndexes.empty()) {
            // first consonant only
            return !fcIndexes.empty();
        }

        if (!fcIndexes.empty()) {
            // first consonant + vowel
            bool cv = IsValidCV(fcIndexes, voIndexes);
            if (!cv || lcIndexes.empty()) return cv;
        }

        if (!lcIndexes.empty()) {
            // vowel + last consonant
            return IsValidVC(voIndexes, lcIndexes);
        }

        return true;
    }

}} // namespace bamboo::engine
