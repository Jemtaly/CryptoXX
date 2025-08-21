#include <cstdio>
#include <stdexcept>
#include <string>

// include all hash algorithms we need
#include "CryptoXX/hash/blake2b.hpp"
#include "CryptoXX/hash/blake2s.hpp"
#include "CryptoXX/hash/blake3.hpp"
#include "CryptoXX/hash/crc.hpp"
#include "CryptoXX/hash/keccak.hpp"
#include "CryptoXX/hash/md5.hpp"
#include "CryptoXX/hash/sha.hpp"
#include "CryptoXX/hash/sha256.hpp"
#include "CryptoXX/hash/sha512.hpp"
#include "CryptoXX/hash/sm3.hpp"
#include "CryptoXX/hash/whirlpool.hpp"

// include hash.hpp and hmac.hpp, which contains the wrapper classes for hash and hmac
#include "CryptoXX/hash.hpp"
#include "CryptoXX/hmac.hpp"

#define BUFSIZE 8192  // Same with OpenSSL default buffer size

void read_bin(std::string const &name, size_t len, char const *hex, uint8_t *bin) {
    if (hex == nullptr) {
        throw std::runtime_error("No " + name + " specified.");
    }
    for (size_t i = 0; i < len * 2; ++i) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            bin[i / 2] = bin[i / 2] & (i % 2 ? 0xf0 : 0x0f) | (hex[i] + 0 & 0xf) << (i % 2 ? 0 : 4);
        } else if (hex[i] >= 'a' && hex[i] <= 'f' || hex[i] >= 'A' && hex[i] <= 'F') {
            bin[i / 2] = bin[i / 2] & (i % 2 ? 0xf0 : 0x0f) | (hex[i] + 9 & 0xf) << (i % 2 ? 0 : 4);
        } else {
            throw std::runtime_error("Invalid character in " + name + ", should be a " + std::to_string(len) + "-byte hex string.");
        }
    }
    if (hex[len * 2] != '\0') {
        throw std::runtime_error("Invalid length of " + name + ", should be a " + std::to_string(len) + "-byte hex string.");
    }
}

void print_hex(uint8_t const *bin, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", bin[i]);
    }
    printf("\n");
}

constexpr uint32_t hash(char const *str) {
    return *str ? *str + hash(str + 1) * 16777619UL : 2166136261UL;
}

class ArgsHandler {
public:
    char **argv;
    int argc;
    int argi = 1;

    char const *next_arg() {
        if (argi == argc) {
            return nullptr;
        }
        return argv[argi++];
    }

    // calculate checksum from stdin, output to stdout in hex
    template<typename HashWrapper, typename... Args>
    void hash_mode_perform(Args &&...args) {
        HashWrapper hw(std::forward<Args>(args)...);
        uint8_t buf[BUFSIZE];
        size_t read;
        while ((read = fread(buf, 1, BUFSIZE, stdin)) == BUFSIZE) {
            hw.update(buf, (uint8_t *)buf + BUFSIZE);
        }
        hw.update(buf, (uint8_t *)buf + read);
        uint8_t digest[HashWrapper::DIGEST_SIZE];
        hw.digest(digest);
        print_hex(digest, HashWrapper::DIGEST_SIZE);
    }

    template<typename Hash>
    void hash_or_hmac_select_mode() {
        auto len_arg = next_arg();
        if (len_arg == nullptr) {
            return hash_mode_perform<HashWrapper<Hash>>();
        } else {
            int len;
            try {
                len = std::stoi(len_arg);
            } catch (std::exception const &e) {
                throw std::runtime_error("Key length should be an integer.");
            }
            if (len < 0) {
                throw std::runtime_error("Invalid key length, required: 0 <= len.");
            }
            auto key_arg = next_arg();
            uint8_t key[len];
            if (len != 0 || key_arg) {
                read_bin("key", len, key_arg, key);
            }
            if (next_arg()) {
                throw std::runtime_error("Too many arguments.");
            }
            return hash_mode_perform<HMACWrapper<Hash>>((uint8_t *)key, len);
        }
    }

    template<typename Hash>
    void hash_or_mac_select_mode() {
        auto len_arg = next_arg();
        if (len_arg == nullptr) {
            return hash_mode_perform<HashWrapper<Hash>>();
        } else {
            int len;
            try {
                len = std::stoi(len_arg);
            } catch (std::exception const &e) {
                throw std::runtime_error("Key length should be an integer.");
            }
            if (len < 0 || len > Hash::BLOCK_SIZE) {
                throw std::runtime_error("Invalid key length, required: 0 <= len <= " + std::to_string(Hash::BLOCK_SIZE) + ".");
            }
            auto key_arg = next_arg();
            uint8_t key[len];
            if (len != 0 || key_arg) {
                read_bin("key", len, key_arg, key);
            }
            if (next_arg()) {
                throw std::runtime_error("Too many arguments.");
            }
            return hash_mode_perform<HashWrapper<Hash>>((uint8_t *)key, len);
        }
    }

    template<typename Hash>
    void hash_select_mode() {
        if (next_arg()) {
            throw std::runtime_error("Too many arguments, the algorithm does not require a key.");
        }
        return hash_mode_perform<HashWrapper<Hash>>();
    }

    void select_algorithm() {
        auto algorithm = next_arg();
        if (algorithm == nullptr) {
            throw std::runtime_error("No algorithm specified.");
        }
        switch (hash(algorithm)) {
        case hash("MD5"):
            return hash_or_hmac_select_mode<MD5>();
        case hash("SHA0"):
            return hash_or_hmac_select_mode<SHA0>();
        case hash("SHA1"):
            return hash_or_hmac_select_mode<SHA1>();
        case hash("SHA224"):
            return hash_or_hmac_select_mode<SHA224>();
        case hash("SHA256"):
            return hash_or_hmac_select_mode<SHA256>();
        case hash("SHA384"):
            return hash_or_hmac_select_mode<SHA384>();
        case hash("SHA512"):
            return hash_or_hmac_select_mode<SHA512>();
        case hash("SHA3-224"):
            return hash_or_hmac_select_mode<SHA3<224>>();
        case hash("SHA3-256"):
            return hash_or_hmac_select_mode<SHA3<256>>();
        case hash("SHA3-384"):
            return hash_or_hmac_select_mode<SHA3<384>>();
        case hash("SHA3-512"):
            return hash_or_hmac_select_mode<SHA3<512>>();
        case hash("SHAKE128"):
            return hash_or_hmac_select_mode<SHAKE<128, 256>>();
        case hash("SHAKE256"):
            return hash_or_hmac_select_mode<SHAKE<256, 512>>();
        case hash("SM3"):
            return hash_or_hmac_select_mode<SM3>();
        case hash("Whirlpool"):
            return hash_or_hmac_select_mode<Whirlpool>();
        case hash("BLAKE2b384"):
            return hash_or_mac_select_mode<BLAKE2b384>();
        case hash("BLAKE2b512"):
            return hash_or_mac_select_mode<BLAKE2b512>();
        case hash("BLAKE2s256"):
            return hash_or_mac_select_mode<BLAKE2s256>();
        case hash("BLAKE2s224"):
            return hash_or_mac_select_mode<BLAKE2s224>();
        case hash("BLAKE3"):
            return hash_select_mode<BLAKE3>();
        case hash("CRC32"):
            return hash_select_mode<CRC32>();
        case hash("CRC64"):
            return hash_select_mode<CRC64>();
        default:
            throw std::runtime_error("Invalid algorithm.");
        }
    }
};

int main(int argc, char **argv) {
    try {
        ArgsHandler handler(argv, argc);
        handler.select_algorithm();
    } catch (std::exception const &e) {
        fprintf(stderr,
                "Error: %s\n"
                "Usage: %s <algorithm> [key length] [key]\n"
                "Available algorithms (Hash only):\n"
                "    CRC32, CRC64, BLAKE3\n"
                "Available algorithms (Hash and MAC):\n"
                "    BLAKE2b384, BLAKE2b512, BLAKE2s256, BLAKE2s224\n"
                "Available algorithms (Hash and HMAC):\n"
                "    SHA224, SHA256, SHA3-224, SHA3-256, SHAKE128,\n"
                "    SHA384, SHA512, SHA3-384, SHA3-512, SHAKE256,\n"
                "    SM3, MD5, SHA0, SHA1, Whirlpool\n"
                "* Program will read from stdin and output to stdout.\n"
                "* Key should be a hex string.\n",
                e.what(), argv[0]);
        return 1;
    }
    return 0;
}
