#pragma once

#include <string>

namespace lmail
{

enum class color_e
{
    red   = 1,
    green = 2,
    brown = 3,
    blue  = 4,
};

inline std::string color_escape_reset()
{
    return "\e[0m";
}

inline std::string color_escape(color_e color)
{
    constexpr int base = 30;
    return "\e[" + std::to_string(base + static_cast<int>(color)) + 'm';
}

inline std::string colored(std::string const &input, color_e color)
{
    return color_escape(color) + input + color_escape_reset();
}

} // namespace lmail
