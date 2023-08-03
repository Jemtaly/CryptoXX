#include <stdio.h>
#include "block/sm4.hpp"
#include "block/rijndael.hpp"
#include "block/twofish.hpp"
#include "block/serpent.hpp"
#include "block/cbc.hpp"
#include "block/ecb.hpp"
#include "async/cfb.hpp"
#include "stream/ctr.hpp"
#include "stream/ofb.hpp"
#define BLK BlockCipherCrypter::BLOCK_SIZE
#define BUFSIZE 65536
#define REC_ERR 1
#define REC_IFP 2
#define REC_OFP 4
template <typename StreamCipherCrypter, typename... Args>
void sc_crypt(FILE *ifp, FILE *ofp, Args &&...args) {
    StreamCipherCrypter scc(std::forward<Args>(args)...);
    uint8_t src[BUFSIZE], dst[BUFSIZE];
    while (fwrite(dst, 1, scc.update(dst, src, (uint8_t *)src + fread(src, 1, BUFSIZE, ifp)) - (uint8_t *)dst, ofp) == BUFSIZE) {}
}
template <typename BlockCipherCrypter, typename... Args>
void bc_crypt(FILE *ifp, FILE *ofp, Args &&...args) {
    BlockCipherCrypter bcc(std::forward<Args>(args)...);
    uint8_t src[BUFSIZE], dst[BUFSIZE + BLK];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE) {
        fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, ofp);
    }
    fwrite(dst, 1, bcc.update(dst, src, (uint8_t *)src + read) - (uint8_t *)dst, ofp);
    if (uint8_t *end = bcc.fflush(dst); end) {
        fwrite(dst, 1, end - (uint8_t *)dst, ofp);
    } else {
        fprintf(stderr, "Error: BlockCipherCrypter::fflush() failed, the input may be corrupted.\n");
    }
}
template <typename BlockCipher>
void process(char mod, char opm, FILE *ifp, FILE *ofp, uint8_t const *civ, uint8_t const *key) {
    switch (mod) {
    case 'N': sc_crypt<StreamCipherCrypter<CTRMode<BlockCipher>>>(ifp, ofp, civ, key); break;
    case 'O': sc_crypt<StreamCipherCrypter<OFBMode<BlockCipher>>>(ifp, ofp, civ, key); break;
    case 'C':
        switch (opm) {
        case 'e': sc_crypt<AsyncCipherEncrypter<CFBMode<BlockCipher>>>(ifp, ofp, civ, key); break;
        case 'd': sc_crypt<AsyncCipherDecrypter<CFBMode<BlockCipher>>>(ifp, ofp, civ, key); break;
        } break;
    case 'H':
        switch (opm) {
        case 'e': bc_crypt<BlockCipherEncrypter<CBCMode<BlockCipher>>>(ifp, ofp, civ, key); break;
        case 'd': bc_crypt<BlockCipherDecrypter<CBCMode<BlockCipher>>>(ifp, ofp, civ, key); break;
        } break;
    default:
        switch (opm) {
        case 'e': bc_crypt<BlockCipherEncrypter<ECBMode<BlockCipher>>>(ifp, ofp, key); break;
        case 'd': bc_crypt<BlockCipherDecrypter<ECBMode<BlockCipher>>>(ifp, ofp, key); break;
        } break;
    }
}
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
int main(int argc, char *argv[]) {
    long rec = 0;
    char alg = 0;
    char mod = 0;
    char opm = 0;
    FILE *ifp = stdin, *ofp = stdout;
    uint8_t *key = NULL, *civ = NULL;
    for (int i = 1; (rec & REC_ERR) == 0 && i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'E' && argv[i][2] == '\0') {
                if (mod == 0) {
                    mod = 'E';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'N' && argv[i][2] == '\0') {
                if (mod == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    mod = 'N';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'O' && argv[i][2] == '\0') {
                if (mod == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    mod = 'O';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'C' && argv[i][2] == '\0') {
                if (mod == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    mod = 'C';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'H' && argv[i][2] == '\0') {
                if (mod == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    mod = 'H';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'd' && argv[i][2] == '\0') {
                if (opm == 0) {
                    opm = 'd';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'e' && argv[i][2] == '\0') {
                if (opm == 0) {
                    opm = 'e';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '1' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    alg = '1';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '2' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    alg = '2';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '3' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    alg = '3';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    alg = '4';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '5' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    alg = '5';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '6' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    alg = '6';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '7' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    alg = '7';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    alg = '8';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '9' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    alg = '9';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '0' && argv[i][2] == '\0') {
                if (alg == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    alg = '0';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'i' && argv[i][2] == '\0') {
                if ((rec & REC_IFP) == 0 && i + 1 < argc && (ifp = fopen(argv[++i], "rb"))) {
                    rec |= REC_IFP;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'o' && argv[i][2] == '\0') {
                if ((rec & REC_OFP) == 0 && i + 1 < argc && (ofp = fopen(argv[++i], "wb"))) {
                    rec |= REC_OFP;
                } else {
                    rec |= REC_ERR;
                }
            } else {
                rec |= REC_ERR;
            }
        } else {
            rec |= REC_ERR;
        }
    }
    if (alg == 0 || opm == 0 && (mod != 'N' && mod != 'O')) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr,
                "Description: Encryption/Decryption Tool\n"
                "Usage: %s (-1 KEY | -2 ~ -4 KEY | -5 ~ -7 KEY | -8 ~ -0 KEY)\n"
                "       [-e | -d] [-E | -N IV | -O IV | -C IV | -H IV] [-i INFILE] [-o OUTFILE]\n"
                "Options:\n"
                "  -1 KEY      SM4 (KEY: 128-bit key in hex)\n"
                "  -2 KEY      AES-128 (KEY: 128-bit key in hex)\n"
                "  -3 KEY      AES-192 (KEY: 192-bit key in hex)\n"
                "  -4 KEY      AES-256 (KEY: 256-bit key in hex)\n"
                "  -5 KEY      Twofish-128 (KEY: 128-bit key in hex)\n"
                "  -6 KEY      Twofish-192 (KEY: 192-bit key in hex)\n"
                "  -7 KEY      Twofish-256 (KEY: 256-bit key in hex)\n"
                "  -8 KEY      Serpent-128 (KEY: 128-bit key in hex)\n"
                "  -9 KEY      Serpent-192 (KEY: 192-bit key in hex)\n"
                "  -0 KEY      Serpent-256 (KEY: 256-bit key in hex)\n"
                "  -e          encryption\n"
                "  -d          decryption\n"
                "  -E          ECB mode (default)\n"
                "  -N IV       CTR mode (IV: 128-bit IV in hex)\n"
                "  -O IV       OFB mode (IV: 128-bit IV in hex)\n"
                "  -C IV       CFB mode (IV: 128-bit IV in hex)\n"
                "  -H IV       CBC mode (IV: 128-bit IV in hex)\n"
                "  -i INFILE   input file (default: stdin)\n"
                "  -o OUTFILE  output file (default: stdout)\n",
                argv[0]);
    } else {
        switch (alg) {
        case '1': process<SM4>(mod, opm, ifp, ofp, civ, key); break;
        case '2': process<AES128>(mod, opm, ifp, ofp, civ, key); break;
        case '3': process<AES192>(mod, opm, ifp, ofp, civ, key); break;
        case '4': process<AES256>(mod, opm, ifp, ofp, civ, key); break;
        case '5': process<Twofish128>(mod, opm, ifp, ofp, civ, key); break;
        case '6': process<Twofish192>(mod, opm, ifp, ofp, civ, key); break;
        case '7': process<Twofish256>(mod, opm, ifp, ofp, civ, key); break;
        case '8': process<Serpent128>(mod, opm, ifp, ofp, civ, key); break;
        case '9': process<Serpent192>(mod, opm, ifp, ofp, civ, key); break;
        case '0': process<Serpent256>(mod, opm, ifp, ofp, civ, key); break;
        }
    }
    if (key) {
        delete[] key;
    }
    if (civ) {
        delete[] civ;
    }
    if (ifp != stdin) {
        fclose(ifp);
    }
    if (ofp != stdout) {
        fclose(ofp);
    }
    return rec & REC_ERR;
}
