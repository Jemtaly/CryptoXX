#include <cstdio>
#include <string>
#include <stdexcept>
#include "block/des.hpp"
#include "block/idea.hpp"
#include "block/3des.hpp"
#include "block/blowfish.hpp"
#include "block/twofish.hpp"
#include "block/rijndael.hpp"
#include "block/serpent.hpp"
#include "block/camellia.hpp"
#include "block/cast128.hpp"
#include "block/cast256.hpp"
#include "block/sm4.hpp"
#include "block/cbc.hpp"
#include "block/ecb.hpp"
#include "block/pcbc.hpp"
#include "stream/chacha.hpp"
#include "stream/salsa.hpp"
#include "stream/rc4.hpp"
#include "stream/zuc.hpp"
#include "stream/ctr.hpp"
#include "stream/ofb.hpp"
#include "ctak/cfb.hpp"
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
void read_key(int argc, char *argv[], uint8_t *key, size_t key_size) {
    if (argc == 0) {
        throw std::runtime_error("No key specified.\n");
    }
    if (not hex2bin(key_size, argv[0], key)) {
        throw std::runtime_error("Invalid key. Required length: " + std::to_string(key_size * 2) + ".\n");
    }
}
void read_civ(int argc, char *argv[], uint8_t *civ, size_t civ_size) {
    if (argc == 0) {
        throw std::runtime_error("No iv specified.\n");
    }
    if (not hex2bin(civ_size, argv[0], civ)) {
        throw std::runtime_error("Invalid iv. Required length: " + std::to_string(civ_size * 2) + ".\n");
    }
}
template <typename StreamCipherCrypter>
void sc_cry(int argc, char *argv[]) {
    uint8_t key[StreamCipherCrypter::KEY_SIZE];
    if (StreamCipherCrypter::KEY_SIZE != 0) {
        read_key(argc--, argv++, key, StreamCipherCrypter::KEY_SIZE);
    }
    uint8_t civ[StreamCipherCrypter::CIV_SIZE];
    if (StreamCipherCrypter::CIV_SIZE != 0) {
        read_civ(argc--, argv++, civ, StreamCipherCrypter::CIV_SIZE);
    }
    StreamCipherCrypter scc(civ, key);
    uint8_t src[BUFSIZE], dst[BUFSIZE];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, stdin)) == BUFSIZE) {
        fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, stdout);
    }
    fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, stdout);
}
template <typename StreamCipherGenerator>
void sc_gen(int argc, char *argv[]) {
    uint8_t key[StreamCipherGenerator::KEY_SIZE];
    if (StreamCipherGenerator::KEY_SIZE != 0) {
        read_key(argc--, argv++, key, StreamCipherGenerator::KEY_SIZE);
    }
    uint8_t civ[StreamCipherGenerator::CIV_SIZE];
    if (StreamCipherGenerator::CIV_SIZE != 0) {
        read_civ(argc--, argv++, civ, StreamCipherGenerator::CIV_SIZE);
    }
    StreamCipherGenerator scg(civ, key);
    uint8_t dst[BUFSIZE];
    while (true) {
        scg.generate(dst, dst + BUFSIZE);
        fwrite(dst, 1, BUFSIZE, stdout);
    }
}
template <typename BlockCipherCrypter>
void bc_cry(int argc, char *argv[]) {
    uint8_t key[BlockCipherCrypter::KEY_SIZE];
    if (BlockCipherCrypter::KEY_SIZE != 0) {
        read_key(argc--, argv++, key, BlockCipherCrypter::KEY_SIZE);
    }
    uint8_t civ[BlockCipherCrypter::CIV_SIZE];
    if (BlockCipherCrypter::CIV_SIZE != 0) {
        read_civ(argc--, argv++, civ, BlockCipherCrypter::CIV_SIZE);
    }
    BlockCipherCrypter bcc(civ, key);
    uint8_t src[BUFSIZE], dst[BUFSIZE + BlockCipherCrypter::BLOCK_SIZE];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, stdin)) == BUFSIZE) {
        fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, stdout);
    }
    fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, stdout);
    if (uint8_t *end = bcc.fflush(dst); end >= (uint8_t *)dst) {
        fwrite(dst, 1, end - (uint8_t *)dst, stdout);
    } else {
        throw std::runtime_error("BlockCipherCrypter::fflush() failed, the input may be corrupted.\n");
    }
}
constexpr uint32_t hash(char const *str) {
    return *str ? *str + hash(str + 1) * 16777619UL : 2166136261UL;
}
template <typename BlockCipher>
void bc_select(int argc, char *argv[]) {
    if (argc == 0) {
        throw std::runtime_error("No mode specified.\n");
    }
    switch (hash(argv[0])) {
    case hash("ECBEnc"):
        bc_cry<ECBEncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("ECBDec"):
        bc_cry<ECBDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CBCEnc"):
        bc_cry<CBCEncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CBCDec"):
        bc_cry<CBCDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("PCBCEnc"):
        bc_cry<PCBCEncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("PCBCDec"):
        bc_cry<PCBCDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CFBEnc"):
        sc_cry<CFBEncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CFBDec"):
        sc_cry<CFBDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("OFBEnc"):
        sc_cry<OFBEncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("OFBDec"):
        sc_cry<OFBDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("OFBGen"):
        sc_gen<OFBGenerator<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CTREnc"):
        sc_cry<CTREncrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CTRDec"):
        sc_cry<CTRDecrypter<BlockCipher>>(argc - 1, argv + 1); break;
    case hash("CTRGen"):
        sc_gen<CTRGenerator<BlockCipher>>(argc - 1, argv + 1); break;
    default:
        throw std::runtime_error("Invalid mode.\n"); break;
    }
}
template <typename StreamCipher>
void sc_select(int argc, char *argv[]) {
    if (argc == 0) {
        throw std::runtime_error("No mode specified.\n");
    }
    switch (hash(argv[0])) {
    case hash("Enc"):
        sc_cry<StreamCipherEncrypter<StreamCipher>>(argc - 1, argv + 1); break;
    case hash("Dec"):
        sc_cry<StreamCipherDecrypter<StreamCipher>>(argc - 1, argv + 1); break;
    case hash("Gen"):
        sc_gen<PseudoRandomGenerator<StreamCipher>>(argc - 1, argv + 1); break;
    default:
        throw std::runtime_error("Invalid mode.\n"); break;
    }
}
void alg_select(int argc, char *argv[]) {
    if (argc == 0) {
        throw std::runtime_error("No algorithm specified.\n");
    }
    switch (hash(argv[0])) {
    case hash("AES128"):
        bc_select<AES128>(argc - 1, argv + 1); break;
    case hash("AES192"):
        bc_select<AES192>(argc - 1, argv + 1); break;
    case hash("AES256"):
        bc_select<AES256>(argc - 1, argv + 1); break;
    case hash("DES"):
        bc_select<DES>(argc - 1, argv + 1); break;
    case hash("TDES2K"):
        bc_select<TDES2K>(argc - 1, argv + 1); break;
    case hash("TDES3K"):
        bc_select<TDES3K>(argc - 1, argv + 1); break;
    case hash("Blowfish"):
        bc_select<Blowfish>(argc - 1, argv + 1); break;
    case hash("Camellia128"):
        bc_select<Camellia128>(argc - 1, argv + 1); break;
    case hash("Camellia192"):
        bc_select<Camellia192>(argc - 1, argv + 1); break;
    case hash("Camellia256"):
        bc_select<Camellia256>(argc - 1, argv + 1); break;
    case hash("Twofish128"):
        bc_select<Twofish128>(argc - 1, argv + 1); break;
    case hash("Twofish192"):
        bc_select<Twofish192>(argc - 1, argv + 1); break;
    case hash("Twofish256"):
        bc_select<Twofish256>(argc - 1, argv + 1); break;
    case hash("Serpent128"):
        bc_select<Serpent128>(argc - 1, argv + 1); break;
    case hash("Serpent192"):
        bc_select<Serpent192>(argc - 1, argv + 1); break;
    case hash("Serpent256"):
        bc_select<Serpent256>(argc - 1, argv + 1); break;
    case hash("CAST128"):
        bc_select<CAST128>(argc - 1, argv + 1); break;
    case hash("CAST256"):
        bc_select<CAST256>(argc - 1, argv + 1); break;
    case hash("IDEA"):
        bc_select<IDEA>(argc - 1, argv + 1); break;
    case hash("SM4"):
        bc_select<SM4>(argc - 1, argv + 1); break;
    case hash("ChaCha20"):
        sc_select<ChaCha20>(argc - 1, argv + 1); break;
    case hash("Salsa20"):
        sc_select<Salsa20>(argc - 1, argv + 1); break;
    case hash("RC4"):
        sc_select<RC4>(argc - 1, argv + 1); break;
    case hash("ZUC"):
        sc_select<ZUC>(argc - 1, argv + 1); break;
    default:
        throw std::runtime_error("Invalid algorithm.\n"); break;
    }
}
int main(int argc, char *argv[]) {
    try {
        alg_select(argc - 1, argv + 1);
    } catch (std::exception const &e) {
        fprintf(stderr, "Error: %s", e.what());
        fprintf(stderr, "Usage: %s <algorithm> <mode> <key> [iv]\n", argv[0]);
        return 1;
    }
    return 0;
}
