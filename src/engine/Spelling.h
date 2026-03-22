#pragma once
#include <string_view>

namespace bamboo { namespace engine {

    /**
     * @brief Validate if a Vietnamese CVC (Consonant-Vowel-Consonant) combination is phonetically valid.
     * 
     * @param fc First consonant (e.g., "tr", "ngh")
     * @param vo Vowel part (e.g., "oa", "uyê")
     * @param lc Last consonant (e.g., "nh", "ng")
     * @param inputIsFullComplete Whether the input is considered a full and complete syllable.
     * @return true if valid, false otherwise.
     */
    bool IsValidCVC(std::u32string_view fc, std::u32string_view vo, std::u32string_view lc, bool inputIsFullComplete);

}} // namespace bamboo::engine
