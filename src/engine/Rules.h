#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Character.h"

namespace bamboo { namespace engine {

    enum class EffectType {
        Appending,
        MarkTransformation,
        ToneTransformation,
        Replacing
    };

    struct Rule {
        char32_t key = 0;
        uint8_t effect = 0; // Combined with EffectType (Tone or Mark value)
        EffectType effectType = EffectType::Appending;
        char32_t effectOn = 0;
        char32_t result = 0;
        std::vector<Rule> appendedRules;
    };

    struct InputMethod {
        std::string name;
        std::vector<Rule> rules;
        std::vector<char32_t> superKeys;
        std::vector<char32_t> toneKeys;
        std::vector<char32_t> keys; // All trigger keys
    };

    // Parser for the DSL used in input_method_def.go
    std::vector<Rule> ParseRules(char32_t key, std::string_view line);
    InputMethod GetInputMethod(std::string_view name);

}} // namespace bamboo::engine
