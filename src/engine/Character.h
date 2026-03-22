#pragma once
#include <cstdint>
#include <string_view>
#include <vector>

namespace bamboo { namespace engine {

    // Vietnamese Tone definitions
    enum Tone : uint8_t {
        ToneNone = 0,
        ToneHuyen = 1,
        ToneSac = 2,
        ToneHoi = 3,
        ToneNga = 4,
        ToneNang = 5
    };

    // Vietnamese Mark definitions (HORN, BREVE, CIRCUMFLEX, etc.)
    enum Mark : uint8_t {
        MarkNone = 0,
        MarkCircumflex = 1, // Â, Ê, Ô
        MarkBreve = 2,      // Ă
        MarkHorn = 3,       // Ơ, Ư
        MarkDashed = 4      // Đ
    };

    char32_t ToLower(char32_t c);
    char32_t ToUpper(char32_t c);

    bool IsVowel(char32_t chr);
    int FindVowelPosition(char32_t chr);
    Tone FindToneFromChar(char32_t chr);
    char32_t AddToneToChar(char32_t chr, uint8_t tone);
    char32_t AddMarkToChar(char32_t chr, uint8_t mark);
    char32_t AddMarkToTonelessChar(char32_t chr, uint8_t mark);
    std::pair<Mark, bool> FindMarkFromChar(char32_t chr);
    int FindMarkPosition(char32_t chr);
    bool IsVietnameseRune(char32_t chr);
    bool IsAlpha(char32_t c);
    bool IsPunctuationMark(char32_t key);
    bool IsWordBreakSymbol(char32_t key);

    std::vector<char32_t> Utf8ToUtf32(std::string_view utf8);
    std::string Utf32ToUtf8(const std::u32string& utf32);

}} // namespace bamboo::engine
