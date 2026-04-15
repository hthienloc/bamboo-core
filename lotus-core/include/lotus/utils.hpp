#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "lotus/input_method.hpp"

namespace lotus {

std::u32string ToU32(const std::u16string& s);
std::u16string ToU16(const std::u32string& s);

bool IsSpace(char32_t key);
bool IsPunctuationMark(char32_t key);
bool IsWordBreakSymbol(char32_t key);
bool IsVowel(char32_t chr);
int FindVowelPosition(char32_t chr);
std::vector<char32_t> GetMarkFamily(char32_t chr);
int FindMarkPosition(char32_t chr);
Mark FindMarkFromChar(char32_t chr);
char32_t AddMarkToTonelessChar(char32_t chr, uint8_t mark);
char32_t AddMarkToChar(char32_t chr, uint8_t mark);
bool IsAlpha(char32_t c);
Tone FindToneFromChar(char32_t chr);
char32_t AddToneToChar(char32_t chr, uint8_t tone);
bool IsVietnameseRune(char32_t lowerKey);
bool HasAnyVietnameseRune(const std::u32string& word);
bool HasAnyVietnameseVowel(const std::u32string& word);

} // namespace lotus
