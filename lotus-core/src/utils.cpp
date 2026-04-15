#include "lotus/utils.hpp"
#include "lotus/constants.hpp"
#include <algorithm>

namespace lotus {

std::u32string ToU32(const std::u16string& s) {
    std::u32string result;
    for (char16_t c : s) {
        result.push_back(static_cast<char32_t>(c));
    }
    return result;
}

std::u16string ToU16(const std::u32string& s) {
    std::u16string result;
    for (char32_t c : s) {
        result.push_back(static_cast<char16_t>(c));
    }
    return result;
}

bool IsSpace(char32_t key) {
    return key == U' ';
}

bool IsPunctuationMark(char32_t key) {
    return std::find(PUNCTUATION_MARKS.begin(), PUNCTUATION_MARKS.end(), key) != PUNCTUATION_MARKS.end();
}

bool IsWordBreakSymbol(char32_t key) {
    return IsPunctuationMark(key) || (key >= U'0' && key <= U'9');
}

bool IsVowel(char32_t chr) {
    return VOWELS.find(chr) != std::u32string::npos;
}

int FindVowelPosition(char32_t chr) {
    auto pos = VOWELS.find(chr);
    if (pos == std::u32string::npos) {
        return -1;
    }
    return static_cast<int>(pos);
}

std::vector<char32_t> GetMarkFamily(char32_t chr) {
    std::vector<char32_t> result;
    auto it = MARKS_MAPS.find(chr);
    if (it != MARKS_MAPS.end()) {
        for (char32_t c : it->second) {
            if (c != U'_') {
                result.push_back(c);
            }
        }
    }
    return result;
}

int FindMarkPosition(char32_t chr) {
    auto it = MARKS_MAPS.find(chr);
    if (it != MARKS_MAPS.end()) {
        const std::u32string& s = it->second;
        for (size_t i = 0; i < s.length(); ++i) {
            if (s[i] == chr) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

Mark FindMarkFromChar(char32_t chr) {
    int pos = FindMarkPosition(chr);
    if (pos >= 0) {
        return static_cast<Mark>(pos);
    }
    return MarkNone;
}

char32_t AddMarkToTonelessChar(char32_t chr, uint8_t mark) {
    auto it = MARKS_MAPS.find(chr);
    if (it != MARKS_MAPS.end()) {
        const std::u32string& marks = it->second;
        if (mark < marks.length() && marks[mark] != U'_') {
            return marks[mark];
        }
    }
    return chr;
}

char32_t AddMarkToChar(char32_t chr, uint8_t mark) {
    Tone tone = FindToneFromChar(chr);
    chr = AddToneToChar(chr, 0);
    chr = AddMarkToTonelessChar(chr, mark);
    return AddToneToChar(chr, static_cast<uint8_t>(tone));
}

bool IsAlpha(char32_t c) {
    return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
}

Tone FindToneFromChar(char32_t chr) {
    int pos = FindVowelPosition(chr);
    if (pos == -1) {
        return ToneNone;
    }
    return static_cast<Tone>(pos % 6);
}

char32_t AddToneToChar(char32_t chr, uint8_t tone) {
    int pos = FindVowelPosition(chr);
    if (pos > -1) {
        int currentTone = pos % 6;
        int offset = static_cast<int>(tone) - currentTone;
        return VOWELS[static_cast<size_t>(pos + offset)];
    } else {
        return chr;
    }
}

bool IsVietnameseRune(char32_t lowerKey) {
    if (FindToneFromChar(lowerKey) != ToneNone) {
        return true;
    }
    return lowerKey != AddMarkToTonelessChar(lowerKey, 0);
}

bool HasAnyVietnameseRune(const std::u32string& word) {
    for (char32_t chr : word) {
        // Assume input is already normalized or handled appropriately
        // Simplified for MVP
        if (IsVietnameseRune(chr)) {
            return true;
        }
    }
    return false;
}

bool HasAnyVietnameseVowel(const std::u32string& word) {
    for (char32_t chr : word) {
        if (IsVowel(chr)) {
            return true;
        }
    }
    return false;
}

} // namespace lotus
