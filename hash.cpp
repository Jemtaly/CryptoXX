#include <stdio.h>
#include <stdlib.h>
#include "hash/crc.hpp"
#include "hash/sm3.hpp"
#include "hash/md5.hpp"
#include "hash/sha.hpp"
#include "hash/sha256.hpp"
#include "hash/sha512.hpp"
#include "hash/keccak.hpp"
#include "hash/blake2b.hpp"
#include "hash/blake2s.hpp"
#include "hash/blake3.hpp"
#include "hash/whirlpool.hpp"
#include "hash/hmac.hpp"
#define BUFSIZE 65536
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
template <typename HashWrapper, typename ...Args>
void do_hash(Args &&...args) {
    HashWrapper hash(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    size_t read;
    while ((read = fread(buf, 1, BUFSIZE, stdin)) == BUFSIZE) {
        hash.update(buf, (uint8_t *)buf + BUFSIZE);
    }
    hash.update(buf, (uint8_t *)buf + read);
    uint8_t digest[HashWrapper::DIGEST_SIZE];
    hash.digest(digest);
    for (int i = 0; i < HashWrapper::DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
constexpr uint32_t hash(char const *str) {
    return *str ? *str + hash(str + 1) * 16777619UL : 2166136261UL;
}
template <typename Hash>
void hmac_select(int argc, char *argv[]) {
    if (argc == 0) {
        do_hash<HashWrapper<Hash>>();
    } else {
        auto len = strlen(argv[0]) / 2;
        if (len < 0) {
            fprintf(stderr, "Error: Key too short.\n");
            exit(1);
        }
        uint8_t bin[len];
        if (not hex2bin(len, argv[0], bin)) {
            fprintf(stderr, "Error: Invalid hex string.\n");
            exit(1);
        }
        do_hash<HMACWrapper<Hash>>((uint8_t *)bin, len);
    }
}
template <typename Hash>
void bmac_select(int argc, char *argv[]) {
    if (argc == 0) {
        do_hash<HashWrapper<Hash>>();
    } else {
        size_t len = strlen(argv[0]) / 2;
        if (len > Hash::BLOCK_SIZE) {
            fprintf(stderr, "Error: Key too long.\n");
            exit(1);
        }
        uint8_t bin[len];
        if (not hex2bin(len, argv[0], bin)) {
            fprintf(stderr, "Error: Invalid hex string.\n");
            exit(1);
        }
        do_hash<HashWrapper<Hash>>((uint8_t *)bin, len);
    }
}
void alg_select(int argc, char *argv[]) {
    if (argc == 0) {
        fprintf(stderr, "Error: No algorithm specified.\n");
        exit(1);
    }
    switch (hash(argv[0])) {
    case hash("SM3"):
        hmac_select<SM3>(argc - 1, argv + 1); break;
    case hash("MD5"):
        hmac_select<MD5>(argc - 1, argv + 1); break;
    case hash("SHA"):
        hmac_select<SHA>(argc - 1, argv + 1); break;
    case hash("SHA224"):
        hmac_select<SHA224>(argc - 1, argv + 1); break;
    case hash("SHA256"):
        hmac_select<SHA256>(argc - 1, argv + 1); break;
    case hash("SHA384"):
        hmac_select<SHA384>(argc - 1, argv + 1); break;
    case hash("SHA512"):
        hmac_select<SHA512>(argc - 1, argv + 1); break;
    case hash("SHA3-224"):
        hmac_select<SHA3<224>>(argc - 1, argv + 1); break;
    case hash("SHA3-256"):
        hmac_select<SHA3<256>>(argc - 1, argv + 1); break;
    case hash("SHA3-384"):
        hmac_select<SHA3<384>>(argc - 1, argv + 1); break;
    case hash("SHA3-512"):
        hmac_select<SHA3<512>>(argc - 1, argv + 1); break;
    case hash("SHAKE128"):
        hmac_select<SHAKE<128, 256>>(argc - 1, argv + 1); break;
    case hash("SHAKE256"):
        hmac_select<SHAKE<256, 512>>(argc - 1, argv + 1); break;
    case hash("BLAKE2b384"):
        hmac_select<BLAKE2b384>(argc - 1, argv + 1); break;
    case hash("BLAKE2b512"):
        hmac_select<BLAKE2b512>(argc - 1, argv + 1); break;
    case hash("BLAKE2s256"):
        hmac_select<BLAKE2s256>(argc - 1, argv + 1); break;
    case hash("BLAKE2s224"):
        hmac_select<BLAKE2s224>(argc - 1, argv + 1); break;
    case hash("Whirlpool"):
        hmac_select<Whirlpool>(argc - 1, argv + 1); break;
    case hash("CRC32"):
        do_hash<HashWrapper<CRC32>>(); break;
    case hash("CRC64"):
        do_hash<HashWrapper<CRC64>>(); break;
    default:
        fprintf(stderr, "Error: Invalid algorithm.\n"); break;
    }
}
int main(int argc, char *argv[]) {
    alg_select(argc - 1, argv + 1);
    return 0;
}
