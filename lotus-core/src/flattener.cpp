#include "lotus/flattener.hpp"
#include "lotus/utils.hpp"
#include <cwctype>
#include <map>
#include <memory>
#include <algorithm>

namespace lotus {

std::u32string getCanvas(const std::vector<std::shared_ptr<Transformation>>& composition, Mode mode) {
    std::u32string canvas;
    std::map<std::shared_ptr<Transformation>, std::vector<std::shared_ptr<Transformation>>> appendingMap;
    std::vector<std::shared_ptr<Transformation>> appendingList;

    bool isEnglish = (static_cast<uint32_t>(mode) & static_cast<uint32_t>(Mode::EnglishMode)) != 0;

    for (const auto& trans : composition) {
        if (isEnglish) {
            if (trans->rule.key == 0) continue;
            appendingList.push_back(trans);
        } else {
            if (trans->rule.effectType == EffectType::Appending) {
                appendingList.push_back(trans);
            } else if (trans->target) {
                appendingMap[trans->target].push_back(trans);
            }
        }
    }

    for (const auto& appendingTrans : appendingList) {
        char32_t chr;
        auto it = appendingMap.find(appendingTrans);
        const std::vector<std::shared_ptr<Transformation>>& transList = (it != appendingMap.end()) ? it->second : std::vector<std::shared_ptr<Transformation>>();

        if (isEnglish) {
            chr = appendingTrans->rule.key;
        } else {
            chr = appendingTrans->rule.result;
            for (const auto& t : transList) {
                if (t->rule.effectType == EffectType::MarkTransformation) {
                    if (t->rule.getMark() == MarkRaw) {
                        chr = appendingTrans->rule.key;
                    } else {
                        chr = AddMarkToChar(chr, t->rule.getMark());
                    }
                } else if (t->rule.effectType == EffectType::ToneTransformation) {
                    chr = AddToneToChar(chr, t->rule.getTone());
                }
            }
        }

        if (appendingTrans->isUpperCase) {
            chr = static_cast<char32_t>(std::towupper(static_cast<wint_t>(chr)));
        }
        canvas.push_back(chr);
    }

    return canvas;
}

std::u32string Flatten(const std::vector<std::shared_ptr<Transformation>>& composition, Mode mode) {
    std::u32string canvas = getCanvas(composition, mode);
    
    if (mode & Mode::MarkLess) {
        for (auto& chr : canvas) {
            chr = AddMarkToTonelessChar(chr, 0);
        }
    }
    if (mode & Mode::ToneLess) {
        for (auto& chr : canvas) {
            chr = AddToneToChar(chr, 0);
        }
    }
    
    if (mode & Mode::LowerCase) {
        for (auto& chr : canvas) {
            chr = static_cast<char32_t>(std::towlower(static_cast<wint_t>(chr)));
        }
    }
    
    return canvas;
}

} // namespace lotus
