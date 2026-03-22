#pragma once
#include <bamboo/api/IEngine.h>
#include "StateManager.h"
#include "Rules.h"

namespace bamboo { namespace engine {

    class Engine : public api::IEngine {
    public:
        Engine(const InputMethod& im);
        virtual ~Engine() = default;

        void setMode(api::Mode mode) override;
        api::Mode getMode() const override;
        void reset() override;
        void processKey(char32_t key) override;
        void processString(std::string_view str) override;
        std::string getProcessedString() const override;
        bool isValid(bool inputIsFullComplete) const override;
        void removeLastChar(bool refreshLastToneTarget) override;
        void restoreLastWord(bool toVietnamese) override;
        void rebuildEngineFromText(std::string_view text) override;

    private:
        void refreshLastToneTarget();
        InputMethod inputMethod;
        StateManager stateManager;
        api::Mode currentMode = api::Mode::Vietnamese;
        uint32_t flags = 7; // EfreeToneMarking | EstdToneStyle | EautoCorrectEnabled
    };

}} // namespace bamboo::engine
