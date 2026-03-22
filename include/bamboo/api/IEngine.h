#pragma once
#include <string>
#include <string_view>
#include <memory>

namespace bamboo { namespace api {
    enum class Mode { Vietnamese, English };
    class IEngine {
    public:
        virtual ~IEngine() = default;
        virtual void setMode(Mode mode) = 0;
        virtual Mode getMode() const = 0;
        virtual void reset() = 0;
        virtual void processKey(char32_t key) = 0;
        virtual void processString(std::string_view str) = 0;
        virtual std::string getProcessedString() const = 0;
        virtual bool isValid(bool inputIsFullComplete) const = 0;
        virtual void removeLastChar(bool refreshLastToneTarget) = 0;
        virtual void restoreLastWord(bool toVietnamese) = 0;
        virtual void rebuildEngineFromText(std::string_view text) = 0;
    };
    std::unique_ptr<IEngine> createEngine(std::string_view dataDirPath, std::string_view inputMethod);
}}
