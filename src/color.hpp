#pragma once

#include <string>
#include <string_view>

namespace lmail
{

enum class color_e
{
    red    = 1,
    green  = 2,
    brown  = 3,
    blue   = 4,
    purple = 5,
    lblue  = 6,
};

inline std::string_view color_escape_reset() noexcept { return "\e[0m"; }

inline std::string color_escape(color_e color)
{
    constexpr int base = 30;
    return "\e[" + std::to_string(base + static_cast<int>(color)) + 'm';
}

inline std::string colored(std::string_view input, color_e color)
{
    return color_escape(color) + input.data() + color_escape_reset().data();
}

inline auto cred(std::string_view input) { return colored(input, color_e::red); }
inline auto cgreen(std::string_view input) { return colored(input, color_e::green); }
inline auto cbrown(std::string_view input) { return colored(input, color_e::brown); }
inline auto cblue(std::string_view input) { return colored(input, color_e::blue); }
inline auto cpurple(std::string_view input) { return colored(input, color_e::purple); }
inline auto clblue(std::string_view input) { return colored(input, color_e::lblue); }

} // namespace lmail
