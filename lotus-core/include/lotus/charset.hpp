#pragma once

#include <string>
#include <unordered_map>

namespace lotus {

using CharsetDefinition = std::unordered_map<char32_t, std::u32string>;

/**
 * Access to various Vietnamese character set definitions (TCVN3, VNI, VISCII, etc.)
 */
extern const std::unordered_map<std::u16string, CharsetDefinition> CHARSET_DEFINITIONS;

} // namespace lotus
