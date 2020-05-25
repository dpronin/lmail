#pragma once

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <filesystem>

#include <boost/algorithm/hex.hpp>

#include <termios.h>
#include <unistd.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha3.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/queue.h>

namespace lmail
{

constexpr char salt[] = "2cipo6snetwdvhf384qbnxgyar51z7";

inline auto sha256(std::string const &data)
{
    using namespace CryptoPP;
    SHA3_256 sha256_algo;
    std::string        digest;
    digest.resize(sha256_algo.DigestSize());
    sha256_algo.CalculateDigest(reinterpret_cast<byte*>(digest.data()), reinterpret_cast<CryptoPP::byte const *>(data.data()), data.size());
    return boost::algorithm::hex_lower(digest);
}

inline auto uread(std::string &input, std::string_view prompt = {})
{
    std::cout << prompt;
    std::cin.clear();
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
void secure_memset(Args &&... args) noexcept
{
    auto volatile f = std::memset;
    f(std::forward<Args>(args)...);
}

inline void create_rsa_key(std::filesystem::path const &keys_dir, size_t keysize)
{
    constexpr char kKeyName[]      = "key";
    constexpr char kPubKeySuffix[] = ".pub";

    using namespace CryptoPP;
    auto save_key = [](auto const &key, std::filesystem::path const &keypath) {
        ByteQueue q;
        key.Save(q);
        FileSink file(keypath.c_str());
        q.CopyTo(file);
        file.MessageEnd();
    };

    if (!std::filesystem::exists(keys_dir) && !std::filesystem::create_directories(keys_dir)
        || !std::filesystem::is_directory(keys_dir))
        throw std::runtime_error("couldn't create '" + std::string(keys_dir) + "' directory\n");

    AutoSeededRandomPool rng;
    RSA::PrivateKey private_key;
    private_key.GenerateRandomWithKeySize(rng, keysize);
    auto key_path = keys_dir / kKeyName;
    save_key(private_key, key_path);
    key_path += kPubKeySuffix;
    save_key(RSA::PublicKey(private_key), key_path);
}

} // namespace lmail
