#include <cstdio>
#include <string>
#include <stdexcept>
#include "block/sm4.hpp"
#include "block/des.hpp"
#include "block/3des.hpp"
#include "block/idea.hpp"
#include "block/cast128.hpp"
#include "block/cast256.hpp"
#include "block/twofish.hpp"
#include "block/serpent.hpp"
#include "block/blowfish.hpp"
#include "block/rijndael.hpp"
#include "block/camellia.hpp"
#include "block/cbc.hpp"
#include "block/ecb.hpp"
#include "block/pcbc.hpp"
#include "ctak/cfb.hpp"
#include "stream/ctr.hpp"
#include "stream/ofb.hpp"
#include "stream/rc4.hpp"
#include "stream/zuc.hpp"
#include "stream/salsa.hpp"
#include "stream/chacha.hpp"
#define BUFSIZE 65536
static char **Argv;
static int Argc;
static int Argi;
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
        throw std::runtime_error("No key specified.\n");
    }
    if (not hex2bin(key_size, Argv[Argi++], key)) {
        throw std::runtime_error("Invalid key, should be a " + std::to_string(key_size) + "-byte hex string.\n");
    }
}
void read_civ(uint8_t *civ, size_t civ_size) {
    if (Argi == Argc) {
        throw std::runtime_error("No iv specified.\n");
    }
    if (not hex2bin(civ_size, Argv[Argi++], civ)) {
        throw std::runtime_error("Invalid iv, should be a " + std::to_string(civ_size) + "-byte hex string.\n");
    }
}
template <typename StreamCipherCrypter>
void sc_cry() {
    uint8_t key[StreamCipherCrypter::KEY_SIZE];
    static_assert(StreamCipherCrypter::KEY_SIZE != 0);
    read_key(key, StreamCipherCrypter::KEY_SIZE);
    uint8_t civ[StreamCipherCrypter::CIV_SIZE];
    if (StreamCipherCrypter::CIV_SIZE != 0) {
        read_civ(civ, StreamCipherCrypter::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.\n");
    }
    StreamCipherCrypter scc(civ, key);
    uint8_t src[BUFSIZE], dst[BUFSIZE];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, stdin)) == BUFSIZE) {
        fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, stdout);
    }
    fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, stdout);
}
template <typename PseudoRandomGenerator>
void pr_gen() {
    uint8_t key[PseudoRandomGenerator::KEY_SIZE];
    static_assert(PseudoRandomGenerator::KEY_SIZE != 0);
    read_key(key, PseudoRandomGenerator::KEY_SIZE);
    uint8_t civ[PseudoRandomGenerator::CIV_SIZE];
    if (PseudoRandomGenerator::CIV_SIZE != 0) {
        read_civ(civ, PseudoRandomGenerator::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.\n");
    }
    PseudoRandomGenerator prg(civ, key);
    uint8_t dst[BUFSIZE];
    while (true) {
        prg.generate(dst, dst + BUFSIZE);
        fwrite(dst, 1, BUFSIZE, stdout);
    }
}
template <typename BlockCipherCrypter>
void bc_cry() {
    uint8_t key[BlockCipherCrypter::KEY_SIZE];
    static_assert(BlockCipherCrypter::KEY_SIZE != 0);
    read_key(key, BlockCipherCrypter::KEY_SIZE);
    uint8_t civ[BlockCipherCrypter::CIV_SIZE];
    if (BlockCipherCrypter::CIV_SIZE != 0) {
        read_civ(civ, BlockCipherCrypter::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.\n");
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
void bc_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No mode specified.\n");
    }
    switch (hash(Argv[Argi++])) {
    case hash("ECBEnc"):
        bc_cry<ECBEncrypter<BlockCipher>>(); break;
    case hash("ECBDec"):
        bc_cry<ECBDecrypter<BlockCipher>>(); break;
    case hash("CBCEnc"):
        bc_cry<CBCEncrypter<BlockCipher>>(); break;
    case hash("CBCDec"):
        bc_cry<CBCDecrypter<BlockCipher>>(); break;
    case hash("PCBCEnc"):
        bc_cry<PCBCEncrypter<BlockCipher>>(); break;
    case hash("PCBCDec"):
        bc_cry<PCBCDecrypter<BlockCipher>>(); break;
    case hash("CFBEnc"):
        sc_cry<CFBEncrypter<BlockCipher>>(); break;
    case hash("CFBDec"):
        sc_cry<CFBDecrypter<BlockCipher>>(); break;
    case hash("OFBEnc"):
        sc_cry<OFBEncrypter<BlockCipher>>(); break;
    case hash("OFBDec"):
        sc_cry<OFBDecrypter<BlockCipher>>(); break;
    case hash("OFBGen"):
        pr_gen<OFBGenerator<BlockCipher>>(); break;
    case hash("CTREnc"):
        sc_cry<CTREncrypter<BlockCipher>>(); break;
    case hash("CTRDec"):
        sc_cry<CTRDecrypter<BlockCipher>>(); break;
    case hash("CTRGen"):
        pr_gen<CTRGenerator<BlockCipher>>(); break;
    default:
        throw std::runtime_error("Invalid mode.\n"); break;
    }
}
template <typename StreamCipher>
void sc_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No mode specified.\n");
    }
    switch (hash(Argv[Argi++])) {
    case hash("Enc"):
        sc_cry<StreamCipherEncrypter<StreamCipher>>(); break;
    case hash("Dec"):
        sc_cry<StreamCipherDecrypter<StreamCipher>>(); break;
    case hash("Gen"):
        pr_gen<PseudoRandomGenerator<StreamCipher>>(); break;
    default:
        throw std::runtime_error("Invalid mode.\n"); break;
    }
}
void alg_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No algorithm specified.\n");
    }
    switch (hash(Argv[Argi++])) {
    case hash("AES128"):
        bc_select<AES128>(); break;
    case hash("AES192"):
        bc_select<AES192>(); break;
    case hash("AES256"):
        bc_select<AES256>(); break;
    case hash("Twofish128"):
        bc_select<Twofish128>(); break;
    case hash("Twofish192"):
        bc_select<Twofish192>(); break;
    case hash("Twofish256"):
        bc_select<Twofish256>(); break;
    case hash("Serpent128"):
        bc_select<Serpent128>(); break;
    case hash("Serpent192"):
        bc_select<Serpent192>(); break;
    case hash("Serpent256"):
        bc_select<Serpent256>(); break;
    case hash("Camellia128"):
        bc_select<Camellia128>(); break;
    case hash("Camellia192"):
        bc_select<Camellia192>(); break;
    case hash("Camellia256"):
        bc_select<Camellia256>(); break;
    case hash("SM4"):
        bc_select<SM4>(); break;
    case hash("CAST128"):
        bc_select<CAST128>(); break;
    case hash("CAST256"):
        bc_select<CAST256>(); break;
    case hash("DES"):
        bc_select<DES>(); break;
    case hash("TDES2K"):
        bc_select<TDES2K>(); break;
    case hash("TDES3K"):
        bc_select<TDES3K>(); break;
    case hash("Blowfish"):
        bc_select<Blowfish>(); break;
    case hash("IDEA"):
        bc_select<IDEA>(); break;
    case hash("ChaCha20"):
        sc_select<ChaCha20>(); break;
    case hash("Salsa20"):
        sc_select<Salsa20>(); break;
    case hash("RC4"):
        sc_select<RC4>(); break;
    case hash("ZUC"):
        sc_select<ZUC>(); break;
    default:
        throw std::runtime_error("Invalid algorithm.\n"); break;
    }
}
int main(int argc, char **argv) {
    Argv = argv;
    Argc = argc;
    Argi = 1;
    try {
        alg_select();
    } catch (std::exception const &e) {
        fprintf(stderr, "Error: %s", e.what());
        fprintf(stderr, "Usage: %s <algorithm> <mode> <key> [iv]\n", argv[0]);
        return 1;
    }
    return 0;
}
