#pragma once

#include <string>
#include <vector>
#include <memory>
#include "lotus/input_method.hpp"

namespace lotus {

/**
 * Converts a list of transformations into a final string based on the given mode.
 */
std::u32string Flatten(const std::vector<std::shared_ptr<Transformation>>& composition, Mode mode);

} // namespace lotus
