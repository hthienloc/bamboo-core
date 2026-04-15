#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace lotus {

// Vietnamese Vowels (Precomposed)
inline const std::u32string VOWELS = U"aàáảãạăằắẳẵặâầấẩẫậeèéẻẽẹêềếểễệiìíỉĩịoòóỏõọôồốổỗộơờớởỡợuùúủũụưừứửữựyỳýỷỹỵ";

// Punctuation Marks
inline const std::vector<char32_t> PUNCTUATION_MARKS = {
    ',', ';', ':', '.', '"', '\'', '!', '?', ' ',
    '<', '>', '=', '+', '-', '*', '/', '\\',
    '_', '~', '`', '@', '#', '$', '%', '^', '&', '(', ')', '{', '}', '[', ']',
    '|'
};

// Marks Mapping
inline const std::unordered_map<char32_t, std::u32string> MARKS_MAPS = {
    {'a', U"aâă__"},
    {U'â', U"aâă__"},
    {U'ă', U"aâă__"},
    {'e', U"eê___"},
    {U'ê', U"eê___"},
    {'o', U"oô_ơ_"},
    {U'ô', U"oô_ơ_"},
    {U'ơ', U"oô_ơ_"},
    {'u', U"u__ư_"},
    {U'ư', U"u__ư_"},
    {'d', U"d___đ"},
    {U'đ', U"d___đ"}
};

} // namespace lotus
