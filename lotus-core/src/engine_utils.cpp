#include "lotus/engine_utils.hpp"
#include "lotus/utils.hpp"
#include "lotus/flattener.hpp"
#include "lotus/spelling.hpp"
#include <algorithm>
#include <map>

namespace lotus {

std::shared_ptr<Transformation> FindRootTarget(std::shared_ptr<Transformation> target) {
    while (target && target->target) target = target->target;
    return target;
}

void ExtractCvcTrans(const std::vector<std::shared_ptr<Transformation>>& composition,
                    std::vector<std::shared_ptr<Transformation>>& fc,
                    std::vector<std::shared_ptr<Transformation>>& vo,
                    std::vector<std::shared_ptr<Transformation>>& lc) {
    std::map<std::shared_ptr<Transformation>, std::vector<std::shared_ptr<Transformation>>> transMap;
    std::vector<std::shared_ptr<Transformation>> appendingList;
    for (const auto& trans : composition) {
        if (trans->rule.effectType == EffectType::Appending) appendingList.push_back(trans);
        else if (trans->target) transMap[FindRootTarget(trans)].push_back(trans);
    }
    std::vector<std::shared_ptr<Transformation>> afc, avo, alc;
    size_t i = 0;
    while (i < appendingList.size()) {
        std::vector<std::shared_ptr<Transformation>> col = {appendingList[i]};
        auto it = transMap.find(appendingList[i]);
        if (it != transMap.end()) col.insert(col.end(), it->second.begin(), it->second.end());
        std::u32string flat = Flatten(col, Mode::VietnameseMode | Mode::ToneLess | Mode::LowerCase | Mode::MarkLess);
        if (flat.empty()) { i++; continue; }
        if (!IsVowel(flat[0])) { afc.push_back(appendingList[i]); i++; } else break;
    }
    while (i < appendingList.size()) {
        std::vector<std::shared_ptr<Transformation>> col = {appendingList[i]};
        auto it = transMap.find(appendingList[i]);
        if (it != transMap.end()) col.insert(col.end(), it->second.begin(), it->second.end());
        std::u32string flat = Flatten(col, Mode::VietnameseMode | Mode::ToneLess | Mode::LowerCase | Mode::MarkLess);
        if (flat.empty()) { i++; continue; }
        if (IsVowel(flat[0])) { avo.push_back(appendingList[i]); i++; } else break;
    }
    while (i < appendingList.size()) { alc.push_back(appendingList[i]); i++; }
    for (auto& r : afc) { fc.push_back(r); auto it = transMap.find(r); if (it != transMap.end()) fc.insert(fc.end(), it->second.begin(), it->second.end()); }
    for (auto& r : avo) { vo.push_back(r); auto it = transMap.find(r); if (it != transMap.end()) vo.insert(vo.end(), it->second.begin(), it->second.end()); }
    for (auto& r : alc) { lc.push_back(r); auto it = transMap.find(r); if (it != transMap.end()) lc.insert(lc.end(), it->second.begin(), it->second.end()); }
}

std::vector<std::shared_ptr<Transformation>> GetRightMostVowels(const std::vector<std::shared_ptr<Transformation>>& composition) {
    std::vector<std::shared_ptr<Transformation>> fc, vo, lc;
    ExtractCvcTrans(composition, fc, vo, lc);
    return vo;
}

std::shared_ptr<Transformation> FindToneTarget(const std::vector<std::shared_ptr<Transformation>>& composition, bool stdStyle) {
    std::vector<std::shared_ptr<Transformation>> fc, vo, lc;
    ExtractCvcTrans(composition, fc, vo, lc);
    std::vector<std::shared_ptr<Transformation>> vowels;
    for (const auto& t : vo) if (t->rule.effectType == EffectType::Appending) vowels.push_back(t);
    if (vowels.empty()) return nullptr;
    if (vowels.size() == 1) return vowels[0];
    size_t targetIdx = vowels.size() - 1;
    if (vowels.size() == 2) {
        if (!lc.empty()) targetIdx = 1;
        else {
            std::u32string vStr = Flatten(vo, Mode::VietnameseMode | Mode::ToneLess | Mode::LowerCase | Mode::MarkLess);
            if (vStr == U"oa" || vStr == U"oe" || vStr == U"uy" || vStr == U"ue" || vStr == U"uo") return vowels[1];
            targetIdx = 0;
        }
    } else if (vowels.size() == 3) {
        std::u32string vStr = Flatten(vo, Mode::VietnameseMode | Mode::ToneLess | Mode::LowerCase | Mode::MarkLess);
        if (vStr == U"uye" || vStr == U"uyê") return vowels[2];
        targetIdx = 1;
    }
    return vowels[targetIdx];
}

bool IsFree(const std::vector<std::shared_ptr<Transformation>>& composition, std::shared_ptr<Transformation> trans, EffectType effectType) {
    for (const auto& t : composition) if (t->target == trans && t->rule.effectType == effectType) return false;
    return true;
}

std::shared_ptr<Transformation> NewAppendingTrans(char32_t key, bool isUpperCase) {
    auto trans = std::make_shared<Transformation>();
    trans->rule.key = key; trans->rule.result = key; trans->rule.effectOn = key;
    trans->rule.effectType = EffectType::Appending; trans->isUpperCase = isUpperCase;
    return trans;
}

std::vector<std::shared_ptr<Transformation>> GenerateAppendingTrans(char32_t key, bool isUpperCase) {
    return {NewAppendingTrans(key, isUpperCase)};
}

std::pair<std::shared_ptr<Transformation>, Rule> findMarkTarget(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& rules) {
    std::u32string currentStr = Flatten(composition, Mode::VietnameseMode);
    for (int i = static_cast<int>(composition.size()) - 1; i >= 0; i--) {
        auto trans = composition[i];
        for (const auto& rule : rules) {
            if (rule.effectType != EffectType::MarkTransformation) continue;
            if (trans->rule.result == rule.effectOn) {
                auto target = FindRootTarget(trans);
                auto tmpComp = composition;
                auto t = std::make_shared<Transformation>(); t->target = target; t->rule = rule;
                tmpComp.push_back(t);
                if (currentStr == Flatten(tmpComp, Mode::VietnameseMode)) continue;
                if (IsValid(tmpComp, false)) return {target, rule};
            }
        }
    }
    return {nullptr, {}};
}

std::pair<std::shared_ptr<Transformation>, Rule> FindTarget(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags) {
    std::u32string currentStr = Flatten(composition, Mode::VietnameseMode);
    for (const auto& rule : applicableRules) {
        if (rule.effectType != EffectType::ToneTransformation) continue;
        std::shared_ptr<Transformation> target = nullptr;
        if (flags & EfreeToneMarking) target = FindToneTarget(composition, (flags & EstdToneStyle) != 0);
        else {
            auto lastApp = FindLastAppendingTrans(composition);
            if (lastApp && IsVowel(lastApp->rule.effectOn)) target = lastApp;
        }
        if (!target) continue;
        auto tmpComp = composition;
        auto t = std::make_shared<Transformation>(); t->target = target; t->rule = rule;
        tmpComp.push_back(t);
        if (currentStr == Flatten(tmpComp, Mode::VietnameseMode)) continue;
        return {target, rule};
    }
    return findMarkTarget(composition, applicableRules);
}

std::vector<std::shared_ptr<Transformation>> GenerateUndoTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags) {
    std::vector<std::shared_ptr<Transformation>> transformations;
    for (const auto& rule : applicableRules) {
        if (rule.effectType == EffectType::ToneTransformation) {
            auto target = FindToneTarget(composition, (flags & EstdToneStyle) != 0);
            if (target && rule.effect == ToneNone) {
                auto trans = std::make_shared<Transformation>(); trans->target = target; trans->rule.effectType = EffectType::ToneTransformation; trans->rule.setTone(ToneNone);
                transformations.push_back(trans);
            }
        }
    }
    return transformations;
}

std::vector<std::shared_ptr<Transformation>> GenerateTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, uint32_t flags, char32_t lowerKey, bool isUpperCase) {
    if (!composition.empty()) {
        auto lastRoot = FindRootTarget(composition.back());
        std::vector<std::shared_ptr<Transformation>> col = {lastRoot};
        for (const auto& t : composition) if (t->target == lastRoot) col.push_back(t);
        std::u32string res = Flatten(col, Mode::VietnameseMode);
        if (!res.empty() && lastRoot->rule.key == lowerKey && lastRoot->rule.key != res[0]) {
            auto trans = std::make_shared<Transformation>(); trans->target = lastRoot; trans->rule.effectType = EffectType::MarkTransformation; trans->rule.setMark(MarkRaw);
            return {trans, NewAppendingTrans(lowerKey, isUpperCase)};
        }
    }
    auto undo = GenerateUndoTransformations(composition, applicableRules, flags);
    if (!undo.empty()) return undo;
    auto [target, rule] = FindTarget(composition, applicableRules, flags);
    if (target) {
        auto trans = std::make_shared<Transformation>(); trans->rule = rule; trans->target = target; trans->isUpperCase = isUpperCase;
        std::vector<std::shared_ptr<Transformation>> result = {trans};
        std::vector<std::shared_ptr<Transformation>> newComp = composition; newComp.push_back(trans);
        for (const auto& ar : rule.appendedRules) {
            auto [tTarget, tRule] = FindTarget(newComp, {ar}, flags);
            if (tTarget) { auto s = std::make_shared<Transformation>(); s->rule = tRule; s->target = tTarget; s->isUpperCase = isUpperCase; result.push_back(s); newComp.push_back(s); }
        }
        return result;
    }
    return {};
}

std::vector<std::shared_ptr<Transformation>> GenerateFallbackTransformations(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<Rule>& applicableRules, char32_t lowerKey, bool isUpperCase) {
    for (const auto& rule : applicableRules) {
        if (rule.effectType == EffectType::Appending) {
            std::vector<std::shared_ptr<Transformation>> result;
            result.push_back(NewAppendingTrans(rule.result, isUpperCase));
            for (const auto& ar : rule.appendedRules) result.push_back(NewAppendingTrans(ar.result, isUpperCase));
            return result;
        }
    }
    return GenerateAppendingTrans(lowerKey, isUpperCase);
}

std::pair<std::vector<std::shared_ptr<Transformation>>, std::vector<std::shared_ptr<Transformation>>> ExtractLastWord(const std::vector<std::shared_ptr<Transformation>>& composition, const std::vector<char32_t>& effectKeys) {
    for (int i = static_cast<int>(composition.size()) - 1; i >= 0; i--) {
        std::shared_ptr<Transformation> trans = composition[i];
        if (trans->rule.effectType == EffectType::Appending) {
            char32_t res = trans->rule.result;
            if (IsSpace(res) || IsPunctuationMark(res)) {
                return {std::vector<std::shared_ptr<Transformation>>(composition.begin(), composition.begin() + i + 1),
                        std::vector<std::shared_ptr<Transformation>>(composition.begin() + i + 1, composition.end())};
            }
        }
    }
    return {{}, composition};
}

std::pair<std::vector<std::shared_ptr<Transformation>>, std::vector<std::shared_ptr<Transformation>>> ExtractLastSyllable(const std::vector<std::shared_ptr<Transformation>>& composition) {
    auto [previous, last] = ExtractLastWord(composition, {});
    size_t anchor = 0;
    for (size_t i = 1; i <= last.size(); i++) {
        std::vector<std::shared_ptr<Transformation>> part(last.begin() + anchor, last.begin() + i);
        if (!IsValid(part, false)) anchor = i - 1;
    }
    std::vector<std::shared_ptr<Transformation>> pref = previous;
    if (anchor > 0) pref.insert(pref.end(), last.begin(), last.begin() + anchor);
    return {pref, std::vector<std::shared_ptr<Transformation>>(last.begin() + anchor, last.end())};
}

std::shared_ptr<Transformation> FindLastAppendingTrans(const std::vector<std::shared_ptr<Transformation>>& composition) {
    for (auto it = composition.rbegin(); it != composition.rend(); ++it) if ((*it)->rule.effectType == EffectType::Appending) return *it;
    return nullptr;
}

std::vector<std::shared_ptr<Transformation>> RefreshLastToneTarget(std::vector<std::shared_ptr<Transformation>>& syllable, bool isStdStyle) {
    Tone currentTone = ToneNone; std::shared_ptr<Transformation> lastToneTrans = nullptr;
    for (auto& t : syllable) if (t->rule.effectType == EffectType::ToneTransformation) { currentTone = t->rule.getTone(); lastToneTrans = t; }
    if (currentTone == ToneNone) return {};
    auto newTarget = FindToneTarget(syllable, isStdStyle);
    if (lastToneTrans && lastToneTrans->target != newTarget) {
        auto trans = std::make_shared<Transformation>(); trans->target = lastToneTrans->target; trans->rule.effectType = EffectType::ToneTransformation; trans->rule.setTone(ToneNone);
        auto trans2 = std::make_shared<Transformation>(); trans2->target = newTarget; trans2->rule.effectType = EffectType::ToneTransformation; trans2->rule.setTone(currentTone);
        return {trans, trans2};
    }
    return {};
}

std::vector<char32_t> BreakComposition(const std::vector<std::shared_ptr<Transformation>>& composition) {
    std::vector<char32_t> result;
    for (const auto& t : composition) if (t->rule.key != 0) result.push_back(t->rule.key);
    return result;
}

static std::vector<std::shared_ptr<Transformation>> processWordToComposition(const std::u32string& word, bool stdStyle) {
    std::vector<std::shared_ptr<Transformation>> composition;
    if (word.length() == 1 && IsWordBreakSymbol(word[0])) {
        auto append = NewAppendingTrans(word[0], std::iswupper(static_cast<wint_t>(word[0])));
        return {append};
    }

    for (char32_t ch : word) {
        char32_t lowerCh = static_cast<char32_t>(std::towlower(static_cast<wint_t>(ch)));
        bool isUpper = std::iswupper(static_cast<wint_t>(ch));
        Tone tone = FindToneFromChar(lowerCh);
        Mark mark = FindMarkFromChar(AddToneToChar(lowerCh, 0));
        char32_t root = AddMarkToTonelessChar(AddToneToChar(lowerCh, 0), 0);
        auto append = NewAppendingTrans(root, isUpper);
        composition.push_back(append);
        if (mark != MarkNone) {
            auto m = std::make_shared<Transformation>();
            m->target = append; m->rule.effectType = EffectType::MarkTransformation;
            m->rule.setMark(mark); m->rule.effectOn = root;
            m->rule.result = AddMarkToTonelessChar(root, static_cast<uint8_t>(mark));
            composition.push_back(m);
        }
    }

    Tone lastTone = ToneNone;
    for (char32_t ch : word) {
        Tone t = FindToneFromChar(static_cast<char32_t>(std::towlower(static_cast<wint_t>(ch))));
        if (t != ToneNone) lastTone = t;
    }

    if (lastTone != ToneNone) {
        auto tTarget = FindToneTarget(composition, stdStyle);
        if (tTarget) {
            auto t = std::make_shared<Transformation>();
            t->target = tTarget; t->rule.effectType = EffectType::ToneTransformation;
            t->rule.setTone(lastTone);
            composition.push_back(t);
        }
    }
    return composition;
}

std::vector<std::shared_ptr<Transformation>> RebuildCompositionFromText(std::u32string text, bool stdStyle) {
    std::vector<std::shared_ptr<Transformation>> composition;
    std::u32string currentWord;
    for (char32_t ch : text) {
        if (IsWordBreakSymbol(ch) || IsSpace(ch)) {
            if (!currentWord.empty()) {
                auto wordComp = processWordToComposition(currentWord, stdStyle);
                composition.insert(composition.end(), wordComp.begin(), wordComp.end());
                currentWord.clear();
            }
            auto symComp = processWordToComposition(std::u32string(1, ch), stdStyle);
            composition.insert(composition.end(), symComp.begin(), symComp.end());
        } else {
            currentWord.push_back(ch);
        }
    }
    if (!currentWord.empty()) {
        auto wordComp = processWordToComposition(currentWord, stdStyle);
        composition.insert(composition.end(), wordComp.begin(), wordComp.end());
    }
    return composition;
}

bool HasValidTone(const std::vector<std::shared_ptr<Transformation>>& composition, Tone tone) {
    if (tone == ToneNone || tone == ToneAcute || tone == ToneDot) return true;
    std::vector<std::shared_ptr<Transformation>> fc, vo, lc;
    ExtractCvcTrans(composition, fc, vo, lc);
    if (lc.empty()) return true;
    std::u32string lastConsonants = Flatten(lc, Mode::EnglishMode | Mode::LowerCase);
    std::vector<std::u32string> stopConsonants = { U"c", U"k", U"p", U"t", U"ch" };
    for (const auto& s : stopConsonants) if (lastConsonants == s) return false;
    return true;
}

bool IsValid(const std::vector<std::shared_ptr<Transformation>>& composition, bool inputIsFullComplete) {
    if (composition.empty()) return true;
    
    // last tone checking
    for (int i = static_cast<int>(composition.size()) - 1; i >= 0; i--) {
        if (composition[i]->rule.effectType == EffectType::ToneTransformation) {
            Tone lastTone = composition[i]->rule.getTone();
            if (!HasValidTone(composition, lastTone)) return false;
            break;
        }
    }

    std::vector<std::shared_ptr<Transformation>> fc, vo, lc;
    ExtractCvcTrans(composition, fc, vo, lc);
    auto flattenMode = Mode::VietnameseMode | Mode::LowerCase | Mode::ToneLess;
    return lotus::IsValidCVC(Flatten(fc, flattenMode), Flatten(vo, flattenMode), Flatten(lc, flattenMode), inputIsFullComplete);
}

} // namespace lotus
