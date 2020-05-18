#pragma once

#include <cstring>

#include <iostream>
#include <string>
#include <utility>

#include <boost/algorithm/hex.hpp>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha3.h>

#include <unistd.h>
#include <termios.h>

namespace lmail
{

constexpr char salt[] = "2cipo6snetwdvhf384qbnxgyar51z7";

inline auto sha256(std::string const &data)
{
    CryptoPP::SHA3_256 sha256_algo;
    std::string        digest;
    digest.resize(sha256_algo.DigestSize());
    sha256_algo.CalculateDigest(reinterpret_cast<CryptoPP::byte *>(digest.data()), reinterpret_cast<CryptoPP::byte const *>(data.data()), data.size());
    return boost::algorithm::hex_lower(digest);
}

inline auto uread(std::string &input, std::string_view prompt = {})
{
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, input));
}

inline auto uread_hidden(std::string &input, std::string_view prompt = {})
{
    termios ts;

    // disabling echoing on STDIN with preserving old flags
    tcgetattr(STDIN_FILENO, &ts);
    auto const old_flags = ts.c_lflag;
    ts.c_lflag &= ~ECHO;
    ts.c_lflag |= ECHONL;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);

    auto const res = uread(input, prompt);

    // restoring flags in STDIN
    tcgetattr(STDIN_FILENO, &ts);
    ts.c_lflag = old_flags;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);

    return res;
}

template <typename... Args>
inline void secure_memset(Args &&... args) noexcept
{
    auto volatile f = std::memset;
    f(std::forward<Args>(args)...);
}

} // namespace lmail
