#include "Rules.h"
#include <unordered_map>
#include <vector>
#include <string_view>
#include <iostream>

namespace bamboo { namespace engine {

    static const std::unordered_map<std::string_view, Tone> TONES_MAP = {
        {"XoaDauThanh", ToneNone},
        {"DauSac", ToneSac},
        {"DauHuyen", ToneHuyen},
        {"DauHoi", ToneHoi},
        {"DauNga", ToneNga},
        {"DauNang", ToneNang}
    };

    std::vector<Rule> ParseRules(char32_t key, std::string_view line) {
        std::vector<Rule> rules;
        auto it = TONES_MAP.find(line);
        if (it != TONES_MAP.end()) {
            Rule r;
            r.key = key;
            r.effectType = EffectType::ToneTransformation;
            r.effect = it->second;
            rules.push_back(r);
            return rules;
        }

        size_t underscorePos = line.find('_');
        if (underscorePos != std::string_view::npos) {
            std::string_view parts1 = line.substr(0, underscorePos);
            std::string_view rest = line.substr(underscorePos + 1);
            size_t secondUnderscore = rest.find('_');
            std::string_view parts2 = (secondUnderscore == std::string_view::npos) ? rest : rest.substr(0, secondUnderscore);
            
            auto v1 = Utf8ToUtf32(parts1);
            auto v2 = Utf8ToUtf32(parts2);
            
            for (size_t i = 0; i < v1.size() && i < v2.size(); ++i) {
                char32_t onChar = ToLower(v1[i]);
                char32_t resultChar = ToLower(v2[i]);
                int markPos = FindMarkPosition(resultChar);
                
                if (markPos != -1) {
                    for (uint8_t t = 0; t <= 5; ++t) {
                        Rule r;
                        r.key = key;
                        r.effectType = EffectType::MarkTransformation;
                        r.effectOn = AddToneToChar(onChar, t);
                        r.effect = static_cast<uint8_t>(markPos);
                        r.result = AddToneToChar(resultChar, t);
                        rules.push_back(r);
                    }
                }
            }
        } else {
            Rule r;
            r.key = key;
            r.effectType = EffectType::Appending;
            r.result = key;
            rules.push_back(r);
        }
        return rules;
    }

    InputMethod GetInputMethod(std::string_view name) {
        InputMethod im;
        im.name = std::string(name);
        
        static const std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::string_view>> DEFS = {
            {"Telex", {{"z", "XoaDauThanh"}, {"s", "DauSac"}, {"f", "DauHuyen"}, {"r", "DauHoi"}, {"x", "DauNga"}, {"j", "DauNang"}, {"a", "A_Â"}, {"e", "E_Ê"}, {"o", "O_Ô"}, {"w", "UOA_ƯƠĂ"}, {"d", "D_Đ"}}},
            {"VNI", {{"0", "XoaDauThanh"}, {"1", "DauSac"}, {"2", "DauHuyen"}, {"3", "DauHoi"}, {"4", "DauNga"}, {"5", "DauNang"}, {"6", "AEO_ÂÊÔ"}, {"7", "UO_ƯƠ"}, {"8", "A_Ă"}, {"9", "D_Đ"}}}
        };

        auto it = DEFS.find(name);
        if (it != DEFS.end()) {
            for (const auto& [kStr, line] : it->second) {
                auto kVec = Utf8ToUtf32(kStr);
                if (kVec.empty()) continue;
                char32_t k = kVec[0];
                auto parsed = ParseRules(k, line);
                im.rules.insert(im.rules.end(), parsed.begin(), parsed.end());
                im.keys.push_back(k);
                
                if (line.find("uo") != std::string_view::npos || line.find("UO") != std::string_view::npos) {
                    im.superKeys.push_back(k);
                }
                
                for (const auto& r : parsed) {
                    if (r.effectType == EffectType::ToneTransformation) {
                        im.toneKeys.push_back(k);
                        break;
                    }
                }
            }
        }
        return im;
    }

}} // namespace bamboo::engine
