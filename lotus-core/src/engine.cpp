#include "lotus/engine.hpp"
#include "lotus/engine_utils.hpp"
#include "lotus/utils.hpp"
#include "lotus/flattener.hpp"
#include "lotus/rules_parser.hpp"
#include <algorithm>
#include <cwctype>
#include <map>
#include <utility>

namespace lotus {

LotusEngine::LotusEngine(const InputMethod& inputMethod, uint32_t flags)
    : inputMethod(inputMethod), flags(flags) {}

LotusEngine::~LotusEngine() = default;

void LotusEngine::SetFlag(uint32_t flag) {
    this->flags = flag;
}

uint32_t LotusEngine::GetFlag() const {
    return this->flags;
}

InputMethod LotusEngine::GetInputMethod() const {
    return this->inputMethod;
}

void LotusEngine::ProcessKey(char32_t key, Mode mode) {
    char32_t lowerKey = static_cast<char32_t>(std::towlower(static_cast<wint_t>(key)));
    bool isUpperCase = std::iswupper(static_cast<wint_t>(key));
    
    if ((mode & Mode::EnglishMode) || !CanProcessKey(lowerKey)) {
        auto trans = NewAppendingTrans(lowerKey, isUpperCase);
        if (mode & Mode::InReverseOrder) {
            composition.insert(composition.begin(), trans);
        } else {
            composition.push_back(trans);
        }
        return;
    }
    
    updateComposition(lowerKey, isUpperCase);
}

void LotusEngine::RebuildEngineFromText(const std::u16string& text) {
    Reset();
    composition = RebuildCompositionFromText(ToU32(text), (flags & EstdToneStyle) != 0);
}

void LotusEngine::ProcessString(const std::u16string& str, Mode mode) {
    std::u32string u32str = ToU32(str);
    for (char32_t key : u32str) {
        ProcessKey(key, mode);
    }
}

std::u16string LotusEngine::GetProcessedString(Mode mode) {
    std::vector<std::shared_ptr<Transformation>> tmp;
    
    if (mode & Mode::FullText) {
        tmp = composition;
    } else {
        auto [prefix, lastWord] = ExtractLastWord(composition, inputMethod.appendingKeys);
        tmp = lastWord;
    }
    
    Mode flattenMode = mode;
    if (mode & Mode::PunctuationMode) {
        flattenMode = Mode::VietnameseMode;
    }
    
    return ToU16(Flatten(tmp, flattenMode));
}

bool LotusEngine::IsValid(bool inputIsFullComplete) {
    auto [prefix, lastWord] = ExtractLastWord(composition, inputMethod.keys);
    return lotus::IsValid(lastWord, inputIsFullComplete);
}

bool LotusEngine::CanProcessKey(char32_t key) const {
    for (char32_t k : inputMethod.keys) {
        if (k == key) return true;
    }
    return false;
}

void LotusEngine::RestoreLastWord(bool toVietnamese) {
    auto [previous, lastComb] = ExtractLastWord(composition, inputMethod.keys);
    if (lastComb.empty()) return;
    if (!toVietnamese) {
        auto broken = BreakComposition(lastComb);
        composition = previous;
        for (char32_t k : broken) composition.push_back(NewAppendingTrans(k, false));
    } else {
        std::vector<std::shared_ptr<Transformation>> newComp;
        for (const auto& tnx : lastComb) {
            newComp = updateComposition(newComp, tnx->rule.key, tnx->isUpperCase);
        }
        composition = previous;
        composition.insert(composition.end(), newComp.begin(), newComp.end());
    }
}

void LotusEngine::RemoveLastChar(bool refreshLastTone) {
    auto lastAppending = FindLastAppendingTrans(composition);
    if (!lastAppending) return;
    if (!CanProcessKey(lastAppending->rule.key)) {
        composition.pop_back();
        return;
    }
    auto [previous, lastComb] = ExtractLastWord(composition, inputMethod.keys);
    std::vector<std::shared_ptr<Transformation>> newComb;
    for (const auto& t : lastComb) {
        if (t->target == lastAppending || t == lastAppending) continue;
        newComb.push_back(t);
    }
    
    if (refreshLastTone) {
        auto refreshed = refreshLastToneTargetLogic(newComb);
        newComb.insert(newComb.end(), refreshed.begin(), refreshed.end());
    }
    
    composition = previous;
    composition.insert(composition.end(), newComb.begin(), newComb.end());
}

void LotusEngine::Reset() {
    composition.clear();
}

std::vector<Rule> LotusEngine::getApplicableRules(char32_t key) const {
    std::vector<Rule> applicable;
    for (const auto& rule : inputMethod.rules) {
        if (rule.key == key) {
            applicable.push_back(rule);
        }
    }
    return applicable;
}

std::pair<std::shared_ptr<Transformation>, Rule> LotusEngine::findTargetByKey(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t key) {
    return FindTarget(comp, getApplicableRules(key), flags);
}

std::vector<std::shared_ptr<Transformation>> LotusEngine::generateTransformations(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t lowerKey, bool isUpperCase) {
    auto result = GenerateTransformations(comp, getApplicableRules(lowerKey), flags, lowerKey, isUpperCase);
    if (result.empty()) {
        result = GenerateFallbackTransformations(comp, getApplicableRules(lowerKey), lowerKey, isUpperCase);
        std::vector<std::shared_ptr<Transformation>> newComp = comp;
        newComp.insert(newComp.end(), result.begin(), result.end());
        
        auto virtualTrans = applyUowShortcut(newComp);
        if (virtualTrans) result.push_back(virtualTrans);
    }
    
    std::vector<std::shared_ptr<Transformation>> newComp = comp;
    newComp.insert(newComp.end(), result.begin(), result.end());
    auto refreshed = refreshLastToneTargetLogic(newComp);
    result.insert(result.end(), refreshed.begin(), refreshed.end());
    
    return result;
}

std::vector<std::shared_ptr<Transformation>> LotusEngine::updateComposition(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t lowerKey, bool isUpperCase) {
    auto [previous, lastSyllable] = ExtractLastSyllable(comp);
    auto generated = generateTransformations(lastSyllable, lowerKey, isUpperCase);
    std::vector<std::shared_ptr<Transformation>> result = lastSyllable;
    for (const auto& t : generated) result.push_back(t);
    return result;
}

void LotusEngine::updateComposition(char32_t lowerKey, bool isUpperCase) {
    auto [previous, lastSyllable] = ExtractLastSyllable(composition);
    auto generated = generateTransformations(lastSyllable, lowerKey, isUpperCase);
    for (const auto& t : generated) {
        composition.push_back(t);
    }
}

std::shared_ptr<Transformation> LotusEngine::applyUowShortcut(const std::vector<std::shared_ptr<Transformation>>& syllable) {
    std::u32string str = Flatten(syllable, Mode::ToneLess | Mode::LowerCase);
    if (!inputMethod.superKeys.empty() && str.find(U"uơ") != std::u32string::npos) {
        auto [target, rule] = findTargetByKey(syllable, inputMethod.superKeys[0]);
        if (target) {
            rule.key = 0;
            auto trans = std::make_shared<Transformation>();
            trans->rule = rule;
            trans->target = target;
            return trans;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Transformation>> LotusEngine::refreshLastToneTargetLogic(const std::vector<std::shared_ptr<Transformation>>& syllable) {
    if ((flags & EfreeToneMarking) && lotus::IsValid(syllable, false)) {
        return RefreshLastToneTarget(const_cast<std::vector<std::shared_ptr<Transformation>>&>(syllable), (flags & EstdToneStyle) != 0);
    }
    return {};
}

std::unique_ptr<IEngine> CreateEngine(const InputMethod& inputMethod, uint32_t flags) {
    return std::make_unique<LotusEngine>(inputMethod, flags);
}

} // namespace lotus
