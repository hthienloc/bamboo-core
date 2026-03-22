#include "StateManager.h"
#include "Character.h"
#include "Spelling.h"
#include <algorithm>

namespace bamboo { namespace engine {

    StateManager::StateManager() {}

    void StateManager::AddTransformation(const Rule& rule, Transformation* target, bool isUpperCase) {
        auto t = std::make_unique<Transformation>();
        t->rule = rule;
        t->target = target;
        t->isUpperCase = isUpperCase;
        
        // Calculate resultChar based on rule effect
        if (rule.effectType == EffectType::Appending) {
            t->resultChar = rule.result;
        } else if (target) {
            if (rule.effectType == EffectType::MarkTransformation) {
                t->resultChar = AddMarkToChar(target->resultChar, rule.effect);
            } else if (rule.effectType == EffectType::ToneTransformation) {
                t->resultChar = AddToneToChar(target->resultChar, rule.effect);
            }
        }
        composition.push_back(std::move(t));
    }

    void StateManager::RemoveLastChar() {
        if (composition.empty()) return;

        // Find the last Appending transformation
        Transformation* lastAppending = nullptr;
        for (auto it = composition.rbegin(); it != composition.rend(); ++it) {
            if ((*it)->rule.effectType == EffectType::Appending) {
                lastAppending = it->get();
                break;
            }
        }

        if (!lastAppending) {
            composition.pop_back();
            return;
        }

        // Remove it and all transformations targeting it (Marks/Tones)
        auto it = std::remove_if(composition.begin(), composition.end(), [&](const std::unique_ptr<Transformation>& t) {
            return t.get() == lastAppending || t->target == lastAppending;
        });
        composition.erase(it, composition.end());
    }

    void StateManager::Reset() {
        composition.clear();
    }

    void StateManager::RebuildFromText(std::u32string_view text, bool stdStyle) {
        (void)stdStyle; // Currently unused, but kept for future Go-parity logic
        Reset();
        // Simplified rebuild logic:
        // For each character in text, decompose into root + mark + tone
        for (char32_t cp : text) {
            char32_t lowerCp = ToLower(cp);
            bool isUpper = (cp != lowerCp);
            
            uint8_t tone = static_cast<uint8_t>(FindToneFromChar(lowerCp));
            auto markFound = FindMarkFromChar(lowerCp);
            uint8_t mark = static_cast<uint8_t>(markFound.first);
            bool hasMark = markFound.second;
            
            char32_t root = lowerCp;
            if (tone != 0) root = AddToneToChar(root, 0);
            if (hasMark && mark != 0) root = AddMarkToChar(root, 0);
            
            // Add root (Appending)
            Rule appRule;
            appRule.key = root;
            appRule.effectType = EffectType::Appending;
            appRule.result = root;
            AddTransformation(appRule, nullptr, isUpper);
            Transformation* rootTrans = composition.back().get();
            
            // Add mark
            if (hasMark && mark != 0) {
                Rule markRule;
                markRule.effectType = EffectType::MarkTransformation;
                markRule.effect = mark;
                AddTransformation(markRule, rootTrans, isUpper);
            }
            
            // Tones are added at the end of word in Go version, 
            // but for simplicity here we add to the last vowel.
            if (tone != 0) {
                Rule toneRule;
                toneRule.effectType = EffectType::ToneTransformation;
                toneRule.effect = tone;
                
                // Find best vowel for tone (using stdStyle)
                // For now, just target the root we just added if it's a vowel
                if (IsVowel(rootTrans->resultChar)) {
                    AddTransformation(toneRule, rootTrans, isUpper);
                }
            }
        }
    }

    std::string StateManager::Flatten(uint32_t mode) {
        (void)mode;
        std::vector<Transformation*> appendingList;
        std::unordered_map<Transformation*, std::vector<Transformation*>> appendingMap;

        for (auto& t : composition) {
            if (t->rule.effectType == EffectType::Appending) {
                appendingList.push_back(t.get());
            } else if (t->target != nullptr) {
                appendingMap[t->target].push_back(t.get());
            }
        }

        std::u32string u32result;
        for (auto* appendingTrans : appendingList) {
            char32_t chr = appendingTrans->rule.result;
            auto it = appendingMap.find(appendingTrans);
            if (it != appendingMap.end()) {
                for (auto* trans : it->second) {
                    if (trans->rule.effectType == EffectType::MarkTransformation) {
                        chr = AddMarkToChar(chr, trans->rule.effect);
                    } else if (trans->rule.effectType == EffectType::ToneTransformation) {
                        chr = AddToneToChar(chr, trans->rule.effect);
                    }
                }
            }
            
            if (appendingTrans->isUpperCase) {
                chr = ToUpper(chr);
            }
            u32result.push_back(chr);
        }
        
        return Utf32ToUtf8(u32result);
    }

    std::pair<size_t, size_t> StateManager::GetLastWordRange() const {
        return {0, composition.size()};
    }

    bool StateManager::IsValidSyllable(bool fullComplete) const {
        std::u32string u32flat;
        std::string flat = const_cast<StateManager*>(this)->Flatten(0);
        auto u32vec = Utf8ToUtf32(flat);
        for (auto cp : u32vec) u32flat.push_back(ToLower(cp));

        std::u32string fc, vo, lc;
        size_t i = 0;
        while (i < u32flat.length() && !IsVowel(u32flat[i])) {
            fc.push_back(u32flat[i]);
            i++;
        }
        while (i < u32flat.length() && IsVowel(u32flat[i])) {
            vo.push_back(AddToneToChar(u32flat[i], 0));
            i++;
        }
        while (i < u32flat.length()) {
            lc.push_back(u32flat[i]);
            i++;
        }

        return IsValidCVC(fc, vo, lc, fullComplete);
    }

}} // namespace bamboo::engine
