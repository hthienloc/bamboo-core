#pragma once

#include "lotus/input_method.hpp"
#include <string>
#include <vector>
#include <memory>

namespace lotus {

/**
 * Interface for the Bamboo/Lotus engine.
 */
class IEngine {
public:
    virtual ~IEngine() = default;
    virtual void SetFlag(uint32_t flag) = 0;
    virtual uint32_t GetFlag() const = 0;
    virtual InputMethod GetInputMethod() const = 0;
    virtual void ProcessKey(char32_t key, Mode mode) = 0;
    virtual void ProcessString(const std::u16string& str, Mode mode) = 0;
    virtual std::u16string GetProcessedString(Mode mode) = 0;
    virtual bool IsValid(bool inputIsFullComplete) = 0;
    virtual bool CanProcessKey(char32_t key) const = 0;
    virtual void RemoveLastChar(bool refreshLastToneTarget) = 0;
    virtual void RestoreLastWord(bool toVietnamese) = 0;
    virtual void RebuildEngineFromText(const std::u16string& text) = 0;
    virtual void Reset() = 0;
};

/**
 * Main implementation of the Vietnamese input engine.
 */
class LotusEngine : public IEngine {
public:
    LotusEngine(const InputMethod& inputMethod, uint32_t flags);
    ~LotusEngine() override;

    void SetFlag(uint32_t flag) override;
    uint32_t GetFlag() const override;
    InputMethod GetInputMethod() const override;
    void ProcessKey(char32_t key, Mode mode) override;
    void ProcessString(const std::u16string& str, Mode mode) override;
    std::u16string GetProcessedString(Mode mode) override;
    bool IsValid(bool inputIsFullComplete) override;
    bool CanProcessKey(char32_t key) const override;
    void RemoveLastChar(bool refreshLastToneTarget) override;
    void RestoreLastWord(bool toVietnamese) override;
    void RebuildEngineFromText(const std::u16string& text) override;
    void Reset() override;

private:
    std::vector<std::shared_ptr<Transformation>> composition;
    InputMethod inputMethod;
    uint32_t flags;

    std::vector<Rule> getApplicableRules(char32_t key) const;
    std::pair<std::shared_ptr<Transformation>, Rule> findTargetByKey(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t key);
    std::vector<std::shared_ptr<Transformation>> generateTransformations(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t lowerKey, bool isUpperCase);
    std::vector<std::shared_ptr<Transformation>> updateComposition(const std::vector<std::shared_ptr<Transformation>>& comp, char32_t lowerKey, bool isUpperCase);
    void updateComposition(char32_t lowerKey, bool isUpperCase);
    std::shared_ptr<Transformation> applyUowShortcut(const std::vector<std::shared_ptr<Transformation>>& syllable);
    std::vector<std::shared_ptr<Transformation>> refreshLastToneTargetLogic(const std::vector<std::shared_ptr<Transformation>>& syllable);
};

std::unique_ptr<IEngine> CreateEngine(const InputMethod& inputMethod, uint32_t flags);

} // namespace lotus
