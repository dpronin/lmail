#pragma once

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <filesystem>
#include <system_error>
#include <algorithm>
#include <string_view>
#include <type_traits>

#include <boost/algorithm/hex.hpp>

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha3.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/queue.h>

#include "application.hpp"
#include "types.hpp"

namespace lmail
{

inline auto sha3_256(std::string const &data)
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

    // restoring flags on STDIN
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

// concept
template <typename KeyT, typename ReturnT = KeyT>
using return_if_rsa = std::enable_if_t<std::is_same_v<KeyT, CryptoPP::RSA::PrivateKey> || std::is_same_v<KeyT, CryptoPP::RSA::PublicKey>, ReturnT>;

template <typename KeyT>
return_if_rsa<KeyT, void> save_key(KeyT const &key, std::filesystem::path const &key_path, int perms)
{
    using namespace CryptoPP;
    auto const fd = creat(key_path.c_str(), perms);
    if (fd < 0)
        throw std::runtime_error("couldn't create a key-related file");
    close(fd);
    ByteQueue bq;
    if constexpr (std::is_same_v<KeyT, RSA::PrivateKey>)
        key.DEREncodePrivateKey(bq);
    else
        key.DEREncodePublicKey(bq);
    FileSink file(key_path.c_str());
    bq.CopyTo(file);
    file.MessageEnd();
}

template <typename KeyT>
return_if_rsa<KeyT> load_key(std::filesystem::path const &key_path)
{
    using namespace CryptoPP;
    KeyT key;
    FileSource file(key_path.c_str(), true);
    ByteQueue bq;
    file.TransferTo(bq);
    bq.MessageEnd();
    if constexpr (std::is_same_v<KeyT, RSA::PrivateKey>)
        key.BERDecodePrivateKey(bq, false, bq.MaxRetrievable());
    else
        key.BERDecodePublicKey(bq, false, bq.MaxRetrievable());
    return key;
}

template <typename UnaryFunction>
void for_each_dir_entry(std::filesystem::path const &dir, UnaryFunction f)
{
    std::error_code ec;
    for (auto const &dir_entry : std::filesystem::directory_iterator(dir, ec))
        f(dir_entry);
}

template <typename Comp, typename UnaryFunction>
void for_each_dir_entry_if(std::filesystem::path const &dir, Comp comp, UnaryFunction f)
{
    std::error_code ec;
    for (auto const &dir_entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (comp(dir_entry))
            f(dir_entry);
    }
}

template <typename Comp>
auto find_dir_entry_if(std::filesystem::path const &dir, Comp comp)
{
    namespace fs = std::filesystem;
    fs::directory_entry dir_entry;
    std::error_code ec;
    auto const rng    = fs::directory_iterator(dir, ec);
    auto dir_entry_it = std::find_if(fs::begin(rng), fs::end(rng), comp);
    if (fs::end(rng) != dir_entry_it)
        dir_entry = *dir_entry_it;
    return dir_entry;
}

template <typename T>
T const& username_to_keyname(T const &username) { return username; }

inline auto find_key(std::filesystem::path const &dir, std::string_view keyname)
{
    return find_dir_entry_if(dir, [keyname](auto const &dir_entry){return dir_entry.path().filename().stem() == keyname; }).path();
}

inline auto find_assoc(std::filesystem::path const &dir, std::string_view keyname) { return find_key(dir, keyname); }

inline void generate_rsa_key_pair(std::filesystem::path const &keys_pair_dir, size_t key_size)
{
    using namespace CryptoPP;
    namespace fs = std::filesystem;

    auto save_priv_key = [](auto const &key, fs::path const &keypath) {
        save_key(key, keypath, S_IRUSR | S_IWUSR);
    };

    auto save_pub_key = [](auto const &key, fs::path const &keypath) {
        save_key(key, keypath, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    };

    if (fs::exists(keys_pair_dir) && !fs::is_directory(keys_pair_dir)
        || !fs::exists(keys_pair_dir) && !fs::create_directories(keys_pair_dir))
    {
        throw std::runtime_error("couldn't create key pair");
    }

    AutoSeededRandomPool rnd;
    RSA::PrivateKey priv_key;
    priv_key.GenerateRandomWithKeySize(rnd, key_size);
    auto key_path = keys_pair_dir / Application::kPrivKeyName;
    save_priv_key(priv_key, key_path);
    key_path += Application::kPubKeySuffix;
    save_pub_key(RSA::PublicKey(priv_key), key_path);
}

inline void encrypt(std::string &msg, CryptoPP::RSA::PublicKey const &key)
{
    using namespace CryptoPP;
    Integer m(reinterpret_cast<byte const*>(msg.data()), msg.size());
    std::ostringstream oss;
    oss << std::hex << key.ApplyFunction(m);
    msg = oss.str();
}

inline void decrypt(std::string &msg, CryptoPP::RSA::PrivateKey const &key)
{
    using namespace CryptoPP;
    AutoSeededRandomPool rng;
    Integer c(msg.data());
    auto r = key.CalculateInverse(rng, c);
    msg.resize(r.MinEncodedSize());
    r.Encode(reinterpret_cast<byte*>(msg.data()), msg.size());
}

} // namespace lmail
