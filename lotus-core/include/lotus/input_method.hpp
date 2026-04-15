#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>

namespace lotus {

enum class Mode : uint32_t {
    VietnameseMode = 1 << 0,
    EnglishMode = 1 << 1,
    ToneLess = 1 << 2,
    MarkLess = 1 << 3,
    LowerCase = 1 << 4,
    FullText = 1 << 5,
    PunctuationMode = 1 << 6,
    InReverseOrder = 1 << 7
};

inline Mode operator|(Mode a, Mode b) {
    return static_cast<Mode>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool operator&(Mode a, Mode b) {
    return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}

enum class EffectType : int {
    Appending = 0,
    MarkTransformation = 1,
    ToneTransformation = 2,
    Replacing = 3
};

using Mark = uint8_t;
const Mark MarkNone = 0;
const Mark MarkHat = 1;
const Mark MarkBreve = 2;
const Mark MarkHorn = 3;
const Mark MarkDash = 4;
const Mark MarkRaw = 5;

using Tone = uint8_t;
const Tone ToneNone = 0;
const Tone ToneGrave = 1;
const Tone ToneAcute = 2;
const Tone ToneHook = 3;
const Tone ToneTilde = 4;
const Tone ToneDot = 5;

struct Rule {
    char32_t key;
    uint8_t effect; // (Tone or Mark)
    EffectType effectType;
    char32_t effectOn;
    char32_t result;
    std::vector<Rule> appendedRules;

    void setTone(Tone tone) { effect = static_cast<uint8_t>(tone); }
    void setMark(Mark mark) { effect = static_cast<uint8_t>(mark); }
    Tone getTone() const { return static_cast<Tone>(effect); }
    Mark getMark() const { return static_cast<Mark>(effect); }
};

struct Transformation {
    Rule rule;
    std::shared_ptr<Transformation> target = nullptr;
    bool isUpperCase = false;
};

struct InputMethod {
    std::u16string name;
    std::vector<Rule> rules;
    std::vector<char32_t> superKeys;
    std::vector<char32_t> toneKeys;
    std::vector<char32_t> appendingKeys;
    std::vector<char32_t> keys;
};

using InputMethodDefinition = std::map<std::u16string, std::u16string>;

extern const std::map<std::u16string, InputMethodDefinition> INPUT_METHOD_DEFINITIONS;

} // namespace lotus
