#include <cstdio>
#include <string>
#include <stdexcept>
// include all hash algorithms we need
#include "CryptoXX/hash/crc.hpp"
#include "CryptoXX/hash/sm3.hpp"
#include "CryptoXX/hash/md5.hpp"
#include "CryptoXX/hash/sha.hpp"
#include "CryptoXX/hash/sha256.hpp"
#include "CryptoXX/hash/sha512.hpp"
#include "CryptoXX/hash/keccak.hpp"
#include "CryptoXX/hash/blake2b.hpp"
#include "CryptoXX/hash/blake2s.hpp"
#include "CryptoXX/hash/blake3.hpp"
#include "CryptoXX/hash/whirlpool.hpp"
// include hash.hpp and hmac.hpp, which contains the wrapper classes for hash and hmac
#include "CryptoXX/hash/hash.hpp"
#include "CryptoXX/hash/hmac.hpp"
#define BUFSIZE 65536
char **Argv;
int Argc, Argi;
bool hex2bin(size_t len, char const *hex, uint8_t *bin) {
    for (size_t i = 0; i < len * 2; ++i) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            bin[i / 2] = bin[i / 2] & (i % 2 ? 0xf0 : 0x0f) | (hex[i] + 0 & 0xf) << (i % 2 ? 0 : 4);
        } else if (hex[i] >= 'a' && hex[i] <= 'f' || hex[i] >= 'A' && hex[i] <= 'F') {
            bin[i / 2] = bin[i / 2] & (i % 2 ? 0xf0 : 0x0f) | (hex[i] + 9 & 0xf) << (i % 2 ? 0 : 4);
        } else {
            return false;
        }
    }
    return hex[len * 2] == '\0';
}
void read_key(uint8_t *key, size_t key_size) {
    if (Argi == Argc) {
        throw std::runtime_error("No key specified.");
    }
    if (not hex2bin(key_size, Argv[Argi++], key)) {
        throw std::runtime_error("Invalid key, should be a " + std::to_string(key_size) + "-byte hex string.");
    }
}
template <typename HashWrapper, typename ...Args>
void do_hash(Args &&...args) {
    HashWrapper hw(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    size_t read;
    while ((read = fread(buf, 1, BUFSIZE, stdin)) == BUFSIZE) {
        hw.update(buf, (uint8_t *)buf + BUFSIZE);
    }
    hw.update(buf, (uint8_t *)buf + read);
    uint8_t digest[HashWrapper::DIGEST_SIZE];
    hw.digest(digest);
    for (int i = 0; i < HashWrapper::DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
constexpr uint32_t hash(char const *str) {
    return *str ? *str + hash(str + 1) * 16777619UL : 2166136261UL;
}
template <typename Hash>
void h_select() {
    if (Argi == Argc) {
        do_hash<HashWrapper<Hash>>();
    } else {
        int len;
        try {
            len = std::stoi(Argv[Argi++]);
        } catch (std::exception const &e) {
            throw std::runtime_error("Key length should be an integer.");
        }
        if (len < 0) {
            throw std::runtime_error("Invalid key length, required: 0 <= len.");
        }
        uint8_t bin[len];
        if (len != 0 || Argi < Argc) {
            read_key(bin, len);
        }
        if (Argi < Argc) {
            throw std::runtime_error("Too many arguments.");
        }
        do_hash<HMACWrapper<Hash>>((uint8_t *)bin, len);
    }
}
template <typename Hash>
void b_select() {
    if (Argi == Argc) {
        do_hash<HashWrapper<Hash>>();
    } else {
        int len;
        try {
            len = std::stoi(Argv[Argi++]);
        } catch (std::exception const &e) {
            throw std::runtime_error("Key length should be an integer.");
        }
        if (len < 0 || len > Hash::BLOCK_SIZE) {
            throw std::runtime_error("Invalid key length, required: 0 <= len <= " + std::to_string(Hash::BLOCK_SIZE) + ".");
        }
        uint8_t bin[len];
        if (len != 0 || Argi < Argc) {
            read_key(bin, len);
        }
        if (Argi < Argc) {
            throw std::runtime_error("Too many arguments.");
        }
        do_hash<HashWrapper<Hash>>((uint8_t *)bin, len);
    }
}
template <typename Hash>
void c_select() {
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments, the algorithm does not require a key.");
    }
    do_hash<HashWrapper<Hash>>();
}
void alg_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No algorithm specified.");
    }
    switch (hash(Argv[Argi++])) {
    case hash("MD5"):
        h_select<MD5>(); break;
    case hash("SHA"):
        h_select<SHA>(); break;
    case hash("SHA224"):
        h_select<SHA224>(); break;
    case hash("SHA256"):
        h_select<SHA256>(); break;
    case hash("SHA384"):
        h_select<SHA384>(); break;
    case hash("SHA512"):
        h_select<SHA512>(); break;
    case hash("SHA3-224"):
        h_select<SHA3<224>>(); break;
    case hash("SHA3-256"):
        h_select<SHA3<256>>(); break;
    case hash("SHA3-384"):
        h_select<SHA3<384>>(); break;
    case hash("SHA3-512"):
        h_select<SHA3<512>>(); break;
    case hash("SHAKE128"):
        h_select<SHAKE<128, 256>>(); break;
    case hash("SHAKE256"):
        h_select<SHAKE<256, 512>>(); break;
    case hash("SM3"):
        h_select<SM3>(); break;
    case hash("Whirlpool"):
        h_select<Whirlpool>(); break;
    case hash("BLAKE2b384"):
        b_select<BLAKE2b384>(); break;
    case hash("BLAKE2b512"):
        b_select<BLAKE2b512>(); break;
    case hash("BLAKE2s256"):
        b_select<BLAKE2s256>(); break;
    case hash("BLAKE2s224"):
        b_select<BLAKE2s224>(); break;
    case hash("BLAKE3"):
        c_select<BLAKE3>(); break;
    case hash("CRC32"):
        c_select<CRC32>(); break;
    case hash("CRC64"):
        c_select<CRC64>(); break;
    default:
        throw std::runtime_error("Invalid algorithm.");
    }
}
int main(int argc, char **argv) {
    Argv = argv;
    Argc = argc;
    Argi = 1;
    try {
        alg_select();
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
                "    SM3, MD5, SHA, Whirlpool\n"
                "* Program will read from stdin and output to stdout.\n"
                "* Key should be a hex string.\n", e.what(), argv[0]);
        return 1;
    }
    return 0;
}
