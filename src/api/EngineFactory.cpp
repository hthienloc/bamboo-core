#include <bamboo/api/IEngine.h>
#include "../engine/Engine.h"
#include "../engine/Rules.h"

namespace bamboo { namespace api {

    std::unique_ptr<IEngine> createEngine(std::string_view dataDirPath, std::string_view inputMethod) {
        (void)dataDirPath;
        auto im = engine::GetInputMethod(inputMethod);
        return std::make_unique<engine::Engine>(im);
    }

}} // namespace bamboo::api
