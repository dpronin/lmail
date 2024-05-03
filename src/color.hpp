#pragma once

#include <concepts>
#include <ostream>
#include <string_view>

#include <termcolor/termcolor.hpp>

namespace std
{

template <typename CharT, typename Traits = std::char_traits<CharT>>
constexpr inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, std::invocable<decltype(os)> auto const& f)
{
    return f(os);
}

} // namespace std

namespace lmail
{

constexpr auto cred(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::red << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto cgreen(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::green << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto cyellow(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::yellow << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto cblue(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::blue << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto cpurple(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::magenta << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto clblue(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::bright_blue << input << termcolor::reset << termcolor::nocolorize;
    };
}

constexpr auto cgrey(std::string_view input)
{
    return [=]<typename CharT, typename Traits = std::char_traits<CharT>>(std::basic_ostream<CharT, Traits>& os) -> std::basic_ostream<CharT, Traits>& {
        return os << termcolor::colorize << termcolor::grey << input << termcolor::reset << termcolor::nocolorize;
    };
}

} // namespace lmail
