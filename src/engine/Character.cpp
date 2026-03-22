#include "Character.h"
#include <algorithm>
#include <unordered_map>
#include <string>

namespace bamboo { namespace engine {

    // UTF-32 encoded vowels: aàáảãạ ăằắẳẵặ âầấẩẫậ eèéẻẽẹ êềếểễệ iìíỉĩị oòóỏõọ ôồốổỗộ ơờớởỡợ uùúủũụ ưừứửữự yỳýỷỹỵ
    static constexpr std::u32string_view VOWELS = 
        U"a\u00e0\u00e1\u1ea3\u00e3\u1ea1"
        U"\u0103\u1eb1\u1eaf\u1eb3\u1eb5\u1eb7"
        U"\u00e2\u1ea7\u1ea5\u1ea9\u1eab\u1ead"
        U"e\u00e8\u00e9\u1ebb\u1ebd\u1eb9"
        U"\u00ea\u1ec1\u1ebf\u1ec3\u1ec5\u1ec7"
        U"i\u00ec\u00ed\u1ec9\u0129\u1ecb"
        U"o\u00f2\u00f3\u1ecf\u00f5\u1ecd"
        U"\u00f4\u1ed3\u1ed1\u1ed5\u1ed7\u1ed9"
        U"\u01a1\u1edd\u1edb\u1edf\u1ee1\u1ee3"
        U"u\u00f9\u00fa\u1ee7\u0169\u1ee5"
        U"\u01b0\u1eeb\u1ee9\u1eed\u1eef\u1ef1"
        U"y\u1ef3\u00fd\u1ef7\u1ef9\u1ef5";

    static constexpr std::u32string_view VOWELS_UPPER = 
        U"A\u00c0\u00c1\u1ea2\u00c3\u1ea0"
        U"\u0102\u1eb0\u1eae\u1eb2\u1eb4\u1eb6"
        U"\u00c2\u1ea6\u1ea4\u1ea8\u1eaa\u1eac"
        U"E\u00c8\u00c9\u1eba\u1ebc\u1eb8"
        U"\u00ca\u1ec0\u1ebe\u1ec2\u1ec4\u1ec6"
        U"I\u00cc\u00cd\u1ec8\u0128\u1eca"
        U"O\u00d2\u00d3\u1ece\u00d5\u1ecc"
        U"\u00d4\u1ed2\u1ed0\u1ed4\u1ed6\u1ed8"
        U"\u01a0\u1edc\u1eda\u1ede\u1ee0\u1ee2"
        U"U\u00d9\u00da\u1ee6\u0168\u1ee4"
        U"\u01af\u1eea\u1ee8\u1eec\u1eee\u1ef0"
        U"Y\u1ef2\u00dd\u1ef6\u1ef8\u1ef4";

    static constexpr std::u32string_view PUNCTUATION_MARKS = U",;.:\"'! ?<>=+-*/\\_~`@#$%^&(){}[]|";

    struct MarkMapping {
        char32_t marks[5]; // base, circumflex, breve, horn, dashed
    };

    static const std::unordered_map<char32_t, MarkMapping> MARKS_MAP = {
        {'a', {U'a', U'\u00e2', U'\u0103', 0, 0}},
        {U'\u00e2', {U'a', U'\u00e2', U'\u0103', 0, 0}},
        {U'\u0103', {U'a', U'\u00e2', U'\u0103', 0, 0}},
        {'e', {U'e', U'\u00ea', 0, 0, 0}},
        {U'\u00ea', {U'e', U'\u00ea', 0, 0, 0}},
        {'o', {U'o', U'\u00f4', 0, U'\u01a1', 0}},
        {U'\u00f4', {U'o', U'\u00f4', 0, U'\u01a1', 0}},
        {U'\u01a1', {U'o', U'\u00f4', 0, U'\u01a1', 0}},
        {'u', {U'u', 0, 0, U'\u01b0', 0}},
        {U'\u01b0', {U'u', 0, 0, U'\u01b0', 0}},
        {'d', {'d', 0, 0, 0, U'\u0111'}},
        {U'\u0111', {'d', 0, 0, 0, U'\u0111'}}
    };

    char32_t ToLower(char32_t c) {
        if (c >= 'A' && c <= 'Z') return c + 32;
        auto pos = VOWELS_UPPER.find(c);
        if (pos != std::u32string_view::npos) return VOWELS[pos];
        if (c == U'\u0110') return U'\u0111'; // Đ -> đ
        return c;
    }

    char32_t ToUpper(char32_t c) {
        if (c >= 'a' && c <= 'z') return c - 32;
        auto pos = VOWELS.find(c);
        if (pos != std::u32string_view::npos) return VOWELS_UPPER[pos];
        if (c == U'\u0111') return U'\u0110'; // đ -> Đ
        return c;
    }

    bool IsVowel(char32_t chr) {
        char32_t low = ToLower(chr);
        return VOWELS.find(low) != std::u32string_view::npos;
    }

    int FindVowelPosition(char32_t chr) {
        char32_t low = ToLower(chr);
        auto pos = VOWELS.find(low);
        return (pos == std::u32string_view::npos) ? -1 : (int)pos;
    }

    Tone FindToneFromChar(char32_t chr) {
        int pos = FindVowelPosition(chr);
        if (pos == -1) return ToneNone;
        return static_cast<Tone>(pos % 6);
    }

    char32_t AddToneToChar(char32_t chr, uint8_t tone) {
        int pos = FindVowelPosition(chr);
        if (pos > -1) {
            int currentTone = pos % 6;
            int offset = (int)tone - currentTone;
            bool isUpper = (chr == ToUpper(chr) && chr != ToLower(chr));
            char32_t res = VOWELS[pos + offset];
            return isUpper ? ToUpper(res) : res;
        }
        return chr;
    }

    char32_t AddMarkToChar(char32_t chr, uint8_t mark) {
        uint8_t tone = static_cast<uint8_t>(FindToneFromChar(chr));
        bool isUpper = (chr == ToUpper(chr) && chr != ToLower(chr));
        char32_t base = ToLower(AddToneToChar(chr, 0));
        char32_t marked = AddMarkToTonelessChar(base, mark);
        char32_t res = AddToneToChar(marked, tone);
        return isUpper ? ToUpper(res) : res;
    }

    char32_t AddMarkToTonelessChar(char32_t chr, uint8_t mark) {
        char32_t low = ToLower(chr);
        auto it = MARKS_MAP.find(low);
        if (it != MARKS_MAP.end()) {
            char32_t m = it->second.marks[mark];
            if (m != 0) return m;
        }
        return chr;
    }

    int FindMarkPosition(char32_t chr) {
        char32_t low = ToLower(chr);
        auto it = MARKS_MAP.find(low);
        if (it != MARKS_MAP.end()) {
            for (int i = 0; i < 5; ++i) {
                if (it->second.marks[i] == low) return i;
            }
        }
        return -1;
    }

    std::pair<Mark, bool> FindMarkFromChar(char32_t chr) {
        int pos = FindMarkPosition(chr);
        if (pos >= 0) return {static_cast<Mark>(pos), true};
        return {MarkNone, false};
    }

    bool IsVietnameseRune(char32_t chr) {
        if (FindToneFromChar(chr) != ToneNone) return true;
        return ToLower(chr) != AddMarkToTonelessChar(chr, 0);
    }

    bool IsAlpha(char32_t c) {
        char32_t low = ToLower(c);
        return (low >= 'a' && low <= 'z') || IsVietnameseRune(c);
    }

    bool IsPunctuationMark(char32_t key) {
        return PUNCTUATION_MARKS.find(key) != std::u32string_view::npos;
    }

    bool IsWordBreakSymbol(char32_t key) {
        return IsPunctuationMark(key) || (key >= '0' && key <= '9');
    }

    std::vector<char32_t> Utf8ToUtf32(std::string_view utf8) {
        std::vector<char32_t> result;
        for (size_t i = 0; i < utf8.length(); ) {
            uint32_t cp = 0;
            unsigned char c = utf8[i];
            size_t len = 0;
            if (c <= 0x7f) { cp = c; len = 1; }
            else if ((c & 0xe0) == 0xc0) { cp = c & 0x1f; len = 2; }
            else if ((c & 0xf0) == 0xe0) { cp = c & 0x0f; len = 3; }
            else if ((c & 0xf8) == 0xf0) { cp = c & 0x07; len = 4; }
            else { i++; continue; }
            
            for (size_t j = 1; j < len && i + j < utf8.length(); j++) {
                cp = (cp << 6) | (utf8[i+j] & 0x3f);
            }
            result.push_back(static_cast<char32_t>(cp));
            i += len;
        }
        return result;
    }

    std::string Utf32ToUtf8(const std::u32string& utf32) {
        std::string result;
        for (char32_t cp : utf32) {
            if (cp <= 0x7F) {
                result += (char)cp;
            } else if (cp <= 0x7FF) {
                result += (char)(0xC0 | (cp >> 6));
                result += (char)(0x80 | (cp & 0x3F));
            } else if (cp <= 0xFFFF) {
                result += (char)(0xE0 | (cp >> 12));
                result += (char)(0x80 | ((cp >> 6) & 0x3F));
                result += (char)(0x80 | (cp & 0x3F));
            } else {
                result += (char)(0xF0 | (cp >> 18));
                result += (char)(0x80 | ((cp >> 12) & 0x3F));
                result += (char)(0x80 | ((cp >> 6) & 0x3F));
                result += (char)(0x80 | (cp & 0x3F));
            }
        }
        return result;
    }

}} // namespace bamboo::engine
