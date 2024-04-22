#include <cstdio>
#include <string>
#include <stdexcept>
// include all block cipher algorithms we need
#include "CryptoXX/block/sm4.hpp"
#include "CryptoXX/block/des.hpp"
#include "CryptoXX/block/3des.hpp"
#include "CryptoXX/block/idea.hpp"
#include "CryptoXX/block/aria.hpp"
#include "CryptoXX/block/seed.hpp"
#include "CryptoXX/block/cast128.hpp"
#include "CryptoXX/block/cast256.hpp"
#include "CryptoXX/block/twofish.hpp"
#include "CryptoXX/block/serpent.hpp"
#include "CryptoXX/block/blowfish.hpp"
#include "CryptoXX/block/rijndael.hpp"
#include "CryptoXX/block/camellia.hpp"
// include all stream cipher algorithms we need
#include "CryptoXX/stream/rc4.hpp"
#include "CryptoXX/stream/zuc.hpp"
#include "CryptoXX/stream/salsa.hpp"
#include "CryptoXX/stream/chacha.hpp"
// the following includes are wrappers for block ciphers in different modes of operation and stream ciphers
#include "CryptoXX/stream.hpp"
#include "CryptoXX/cbc.hpp"
#include "CryptoXX/ecb.hpp"
#include "CryptoXX/pcbc.hpp"
#include "CryptoXX/cfb.hpp"
#include "CryptoXX/ctr.hpp"
#include "CryptoXX/ofb.hpp"
#define BUFSIZE 8192 // Same with OpenSSL default buffer size
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
void print_hex(uint8_t const *bin, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", bin[i]);
    }
    printf("\n");
}
void read_arg(std::string const &arg, uint8_t *dst, size_t len) {
    if (Argi == Argc) {
        throw std::runtime_error("No " + arg + " specified.");
    }
    if (not hex2bin(len, Argv[Argi++], dst)) {
        throw std::runtime_error("Invalid " + arg + ", should be a " + std::to_string(len) + "-byte hex string.");
    }
}
// encrypt single block of data from command line arguments, and print the result (in hex format)
template <typename BlockCipher>
void bc_enc() {
    uint8_t key[BlockCipher::KEY_SIZE];
    read_arg("key", key, BlockCipher::KEY_SIZE);
    uint8_t msg[BlockCipher::BLOCK_SIZE];
    read_arg("message", msg, BlockCipher::BLOCK_SIZE);
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.");
    }
    BlockCipher(key).encrypt(msg, msg);
    print_hex(msg, BlockCipher::BLOCK_SIZE);
}
// decrypt single block of data from command line arguments, and print the result (in hex format)
template <typename BlockCipher>
void bc_dec() {
    uint8_t key[BlockCipher::KEY_SIZE];
    read_arg("key", key, BlockCipher::KEY_SIZE);
    uint8_t msg[BlockCipher::BLOCK_SIZE];
    read_arg("message", msg, BlockCipher::BLOCK_SIZE);
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.");
    }
    BlockCipher(key).decrypt(msg, msg);
    print_hex(msg, BlockCipher::BLOCK_SIZE);
}
// encrypt/decrypt data with block cipher mode of operation from stdin to stdout
template <typename BlockCipherCrypter>
void bc_cry() {
    uint8_t key[BlockCipherCrypter::KEY_SIZE];
    read_arg("key", key, BlockCipherCrypter::KEY_SIZE);
    uint8_t civ[BlockCipherCrypter::CIV_SIZE];
    if (BlockCipherCrypter::CIV_SIZE != 0 || Argi < Argc) {
        read_arg("iv", civ, BlockCipherCrypter::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.");
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
        fprintf(stderr, "Warning: Error occurred while flushing cipher.\n");
    }
}
// encrypt/decrypt data with stream cipher from stdin to stdout
template <typename StreamCipherCrypter>
void sc_cry() {
    uint8_t key[StreamCipherCrypter::KEY_SIZE];
    read_arg("key", key, StreamCipherCrypter::KEY_SIZE);
    uint8_t civ[StreamCipherCrypter::CIV_SIZE];
    if (StreamCipherCrypter::CIV_SIZE != 0 || Argi < Argc) {
        read_arg("iv", civ, StreamCipherCrypter::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.");
    }
    StreamCipherCrypter scc(civ, key);
    uint8_t src[BUFSIZE], dst[BUFSIZE];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, stdin)) == BUFSIZE) {
        fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, stdout);
    }
    fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, stdout);
}
// generate pseudo-random data with stream cipher from stdin to stdout
template <typename PseudoRandomGenerator>
void pr_gen() {
    uint8_t key[PseudoRandomGenerator::KEY_SIZE];
    read_arg("key", key, PseudoRandomGenerator::KEY_SIZE);
    uint8_t civ[PseudoRandomGenerator::CIV_SIZE];
    if (PseudoRandomGenerator::CIV_SIZE != 0 || Argi < Argc) {
        read_arg("iv", civ, PseudoRandomGenerator::CIV_SIZE);
    }
    if (Argi < Argc) {
        throw std::runtime_error("Too many arguments.");
    }
    PseudoRandomGenerator prg(civ, key);
    uint8_t dst[BUFSIZE];
    while (true) {
        prg.generate(dst, dst + BUFSIZE);
        fwrite(dst, 1, BUFSIZE, stdout);
    }
}
constexpr uint32_t hash(char const *str) {
    return *str ? *str + hash(str + 1) * 16777619UL : 2166136261UL;
}
template <typename BlockCipher>
void bc_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No mode specified.");
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
    case hash("Enc"):
        bc_enc<BlockCipher>(); break;
    case hash("Dec"):
        bc_dec<BlockCipher>(); break;
    default:
        throw std::runtime_error("Invalid mode.");
    }
}
template <typename StreamCipher>
void sc_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No mode specified.");
    }
    switch (hash(Argv[Argi++])) {
    case hash("Enc"):
        sc_cry<StreamCipherEncrypter<StreamCipher>>(); break;
    case hash("Dec"):
        sc_cry<StreamCipherDecrypter<StreamCipher>>(); break;
    case hash("Gen"):
        pr_gen<PseudoRandomGenerator<StreamCipher>>(); break;
    default:
        throw std::runtime_error("Invalid mode.");
    }
}
void alg_select() {
    if (Argi == Argc) {
        throw std::runtime_error("No algorithm specified.");
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
    case hash("ARIA128"):
        bc_select<ARIA128>(); break;
    case hash("ARIA192"):
        bc_select<ARIA192>(); break;
    case hash("ARIA256"):
        bc_select<ARIA256>(); break;
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
    case hash("SEED"):
        bc_select<SEED>(); break;
    case hash("ChaCha20"):
        sc_select<ChaCha20>(); break;
    case hash("Salsa20"):
        sc_select<Salsa20>(); break;
    case hash("RC4"):
        sc_select<RC4>(); break;
    case hash("ZUC"):
        sc_select<ZUC>(); break;
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
                "Usage: %s <algorithm> <mode> <key> [iv]\n"
                "Supported stream ciphers:\n"
                "    ChaCha20, Salsa20, RC4, ZUC\n"
                "Supported stream cipher modes:\n"
                "    Enc, Dec, Gen\n"
                "Supported block ciphers:\n"
                "    ARIA128, Camellia128, Serpent128, Twofish128,\n"
                "    ARIA192, Camellia192, Serpent192, Twofish192,\n"
                "    ARIA256, Camellia256, Serpent256, Twofish256,\n"
                "    CAST128, DES, TDES2K, TDES3K, SEED, Blowfish,\n"
                "    CAST256, SM4, IDES, AES128, AES192, AES256\n"
                "Supported block cipher modes:\n"
                "    ECBEnc, ECBDec, CTREnc, CTRDec, CTRGen,\n"
                "    CFBEnc, CFBDec, OFBEnc, OFBDec, OFBGen,\n"
                "    CBCEnc, CBCDec, PCBCEnc, PCBCDec\n"
                "* Program will read from stdin and write to stdout.\n"
                "* Key and iv should be hex strings.\n", e.what(), argv[0]);
        return 1;
    }
    return 0;
}
