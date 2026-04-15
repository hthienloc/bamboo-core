#include "lotus/rules_parser.hpp"
#include "lotus/utils.hpp"
#include <algorithm>
#include <codecvt>
#include <locale>

namespace lotus {

static std::map<std::u16string, Tone> TONES_MAP = {
    {u"XoaDauThanh", ToneNone},
    {u"DauSac", ToneAcute},
    {u"DauHuyen", ToneGrave},
    {u"DauNga", ToneTilde},
    {u"DauNang", ToneDot},
    {u"DauHoi", ToneHook}
};

InputMethod ParseInputMethod(const std::u16string& name, const InputMethodDefinition& def) {
    InputMethod im;
    im.name = name;
    
    // Parse rules
    if (def.count(u"ToneRules")) im.rules = ParseRules(0, def.at(u"ToneRules"));
    // Add other rules... for MVP, let's just parse what we need.
    
    // Telex-specific (simplified)
    for (const auto& [key, value] : def) {
        if (key.length() == 1) { // Single key rules
            char32_t k = static_cast<char32_t>(key[0]);
            std::vector<Rule> rs = ParseRules(k, value);
            im.rules.insert(im.rules.end(), rs.begin(), rs.end());
            im.keys.push_back(k); // Add to handled keys
        }
    }
    
    // Add super keys, etc.
    return im;
}

InputMethod ParseInputMethod(const std::map<std::u16string, InputMethodDefinition>& imDef, const std::u16string& imName) {
    auto it = imDef.find(imName);
    if (it == imDef.end()) return {};

    InputMethod im;
    im.name = imName;

    for (const auto& [keyStr, line] : it->second) {
        if (keyStr.empty()) continue;
        char32_t key = static_cast<char32_t>(keyStr[0]);
        
        auto rules = ParseRules(key, line);
        im.rules.insert(im.rules.end(), rules.begin(), rules.end());
        
        std::u16string lowerLine = line;
        std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
        if (lowerLine.find(u"uo") != std::u16string::npos) {
            im.superKeys.push_back(key);
        }
        im.keys.push_back(key);
    }

    for (const auto& rule : im.rules) {
        if (rule.effectType == EffectType::Appending) {
            im.appendingKeys.push_back(rule.key);
        }
        if (rule.effectType == EffectType::ToneTransformation) {
            im.toneKeys.push_back(rule.key);
        }
    }

    return im;
}

std::vector<Rule> ParseRules(char32_t key, const std::u16string& line) {
    auto it = TONES_MAP.find(line);
    if (it != TONES_MAP.end()) {
        Rule rule;
        rule.key = key;
        rule.effectType = EffectType::ToneTransformation;
        rule.setTone(it->second);
        return {rule};
    }
    return ParseTonelessRules(key, line);
}

std::vector<Rule> ParseTonelessRules(char32_t key, const std::u16string& line) {
    std::vector<Rule> rules;
    std::u32string u32line = ToU32(line);
    for (auto& c : u32line) {
        if (c < 128) c = static_cast<char32_t>(std::tolower(static_cast<int>(c)));
        else if (c == U'Đ') c = U'đ';
        else if (c == U'Â') c = U'â';
        else if (c == U'Ă') c = U'ă';
        else if (c == U'Ê') c = U'ê';
        else if (c == U'Ô') c = U'ô';
        else if (c == U'Ơ') c = U'ơ';
        else if (c == U'Ư') c = U'ư';
        // Simplified for MVP, should use a proper Unicode tolower
    }
    
    // Simple DSL parser: ([a-zA-Z]+)_(\p{L}+)([_\p{L}]*)
    size_t underscorePos = u32line.find(U'_');
    if (underscorePos != std::u32string::npos && underscorePos > 0) {
        std::u32string effectiveOns = u32line.substr(0, underscorePos);
        std::u32string rest = u32line.substr(underscorePos + 1);
        
        // Find where results end and appending starts
        size_t resultsEnd = 0;
        while (resultsEnd < rest.length() && rest[resultsEnd] != U'_') {
            resultsEnd++;
        }
        
        std::u32string results = rest.substr(0, resultsEnd);
        std::u32string appendingPart = (resultsEnd < rest.length()) ? rest.substr(resultsEnd) : U"";
        
        for (size_t i = 0; i < effectiveOns.length() && i < results.length(); ++i) {
            Mark effect = FindMarkFromChar(results[i]);
            auto subRules = ParseToneLessRule(key, effectiveOns[i], results[i], effect);
            rules.insert(rules.end(), subRules.begin(), subRules.end());
        }
        
        if (!appendingPart.empty()) {
            auto [rule, ok] = GetAppendingRule(key, std::u16string(appendingPart.begin(), appendingPart.end()));
            if (ok) rules.push_back(rule);
        }
    } else {
        auto [rule, ok] = GetAppendingRule(key, line);
        if (ok) rules.push_back(rule);
    }
    
    return rules;
}

std::vector<Rule> ParseToneLessRule(char32_t key, char32_t effectiveOn, char32_t result, Mark effect) {
    std::vector<Rule> rules;
    std::vector<Tone> tones = {ToneNone, ToneDot, ToneAcute, ToneGrave, ToneHook, ToneTilde};
    
    for (char32_t chr : GetMarkFamily(effectiveOn)) {
        if (chr == result) {
            Rule rule;
            rule.key = key;
            rule.effectType = EffectType::MarkTransformation;
            rule.effect = 0;
            rule.effectOn = result;
            rule.result = effectiveOn;
            rules.push_back(rule);
        } else if (IsVowel(chr)) {
            for (auto tone : tones) {
                Rule rule;
                rule.key = key;
                rule.effectType = EffectType::MarkTransformation;
                rule.effectOn = AddToneToChar(chr, tone);
                rule.setMark(effect);
                rule.result = AddToneToChar(result, tone);
                rules.push_back(rule);
            }
        } else {
            Rule rule;
            rule.key = key;
            rule.effectType = EffectType::MarkTransformation;
            rule.effectOn = chr;
            rule.setMark(effect);
            rule.result = result;
            rules.push_back(rule);
        }
    }
    return rules;
}

std::pair<Rule, bool> GetAppendingRule(char32_t key, const std::u16string& value) {
    std::u32string u32val = ToU32(value);
    if (u32val.empty()) return {{}, false};
    
    // Match (_?)_(\p{L}+)
    size_t startIdx = 0;
    if (u32val[0] == U'_') {
        startIdx = 1;
        if (u32val.length() > 1 && u32val[1] == U'_') {
            startIdx = 2;
        }
    } else {
        return {{}, false};
    }
    
    if (startIdx >= u32val.length()) return {{}, false};
    
    std::u32string chars = u32val.substr(startIdx);
    Rule rule;
    rule.key = key;
    rule.effectType = EffectType::Appending;
    rule.effectOn = chars[0];
    rule.result = chars[0];
    
    for (size_t i = 1; i < chars.length(); ++i) {
        Rule sub;
        sub.key = key;
        sub.effectType = EffectType::Appending;
        sub.effectOn = chars[i];
        sub.result = chars[i];
        rule.appendedRules.push_back(sub);
    }
    
    return {rule, true};
}

} // namespace lotus
