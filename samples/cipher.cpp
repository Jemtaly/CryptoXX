#include <cstdio>
#include <stdexcept>
#include <string>

// include all block cipher algorithms we need
#include "CryptoXX/block/aria.hpp"
#include "CryptoXX/block/blowfish.hpp"
#include "CryptoXX/block/camellia.hpp"
#include "CryptoXX/block/cast128.hpp"
#include "CryptoXX/block/cast256.hpp"
#include "CryptoXX/block/des.hpp"
#include "CryptoXX/block/idea.hpp"
#include "CryptoXX/block/rijndael.hpp"
#include "CryptoXX/block/seed.hpp"
#include "CryptoXX/block/serpent.hpp"
#include "CryptoXX/block/sm4.hpp"
#include "CryptoXX/block/twofish.hpp"

// include all stream cipher algorithms we need
#include "CryptoXX/stream/chacha.hpp"
#include "CryptoXX/stream/rc4.hpp"
#include "CryptoXX/stream/salsa.hpp"
#include "CryptoXX/stream/zuc.hpp"

// the following includes are wrappers for block ciphers in different modes of operation and stream ciphers
#include "CryptoXX/cbc.hpp"
#include "CryptoXX/cfb.hpp"
#include "CryptoXX/ctr.hpp"
#include "CryptoXX/ecb.hpp"
#include "CryptoXX/ofb.hpp"
#include "CryptoXX/pcbc.hpp"
#include "CryptoXX/stream.hpp"

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

    // encrypt single block of data from command line arguments, and print the result (in hex format)
    template<typename BlockCipher>
    void block_cipher_encrypt() {
        auto key_arg = next_arg();
        uint8_t key[BlockCipher::KEY_SIZE];
        read_bin("key", BlockCipher::KEY_SIZE, key_arg, key);
        auto msg_arg = next_arg();
        uint8_t msg[BlockCipher::BLOCK_SIZE];
        read_bin("message", BlockCipher::BLOCK_SIZE, msg_arg, msg);
        if (next_arg()) {
            throw std::runtime_error("Too many arguments.");
        }
        BlockCipher(key).encrypt(msg, msg);
        print_hex(msg, BlockCipher::BLOCK_SIZE);
    }

    // decrypt single block of data from command line arguments, and print the result (in hex format)
    template<typename BlockCipher>
    void block_cipher_decrypt() {
        auto key_arg = next_arg();
        uint8_t key[BlockCipher::KEY_SIZE];
        read_bin("key", BlockCipher::KEY_SIZE, key_arg, key);
        auto msg_arg = next_arg();
        uint8_t msg[BlockCipher::BLOCK_SIZE];
        read_bin("message", BlockCipher::BLOCK_SIZE, msg_arg, msg);
        if (next_arg()) {
            throw std::runtime_error("Too many arguments.");
        }
        BlockCipher(key).decrypt(msg, msg);
        print_hex(msg, BlockCipher::BLOCK_SIZE);
    }

    // encrypt/decrypt data with block cipher mode of operation from stdin to stdout
    template<typename BlockCipherCrypter>
    void block_cipher_mode_perform() {
        auto key_arg = next_arg();
        uint8_t key[BlockCipherCrypter::KEY_SIZE];
        read_bin("key", BlockCipherCrypter::KEY_SIZE, key_arg, key);
        auto civ_arg = next_arg();
        uint8_t civ[BlockCipherCrypter::CIV_SIZE];
        if (BlockCipherCrypter::CIV_SIZE != 0 || civ_arg) {
            read_bin("iv", BlockCipherCrypter::CIV_SIZE, civ_arg, civ);
        }
        if (next_arg()) {
            throw std::runtime_error("Too many arguments.");
        }
        BlockCipherCrypter bcc(civ, key);
        uint8_t src[BUFSIZE], dst[BUFSIZE + BlockCipherCrypter::BLOCK_SIZE];
        size_t read;
        while ((read = fread(src, 1, BUFSIZE, stdin)) == BUFSIZE) {
            fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, stdout);
        }
        fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, stdout);
        if (uint8_t *end = bcc.fflush(dst)) {
            fwrite(dst, 1, end - (uint8_t *)dst, stdout);
        } else {
            fprintf(stderr, "Warning: Error occurred while flushing cipher.\n");
        }
    }

    // encrypt/decrypt data with stream cipher from stdin to stdout
    template<typename StreamCipherCrypter>
    void stream_cipher_mode_perform() {
        auto key_arg = next_arg();
        uint8_t key[StreamCipherCrypter::KEY_SIZE];
        read_bin("key", StreamCipherCrypter::KEY_SIZE, key_arg, key);
        auto civ_arg = next_arg();
        uint8_t civ[StreamCipherCrypter::CIV_SIZE];
        if (StreamCipherCrypter::CIV_SIZE != 0 || civ_arg) {
            read_bin("iv", StreamCipherCrypter::CIV_SIZE, civ_arg, civ);
        }
        if (next_arg()) {
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
    template<typename PseudoRandomGenerator>
    void stream_cipher_mode_generate() {
        auto key_arg = next_arg();
        uint8_t key[PseudoRandomGenerator::KEY_SIZE];
        read_bin("key", PseudoRandomGenerator::KEY_SIZE, key_arg, key);
        auto civ_arg = next_arg();
        uint8_t civ[PseudoRandomGenerator::CIV_SIZE];
        if (PseudoRandomGenerator::CIV_SIZE != 0 || civ_arg) {
            read_bin("iv", PseudoRandomGenerator::CIV_SIZE, civ_arg, civ);
        }
        if (next_arg()) {
            throw std::runtime_error("Too many arguments.");
        }
        PseudoRandomGenerator prg(civ, key);
        uint8_t dst[BUFSIZE];
        while (true) {
            prg.generate(dst, dst + BUFSIZE);
            fwrite(dst, 1, BUFSIZE, stdout);
        }
    }

    template<typename BlockCipher>
    void block_cipher_select_mode() {
        auto mode = next_arg();
        if (mode == nullptr) {
            throw std::runtime_error("No mode specified.");
        }
        switch (hash(mode)) {
        case hash("ECBEnc"):
            return block_cipher_mode_perform<ECBEncrypter<BlockCipher>>();
        case hash("ECBDec"):
            return block_cipher_mode_perform<ECBDecrypter<BlockCipher>>();
        case hash("CBCEnc"):
            return block_cipher_mode_perform<CBCEncrypter<BlockCipher>>();
        case hash("CBCDec"):
            return block_cipher_mode_perform<CBCDecrypter<BlockCipher>>();
        case hash("PCBCEnc"):
            return block_cipher_mode_perform<PCBCEncrypter<BlockCipher>>();
        case hash("PCBCDec"):
            return block_cipher_mode_perform<PCBCDecrypter<BlockCipher>>();
        case hash("CFBEnc"):
            return stream_cipher_mode_perform<CFBEncrypter<BlockCipher>>();
        case hash("CFBDec"):
            return stream_cipher_mode_perform<CFBDecrypter<BlockCipher>>();
        case hash("OFBEnc"):
            return stream_cipher_mode_perform<OFBEncrypter<BlockCipher>>();
        case hash("OFBDec"):
            return stream_cipher_mode_perform<OFBDecrypter<BlockCipher>>();
        case hash("OFBGen"):
            return stream_cipher_mode_generate<OFBGenerator<BlockCipher>>();
        case hash("CTREnc"):
            return stream_cipher_mode_perform<CTREncrypter<BlockCipher>>();
        case hash("CTRDec"):
            return stream_cipher_mode_perform<CTRDecrypter<BlockCipher>>();
        case hash("CTRGen"):
            return stream_cipher_mode_generate<CTRGenerator<BlockCipher>>();
        case hash("Enc"):
            return block_cipher_encrypt<BlockCipher>();
        case hash("Dec"):
            return block_cipher_decrypt<BlockCipher>();
        default:
            throw std::runtime_error("Invalid mode.");
        }
    }

    template<typename StreamCipher>
    void stream_cipher_select_mode() {
        auto mode = next_arg();
        if (mode == nullptr) {
            throw std::runtime_error("No mode specified.");
        }
        switch (hash(mode)) {
        case hash("Enc"):
            return stream_cipher_mode_perform<StreamCipherEncrypter<StreamCipher>>();
        case hash("Dec"):
            return stream_cipher_mode_perform<StreamCipherDecrypter<StreamCipher>>();
        case hash("Gen"):
            return stream_cipher_mode_generate<PseudoRandomGenerator<StreamCipher>>();
        default:
            throw std::runtime_error("Invalid mode.");
        }
    }

    void select_algorithm() {
        auto algorithm = next_arg();
        if (algorithm == nullptr) {
            throw std::runtime_error("No algorithm specified.");
        }
        switch (hash(algorithm)) {
        case hash("AES128"):
            return block_cipher_select_mode<AES128>();
        case hash("AES192"):
            return block_cipher_select_mode<AES192>();
        case hash("AES256"):
            return block_cipher_select_mode<AES256>();
        case hash("Twofish128"):
            return block_cipher_select_mode<Twofish128>();
        case hash("Twofish192"):
            return block_cipher_select_mode<Twofish192>();
        case hash("Twofish256"):
            return block_cipher_select_mode<Twofish256>();
        case hash("Serpent128"):
            return block_cipher_select_mode<Serpent128>();
        case hash("Serpent192"):
            return block_cipher_select_mode<Serpent192>();
        case hash("Serpent256"):
            return block_cipher_select_mode<Serpent256>();
        case hash("Camellia128"):
            return block_cipher_select_mode<Camellia128>();
        case hash("Camellia192"):
            return block_cipher_select_mode<Camellia192>();
        case hash("Camellia256"):
            return block_cipher_select_mode<Camellia256>();
        case hash("ARIA128"):
            return block_cipher_select_mode<ARIA128>();
        case hash("ARIA192"):
            return block_cipher_select_mode<ARIA192>();
        case hash("ARIA256"):
            return block_cipher_select_mode<ARIA256>();
        case hash("SM4"):
            return block_cipher_select_mode<SM4>();
        case hash("CAST128"):
            return block_cipher_select_mode<CAST128>();
        case hash("CAST256"):
            return block_cipher_select_mode<CAST256>();
        case hash("DES"):
            return block_cipher_select_mode<DES>();
        case hash("TDES2K"):
            return block_cipher_select_mode<TDES2K>();
        case hash("TDES3K"):
            return block_cipher_select_mode<TDES3K>();
        case hash("Blowfish"):
            return block_cipher_select_mode<Blowfish>();
        case hash("IDEA"):
            return block_cipher_select_mode<IDEA>();
        case hash("SEED"):
            return block_cipher_select_mode<SEED>();
        case hash("ChaCha20"):
            return stream_cipher_select_mode<ChaCha20>();
        case hash("Salsa20"):
            return stream_cipher_select_mode<Salsa20>();
        case hash("RC4"):
            return stream_cipher_select_mode<RC4>();
        case hash("ZUC"):
            return stream_cipher_select_mode<ZUC>();
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
                "* Key and iv should be hex strings.\n",
                e.what(), argv[0]);
        return 1;
    }
    return 0;
}
