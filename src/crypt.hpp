#pragma once

#include <cstring>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "boost/algorithm/hex.hpp"
#include "boost/range/algorithm/copy.hpp"

#include "cryptopp/aes.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/files.h"
#include "cryptopp/modes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/queue.h"
#include "cryptopp/rsa.h"
#include "cryptopp/sha3.h"

#include "application.hpp"
#include "utility.hpp"

namespace lmail
{

inline auto sha3_256(std::string_view data)
{
    using namespace CryptoPP;
    std::string digest;
    SHA3_256 sha256_algo;
    digest.resize(sha256_algo.DigestSize());
    sha256_algo.CalculateDigest(reinterpret_cast<byte*>(digest.data()), reinterpret_cast<byte const*>(data.data()), data.size());
    return boost::algorithm::hex_lower(digest);
}

inline void secure_memset(auto&&... args) noexcept
{
    auto volatile f = std::memset;
    f(std::forward<decltype(args)>(args)...);
}

// concept
template <typename KeyT, typename ReturnT = KeyT>
using return_if_rsa = std::enable_if_t<std::is_same_v<KeyT, CryptoPP::RSA::PrivateKey> || std::is_same_v<KeyT, CryptoPP::RSA::PublicKey>, ReturnT>;

template <typename KeyT>
return_if_rsa<KeyT, void> save_key(KeyT const& key, std::filesystem::path const& key_path, int perms)
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
return_if_rsa<KeyT> load_key(std::filesystem::path const& key_path)
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

inline rsa_key_pair_t generate_rsa_key_pair(size_t key_size)
{
    using namespace CryptoPP;
    AutoSeededRandomPool rnd;
    RSA::PrivateKey priv_key;
    priv_key.GenerateRandomWithKeySize(rnd, key_size);
    return {priv_key, RSA::PublicKey(priv_key)};
}

inline void store(rsa_key_pair_t const& rsa_key_pair, std::filesystem::path const& keys_pair_dir)
{
    namespace fs = std::filesystem;

    if (!create_dir_if_doesnt_exist(keys_pair_dir))
        throw std::runtime_error("couldn't create key pair");

    auto key_path = keys_pair_dir / Application::kPrivKeyName;
    save_key(rsa_key_pair.first, key_path, S_IRUSR | S_IWUSR);
    key_path += Application::kPubKeySuffix;
    save_key(rsa_key_pair.second, key_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

inline void encrypt(std::string& msg, CryptoPP::RSA::PublicKey const& rsa_key)
{
    using namespace CryptoPP;

    AutoSeededRandomPool rnd;

    auto gen_byte_block = [rnd = AutoSeededRandomPool()](uint16_t size) mutable {
        SecByteBlock sec_byte_block(0, size);
        rnd.GenerateBlock(sec_byte_block, sec_byte_block.size());
        return sec_byte_block;
    };

    auto const aes_key     = gen_byte_block(AES::DEFAULT_KEYLENGTH);
    auto const init_vector = gen_byte_block(AES::BLOCKSIZE);

    std::string cmsg;
    cmsg.resize(msg.size());
    CFB_Mode<AES>::Encryption cfbEncryption(aes_key, aes_key.size(), init_vector);
    // clang-format off
    cfbEncryption.ProcessData(reinterpret_cast<byte *>(cmsg.data()),
                              reinterpret_cast<byte const *>(msg.data()),
                              msg.size());
    // clang-format on

    auto cypher_byte_block = [&rsa_key](SecByteBlock const& sec_byte_block) {
        return rsa_key.ApplyFunction(Integer(reinterpret_cast<byte const*>(sec_byte_block.data()), sec_byte_block.size()));
    };

    auto const caesk = cypher_byte_block(aes_key);
    auto const civ   = cypher_byte_block(init_vector);

    std::string out;
    out.resize(2 * sizeof(uint16_t) + caesk.MinEncodedSize() + civ.MinEncodedSize() + cmsg.size());

    auto* pout = out.data();
    auto* pend = pout + out.size();

    auto push_size = [&](uint16_t size) {
        if (pend - pout < sizeof(size))
            throw std::invalid_argument("insufficient place, cannot encrypt");
        std::memcpy(pout, reinterpret_cast<byte const*>(&size), sizeof(size));
        pout += sizeof(size);
    };

    auto push_integer = [&](Integer const& integer) {
        push_size(integer.MinEncodedSize());
        if (pend - pout < integer.MinEncodedSize())
            throw std::invalid_argument("insufficient place, cannot encrypt");
        integer.Encode(reinterpret_cast<byte*>(pout), integer.MinEncodedSize());
        pout += integer.MinEncodedSize();
    };

    // encoding cyphered AES key and its size coming first
    push_integer(caesk);
    // encoding cyphered initial vector and its size coming first
    push_integer(civ);
    // copying the message cyphered with AES key and initial vector
    boost::copy(cmsg, pout);
    msg = std::move(out);
}

inline void decrypt(std::string& cmsg, CryptoPP::RSA::PrivateKey const& rsa_key)
{
    using namespace CryptoPP;

    auto const* pin  = cmsg.data();
    auto const* pend = pin + cmsg.size();

    auto pop_size = [&] {
        uint16_t size;
        if (pend - pin < sizeof(size))
            throw std::invalid_argument("malformed message, cannot decrypt");
        std::memcpy(reinterpret_cast<byte*>(&size), pin, sizeof(size));
        pin += sizeof(size);
        return size;
    };

    auto pop_integer = [&] {
        auto const size = pop_size();
        if (pend - pin < size)
            throw std::invalid_argument("malformed message, cannot decrypt");
        Integer const integer(reinterpret_cast<byte const*>(pin), size);
        pin += size;
        return integer;
    };

    auto inv_byte_block = [&rsa_key, rnd = AutoSeededRandomPool()](Integer const& integer) mutable {
        auto const inv_integer = rsa_key.CalculateInverse(rnd, integer);
        SecByteBlock sec_byte_block(inv_integer.MinEncodedSize());
        inv_integer.Encode(reinterpret_cast<byte*>(sec_byte_block.data()), sec_byte_block.size());
        return sec_byte_block;
    };

    // decoding integer representing cyphered AES key
    auto const caesk = pop_integer();
    // decoding integer representing cyphered initial vector
    auto const civ = pop_integer();
    // empty message received
    auto const msg_size = pend - pin;
    if (0 == msg_size) {
        cmsg.clear();
        return;
    }

    // decoding AES key using RSA private key
    auto const aes_key = inv_byte_block(caesk);
    // decoding initial vector using RSA private key
    auto const init_vector = inv_byte_block(civ);

    std::string msg;
    msg.resize(msg_size);
    CFB_Mode<AES>::Decryption cfbDecryption(aes_key, aes_key.size(), init_vector);
    // clang-format off
    // decrypting message using AES key and initial vector decoded from the original message
    cfbDecryption.ProcessData(reinterpret_cast<byte *>(msg.data()),
                              reinterpret_cast<byte const *>(pin),
                              msg_size);
    // clang-format on
    cmsg = std::move(msg);
}

} // namespace lmail
