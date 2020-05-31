#pragma once

#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

#include <boost/algorithm/hex.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha3.h>

#include "application.hpp"
#include "types.hpp"

namespace lmail
{

inline auto sha3_256(std::string const &data)
{
    using namespace CryptoPP;
    SHA3_256    sha256_algo;
    std::string digest;
    digest.resize(sha256_algo.DigestSize());
    sha256_algo.CalculateDigest(reinterpret_cast<byte *>(digest.data()), reinterpret_cast<CryptoPP::byte const *>(data.data()), data.size());
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
    KeyT       key;
    FileSource file(key_path.c_str(), true);
    ByteQueue  bq;
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
    std::error_code     ec;
    auto const          rng          = fs::directory_iterator(dir, ec);
    auto                dir_entry_it = std::find_if(fs::begin(rng), fs::end(rng), comp);
    if (fs::end(rng) != dir_entry_it)
        dir_entry = *dir_entry_it;
    return dir_entry;
}

template <typename T>
T const &username_to_keyname(T const &username) { return username; }

inline auto find_key(std::filesystem::path const &dir, std::string_view keyname)
{
    return find_dir_entry_if(dir, [keyname](auto const &dir_entry) { return dir_entry.path().filename().stem() == keyname; }).path();
}

inline rsa_key_pair_t generate_rsa_key_pair(size_t key_size)
{
    using namespace CryptoPP;
    AutoSeededRandomPool rnd;
    RSA::PrivateKey      priv_key;
    priv_key.GenerateRandomWithKeySize(rnd, key_size);
    return { priv_key, RSA::PublicKey(priv_key) };
}

inline void store(rsa_key_pair_t const &rsa_key_pair, std::filesystem::path const &keys_pair_dir)
{
    namespace fs = std::filesystem;

    if (fs::exists(keys_pair_dir) && !fs::is_directory(keys_pair_dir) || !fs::exists(keys_pair_dir) && !fs::create_directories(keys_pair_dir))
        throw std::runtime_error("couldn't create key pair");

    auto key_path = keys_pair_dir / Application::kPrivKeyName;
    save_key(rsa_key_pair.first, key_path, S_IRUSR | S_IWUSR);
    key_path += Application::kPubKeySuffix;
    save_key(rsa_key_pair.second, key_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

inline void encrypt(std::string &msg, CryptoPP::RSA::PublicKey const &rsa_key)
{
    using namespace CryptoPP;

    AutoSeededRandomPool rnd;

    SecByteBlock aes_key(0, AES::DEFAULT_KEYLENGTH);
    rnd.GenerateBlock(aes_key, aes_key.size());
    Integer aesk(reinterpret_cast<byte const *>(aes_key.data()), aes_key.size());

    SecByteBlock init_vector(0, AES::BLOCKSIZE);
    rnd.GenerateBlock(init_vector, init_vector.size());
    Integer iv(reinterpret_cast<byte const *>(init_vector.data()), init_vector.size());

    std::string cmsg;
    cmsg.resize(msg.size());
    CFB_Mode<AES>::Encryption cfbEncryption(aes_key, aes_key.size(), init_vector);
    cfbEncryption.ProcessData(reinterpret_cast<byte*>(cmsg.data()),
                              reinterpret_cast<byte const*>(msg.data()),
                              msg.size());

    auto const caesk = rsa_key.ApplyFunction(aesk);
    auto const civ   = rsa_key.ApplyFunction(iv);

    auto const caesk_size = static_cast<uint16_t>(caesk.MinEncodedSize());
    auto const civ_size   = static_cast<uint16_t>(civ.MinEncodedSize());
    std::string out;
    out.resize(2 * sizeof(uint16_t) + caesk_size + civ_size + cmsg.size());
    auto *pout = out.data();

    // encoding cyphered AES key and its size coming first
    std::memcpy(pout, reinterpret_cast<byte const*>(&caesk_size), sizeof(uint16_t));
    pout += sizeof(uint16_t);
    caesk.Encode(reinterpret_cast<byte*>(pout), caesk_size);
    pout += caesk_size;

    // encoding cyphered initial vector and its size coming first
    std::memcpy(pout, reinterpret_cast<byte const*>(&civ_size), sizeof(uint16_t));
    pout += sizeof(uint16_t);
    civ.Encode(reinterpret_cast<byte*>(pout), civ_size);
    pout += civ_size;

    // copying the message cyphered with AES key and initial vector
    boost::copy(cmsg, pout);
    msg = std::move(out);
}

inline void decrypt(std::string &cmsg, CryptoPP::RSA::PrivateKey const &rsa_key)
{
    using namespace CryptoPP;

    if (cmsg.size() < 2 * sizeof(uint16_t))
        throw std::invalid_argument("malformed message, cannot decrypt");

    auto const *pin  = cmsg.data();
    auto const *pend = cmsg.data() + cmsg.size();

    uint16_t caesk_size;
    std::memcpy(reinterpret_cast<byte*>(&caesk_size), pin, sizeof(uint16_t));
    pin += sizeof(uint16_t);

    if (pend - pin < caesk_size)
        throw std::invalid_argument("malformed message, cannot decrypt");

    Integer const caesk(reinterpret_cast<byte const*>(pin), caesk_size);
    pin += caesk_size;

    if (pend - pin < sizeof(uint16_t))
        throw std::invalid_argument("malformed message, cannot decrypt");

    uint16_t civ_size;
    std::memcpy(reinterpret_cast<byte*>(&civ_size), pin, sizeof(uint16_t));
    pin += sizeof(uint16_t);

    if (pend - pin < civ_size)
        throw std::invalid_argument("malformed message, cannot decrypt");

    Integer const civ(reinterpret_cast<byte const*>(pin), civ_size);
    pin += civ_size;

    // empty message received
    if (0 == pend - pin)
    {
        cmsg.clear();
        return;
    }

    AutoSeededRandomPool rnd;

    auto const aesk = rsa_key.CalculateInverse(rnd, caesk);
    SecByteBlock aes_key(aesk.MinEncodedSize());
    aesk.Encode(reinterpret_cast<byte *>(aes_key.data()), aes_key.size());

    auto const iv = rsa_key.CalculateInverse(rnd, civ);
    SecByteBlock init_vector(iv.MinEncodedSize());
    iv.Encode(reinterpret_cast<byte *>(init_vector.data()), init_vector.size());

    std::string msg;
    msg.resize(pend - pin);
    CFB_Mode<AES>::Decryption cfbDecryption(aes_key, aes_key.size(), init_vector);
    cfbDecryption.ProcessData(reinterpret_cast<byte *>(msg.data()),
                              reinterpret_cast<byte const *>(pin),
                              pend - pin);
    cmsg = std::move(msg);
}

} // namespace lmail
