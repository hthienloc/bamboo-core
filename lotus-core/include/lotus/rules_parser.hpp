#pragma once

#include <string>
#include <vector>
#include <map>
#include "lotus/input_method.hpp"

namespace lotus {

/**
 * Parses input method definitions into functional InputMethod objects.
 */
InputMethod ParseInputMethod(const std::map<std::u16string, InputMethodDefinition>& imDef, const std::u16string& imName);

/**
 * Internal parsing helpers
 */
std::vector<Rule> ParseRules(char32_t key, const std::u16string& line);
std::vector<Rule> ParseTonelessRules(char32_t key, const std::u16string& line);
std::vector<Rule> ParseToneLessRule(char32_t key, char32_t effectiveOn, char32_t result, Mark effect);
std::pair<Rule, bool> GetAppendingRule(char32_t key, const std::u16string& value);

} // namespace lotus
