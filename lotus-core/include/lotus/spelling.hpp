#pragma once

#include <string>
#include <vector>

namespace lotus {

/**
 * Validates Vietnamese spelling (Consonant-Vowel-Consonant).
 */
struct CVC {
    std::u32string fc;
    std::u32string vo;
    std::u32string lc;
};

CVC ParseCVC(const std::u32string& word);
bool IsValidCVC(const CVC& cvc, bool inputIsFullComplete);
bool IsValidCVC(const std::u32string& fc, const std::u32string& vo, const std::u32string& lc, bool inputIsFullComplete);

} // namespace lotus
