#include "Engine.h"
#include "Character.h"
#include <iostream>
#include <algorithm>

namespace bamboo { namespace engine {

    Engine::Engine(const InputMethod& im) : inputMethod(im) {}

    void Engine::setMode(api::Mode mode) {
        currentMode = mode;
    }

    api::Mode Engine::getMode() const {
        return currentMode;
    }

    void Engine::reset() {
        stateManager.Reset();
    }

    void Engine::processKey(char32_t key) {
        if (currentMode == api::Mode::English) {
            stateManager.AddTransformation({key, 0, EffectType::Appending, 0, key, {}}, nullptr, false);
            return;
        }

        char32_t lowerKey = ToLower(key);
        bool isUpperCase = (key != lowerKey);

        const auto& comp = stateManager.GetComposition();

        // Inner logic to find target for Tone/Mark transformation
        auto findTarget = [&](const Rule& rule) -> Transformation* {
            if (rule.effectType == EffectType::Appending) return nullptr;
            
            // For Tones, target the "best" vowel. 
            if (rule.effectType == EffectType::ToneTransformation) {
                Transformation* bestVowel = nullptr;
                bool bestWasMarked = false;
                for (auto it = comp.rbegin(); it != comp.rend(); ++it) {
                    if (IsWordBreakSymbol((*it)->resultChar)) break; // Stop at word boundary
                    if (IsVowel((*it)->resultChar)) {
                        Transformation* root = it->get();
                        while (root->target != nullptr) root = root->target;
                        
                        bool hasMark = false;
                        for (auto& t_ptr : comp) {
                            if (t_ptr->target == root && t_ptr->rule.effectType == EffectType::MarkTransformation) {
                                hasMark = true;
                                break;
                            }
                        }

                        if (bestVowel == nullptr) {
                            bestVowel = root;
                            bestWasMarked = hasMark;
                        } else {
                            if (!bestWasMarked && hasMark) {
                                bestVowel = root;
                                bestWasMarked = true;
                            }
                        }
                    }
                }
                return bestVowel;
            }

            // For Marks, look backward in the composition for a matching target within the same word
            for (auto it = comp.rbegin(); it != comp.rend(); ++it) {
                if (IsWordBreakSymbol((*it)->resultChar)) break; // Stop at word boundary
                if (rule.effectOn != 0 && ToLower((*it)->resultChar) == ToLower(rule.effectOn)) {
                    Transformation* root = it->get();
                    while (root->target != nullptr) root = root->target;
                    
                    bool alreadyMarked = false;
                    for (auto& t_ptr : comp) {
                        if (t_ptr->target == root && t_ptr->rule.effectType == EffectType::MarkTransformation && t_ptr->rule.effect == rule.effect) {
                            alreadyMarked = true;
                            break;
                        }
                    }
                    if (!alreadyMarked) return root;
                }
            }
            return nullptr;
        };

        bool isSuperKey = std::find(inputMethod.superKeys.begin(), inputMethod.superKeys.end(), lowerKey) != inputMethod.superKeys.end();
        bool appliedAny = false;

        if (isSuperKey) {
            // Collect all possible transformations
            std::vector<std::pair<const Rule*, Transformation*>> matches;
            for (const auto& rule : inputMethod.rules) {
                if (rule.key == lowerKey && rule.effectType != EffectType::Appending) {
                    Transformation* target = findTarget(rule);
                    if (target) {
                        // Check if we already have a transformation for this root
                        bool rootUsed = false;
                        for (const auto& m : matches) {
                            if (m.second == target && m.first->effectType == rule.effectType) {
                                rootUsed = true;
                                break;
                            }
                        }
                        if (!rootUsed) {
                            matches.push_back({&rule, target});
                        }
                    }
                }
            }
            for (const auto& m : matches) {
                stateManager.AddTransformation(*m.first, m.second, isUpperCase);
                appliedAny = true;
            }
        } else {
            // Closest target logic for regular keys
            const Rule* bestRule = nullptr;
            Transformation* bestTarget = nullptr;
            int minDistance = 1000000;

            for (const auto& rule : inputMethod.rules) {
                if (rule.key == lowerKey) {
                    if (rule.effectType == EffectType::Appending) {
                        if (!bestRule) bestRule = &rule;
                    } else {
                        Transformation* target = findTarget(rule);
                        if (target) {
                            // Distance calculation
                            int dist = 1000000;
                            for (int i = (int)comp.size() - 1; i >= 0; --i) {
                                Transformation* curr = comp[i].get();
                                while (curr->target) curr = curr->target;
                                if (curr == target) {
                                    dist = (int)comp.size() - 1 - i;
                                    break;
                                }
                            }
                            if (dist < minDistance) {
                                minDistance = dist;
                                bestRule = &rule;
                                bestTarget = target;
                            }
                        }
                    }
                }
            }
            if (bestRule) {
                stateManager.AddTransformation(*bestRule, bestTarget, isUpperCase);
                appliedAny = true;
            }
        }

        if (!appliedAny) {
            // Special case for Telex 'w' shortcut as standalone 'ư'
            if (inputMethod.name == "Telex" && lowerKey == 'w') {
                Rule r;
                r.key = 'w';
                r.effectType = EffectType::Appending;
                r.result = U'\u01b0'; // ư
                stateManager.AddTransformation(r, nullptr, isUpperCase);
            } else {
                // Fallback: Appending
                stateManager.AddTransformation({lowerKey, 0, EffectType::Appending, 0, lowerKey, {}}, nullptr, isUpperCase);
            }
        }

        refreshLastToneTarget();
    }

    void Engine::refreshLastToneTarget() {
        const auto& comp = stateManager.GetComposition();
        
        // Find if there's a tone in the current syllable
        Transformation* existingToneTrans = nullptr;
        for (auto it = comp.rbegin(); it != comp.rend(); ++it) {
            if (IsWordBreakSymbol((*it)->resultChar)) break; // Stop at word boundary
            if ((*it)->rule.effectType == EffectType::ToneTransformation) {
                existingToneTrans = it->get();
                break;
            }
        }
        
        if (!existingToneTrans || existingToneTrans->rule.effect == 0) return;

        // Re-calculate the best vowel
        Transformation* oldTarget = existingToneTrans->target;
        
        // Use a temporary copy of composition to find target, or just re-run the logic
        // But we must NOT consider the existing tone if it's already affecting the resultChar
        // Actually, our findTarget uses resultChar. If it's already toned, it's fine.
        
        // We need a way to call the findTarget closure or duplicate logic
        // Let's duplicate it for now or move it to a method
        
        Transformation* bestVowel = nullptr;
        bool bestWasMarked = false;
        for (auto it = comp.rbegin(); it != comp.rend(); ++it) {
            if (IsVowel((*it)->resultChar)) {
                Transformation* root = it->get();
                while (root->target != nullptr) root = root->target;
                
                bool hasMark = false;
                for (auto& t_ptr : comp) {
                    if (t_ptr->target == root && t_ptr->rule.effectType == EffectType::MarkTransformation) {
                        hasMark = true;
                        break;
                    }
                }

                if (bestVowel == nullptr) {
                    bestVowel = root;
                    bestWasMarked = hasMark;
                } else if (!bestWasMarked && hasMark) {
                    bestVowel = root;
                    bestWasMarked = true;
                }
            }
        }

        if (bestVowel && bestVowel != oldTarget) {
            // Move tone: add a reset tone to old target, and add the tone to new target
            Rule resetRule;
            resetRule.effectType = EffectType::ToneTransformation;
            resetRule.effect = 0; // ToneNone
            stateManager.AddTransformation(resetRule, oldTarget, false);
            
            Rule newToneRule = existingToneTrans->rule;
            stateManager.AddTransformation(newToneRule, bestVowel, false);
        }
    }

    void Engine::processString(std::string_view str) {
        auto u32vec = Utf8ToUtf32(str);
        for (char32_t cp : u32vec) {
            processKey(cp);
        }
    }

    std::string Engine::getProcessedString() const {
        return const_cast<StateManager&>(stateManager).Flatten(0);
    }

    bool Engine::isValid(bool inputIsFullComplete) const {
        return stateManager.IsValidSyllable(inputIsFullComplete);
    }

    void Engine::removeLastChar(bool refreshLastToneTargetFlag) {
        stateManager.RemoveLastChar();
        if (refreshLastToneTargetFlag) {
            refreshLastToneTarget();
        }
    }

    void Engine::rebuildEngineFromText(std::string_view text) {
        auto u32vec = Utf8ToUtf32(text);
        stateManager.RebuildFromText({u32vec.data(), u32vec.size()}, (flags & 2) != 0);
    }

    void Engine::restoreLastWord(bool toVietnamese) {
        if (!toVietnamese) {
            // "Break" the composition by turning everything into Appending
            const auto& comp = stateManager.GetComposition();
            std::vector<Transformation> rawTrans;
            for (auto& t : comp) {
                if (t->rule.key != 0) {
                    Rule r;
                    r.key = t->rule.key;
                    r.effectType = EffectType::Appending;
                    r.result = t->rule.key;
                    rawTrans.push_back({r, nullptr, t->isUpperCase});
                }
            }
            stateManager.Reset();
            for (auto& t : rawTrans) {
                stateManager.AddTransformation(t.rule, nullptr, t.isUpperCase);
            }
        }
    }

}} // namespace bamboo::engine
