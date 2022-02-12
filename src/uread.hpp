#pragma once

#include <iostream>

#include <string>
#include <string_view>

#include <termios.h>
#include <unistd.h>

namespace lmail
{

inline auto uread(std::string& input, std::string_view prompt = {})
{
    std::cout << prompt;
    std::cin.clear();
    return static_cast<bool>(std::getline(std::cin, input));
}

inline auto uread_hidden(std::string& input, std::string_view prompt = {})
{
    termios ts;

    // disabling echoing on STDIN with preserving old flags
    tcgetattr(STDIN_FILENO, &ts);
    auto const old_flags = ts.c_lflag;
    ts.c_lflag &= ~ECHO;
    ts.c_lflag |= ECHONL;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);

    auto const res = uread(input, prompt);

    // restoring flags on STDIN
    tcgetattr(STDIN_FILENO, &ts);
    ts.c_lflag = old_flags;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);

    return res;
}

} // namespace lmail
