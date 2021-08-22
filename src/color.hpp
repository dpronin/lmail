#pragma once

#include <string>
#include <string_view>

namespace lmail
{

enum class color_e
{
    kRed    = 1,
    kGreen  = 2,
    kBrown  = 3,
    kBlue   = 4,
    kPurple = 5,
    kLblue  = 6,
};

inline std::string_view color_escape_reset() noexcept { return "\e[0m"; }

inline std::string color_escape(color_e color)
{
    constexpr int kBase = 30;
    return "\e[" + std::to_string(kBase + static_cast<int>(color)) + 'm';
}

inline std::string colored(std::string_view input, color_e color)
{
    return color_escape(color) + input.data() + color_escape_reset().data();
}

inline auto cred(std::string_view input) { return colored(input, color_e::kRed); }
inline auto cgreen(std::string_view input) { return colored(input, color_e::kGreen); }
inline auto cbrown(std::string_view input) { return colored(input, color_e::kBrown); }
inline auto cblue(std::string_view input) { return colored(input, color_e::kBlue); }
inline auto cpurple(std::string_view input) { return colored(input, color_e::kPurple); }
inline auto clblue(std::string_view input) { return colored(input, color_e::kLblue); }

} // namespace lmail
