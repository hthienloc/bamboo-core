#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "lotus/input_method.hpp"

namespace lotus {

// Bitwise flags for engine behavior
const uint32_t EfreeToneMarking  = 1 << 0;
const uint32_t EstdToneStyle     = 1 << 1;
const uint32_t EautoCorrectEnabled = 1 << 2;
const uint32_t EbackspaceAction  = 1 << 3;
const uint32_t EstdFlags = EfreeToneMarking | EstdToneStyle | EautoCorrectEnabled;

std::shared_ptr<Transformation> NewAppendingTrans(char32_t key, bool isUpperCase);

std::vector<std::shared_ptr<Transformation>> GenerateAppendingTrans(char32_t key, bool isUpperCase);

std::vector<std::shared_ptr<Transformation>> GenerateUndoTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags);

std::pair<std::shared_ptr<Transformation>, Rule> FindTarget(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags);

std::vector<std::shared_ptr<Transformation>> GenerateTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags, char32_t lowerKey, bool isUpperCase);

std::vector<std::shared_ptr<Transformation>> GenerateFallbackTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, char32_t lowerKey, bool isUpperCase);

std::pair<std::vector<std::shared_ptr<Transformation>>, std::vector<std::shared_ptr<Transformation>>> ExtractLastWord(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<char32_t>& appendingKeys);

std::pair<std::vector<std::shared_ptr<Transformation>>, std::vector<std::shared_ptr<Transformation>>> ExtractLastSyllable(const std::vector<std::shared_ptr<Transformation>>& composition);

std::shared_ptr<Transformation> FindLastAppendingTrans(const std::vector<std::shared_ptr<Transformation>>& composition);

std::vector<std::shared_ptr<Transformation>> RefreshLastToneTarget(std::vector<std::shared_ptr<Transformation>>& syllable, bool isStdStyle);

std::vector<char32_t> BreakComposition(const std::vector<std::shared_ptr<Transformation>>& composition);

bool IsValid(const std::vector<std::shared_ptr<Transformation>>& composition, bool inputIsFullComplete);

std::vector<std::shared_ptr<Transformation>> RebuildCompositionFromText(std::u32string text, bool stdStyle);

// Additional helpers needed by flattener or internal logic
std::shared_ptr<Transformation> FindRootTarget(std::shared_ptr<Transformation> target);
std::vector<std::shared_ptr<Transformation>> GetRightMostVowels(const std::vector<std::shared_ptr<Transformation>>& composition);
std::shared_ptr<Transformation> FindToneTarget(const std::vector<std::shared_ptr<Transformation>>& composition, bool stdStyle);

} // namespace lotus
