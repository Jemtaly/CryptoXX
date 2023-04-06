#include <stdio.h>
#include "block/aes.hpp"
#include "block/sm4.hpp"
#include "block/twofish.hpp"
#include "block/serpent.hpp"
#include "block/cbc.hpp"
#include "async/cfb.hpp"
#include "stream/ofb.hpp"
#include "stream/ctr.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define BUFSIZE 65536
#define REC_ERR 1
#define REC_OFP 2
#define REC_IFP 4
#define REC_SM4 8
#define AES_128 16
#define AES_256 32
#define AES_192 64
#define TWO_128 128
#define TWO_256 256
#define TWO_192 512
#define REC_ENC 1024
#define REC_DEC 2048
#define REC_ECB 4096
#define REC_CBC 8192
#define REC_CFB 16384
#define REC_OFB 32768
#define REC_CTR 65536
#define SER_128 131072
#define SER_192 262144
#define SER_256 524288
#define REC_ALG (REC_SM4 | AES_128 | AES_192 | AES_256 | TWO_128 | TWO_192 | TWO_256 | SER_128 | SER_192 | SER_256)
#define REC_MOD (REC_ECB | REC_CBC | REC_CFB | REC_OFB | REC_CTR)
#define REC_OPM (REC_ENC | REC_DEC)
template <typename StreamCipher, typename... Args>
void sc_xxc(FILE *ifp, FILE *ofp, Args &&...args) {
    StreamCipherCrypter<StreamCipher> scc(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    while (fwrite(buf, 1, scc.update(buf, buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp)) - (uint8_t *)buf, ofp) == BUFSIZE) {}
}
template <typename AsyncCipher, typename... Args>
void ac_enc(FILE *ifp, FILE *ofp, Args &&...args) {
    AsyncCipherEncrypter<AsyncCipher> scc(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    while (fwrite(buf, 1, scc.update(buf, buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp)) - (uint8_t *)buf, ofp) == BUFSIZE) {}
}
template <typename AsyncCipher, typename... Args>
void ac_dec(FILE *ifp, FILE *ofp, Args &&...args) {
    AsyncCipherDecrypter<AsyncCipher> scc(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    while (fwrite(buf, 1, scc.update(buf, buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp)) - (uint8_t *)buf, ofp) == BUFSIZE) {}
}
template <typename BlockCipher, typename... Args>
void bc_enc(FILE *ifp, FILE *ofp, Args &&...args) {
    BlockCipherEncrypter<BlockCipher> bcf(std::forward<Args>(args)...);
    uint8_t src[BUFSIZE], dst[BUFSIZE + BLK];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE) {
        fwrite(dst, 1, bcf.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, ofp);
    }
    fwrite(dst, 1, bcf.fflush(bcf.update(dst, src, (uint8_t *)src + read)) - (uint8_t *)dst, ofp);
}
template <typename BlockCipher, typename... Args>
void bc_dec(FILE *ifp, FILE *ofp, Args &&...args) {
    BlockCipherDecrypter<BlockCipher> bcf(std::forward<Args>(args)...);
    uint8_t src[BUFSIZE], dst[BUFSIZE];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE) {
        fwrite(dst, 1, bcf.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, ofp);
    }
    fwrite(dst, 1, bcf.fflush(bcf.update(dst, src, (uint8_t *)src + read)) - (uint8_t *)dst, ofp);
}
template <typename BlockCipher>
void process(int rec, FILE *ifp, FILE *ofp, uint8_t const *civ, uint8_t const *key) {
    if ((rec & REC_CTR) != 0) {
        sc_xxc<CTRMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_OFB) != 0) {
        sc_xxc<OFBMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_CFB) != 0 && (rec & REC_ENC) != 0) {
        ac_enc<CFBMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_CFB) != 0 && (rec & REC_DEC) != 0) {
        ac_dec<CFBMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_CBC) != 0 && (rec & REC_ENC) != 0) {
        bc_enc<CBCMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_CBC) != 0 && (rec & REC_DEC) != 0) {
        bc_dec<CBCMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_ENC) != 0) {
        bc_enc<BlockCipher>(ifp, ofp, key);
    } else if ((rec & REC_DEC) != 0) {
        bc_dec<BlockCipher>(ifp, ofp, key);
    }
}
bool hex2bin(size_t len, char const *hex, uint8_t *bin) {
    for (size_t i = 0; i < len * 2; ++i) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            (bin[i / 2] &= (i % 2 ? 0xf0 : 0x0f)) |= (hex[i] - '0') << (i % 2 ? 0 : 4);
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            (bin[i / 2] &= (i % 2 ? 0xf0 : 0x0f)) |= (hex[i] - 'a' + 10) << (i % 2 ? 0 : 4);
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            (bin[i / 2] &= (i % 2 ? 0xf0 : 0x0f)) |= (hex[i] - 'A' + 10) << (i % 2 ? 0 : 4);
        } else {
            return false;
        }
    }
    return hex[len * 2] == '\0';
}
int main(int argc, char *argv[]) {
    int rec = 0;
    FILE *ifp = stdin, *ofp = stdout;
    uint8_t *key = NULL, *civ = NULL;
    for (int i = 1; (rec & REC_ERR) == 0 && i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'E' && argv[i][2] == '\0') {
                if ((rec & REC_MOD) == 0) {
                    rec |= REC_ECB;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'N' && argv[i][2] == '\0') {
                if ((rec & REC_MOD) == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    rec |= REC_CTR;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'O' && argv[i][2] == '\0') {
                if ((rec & REC_MOD) == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    rec |= REC_OFB;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'C' && argv[i][2] == '\0') {
                if ((rec & REC_MOD) == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    rec |= REC_CFB;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'H' && argv[i][2] == '\0') {
                if ((rec & REC_MOD) == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    rec |= REC_CBC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'd' && argv[i][2] == '\0') {
                if ((rec & REC_OPM) == 0) {
                    rec |= REC_DEC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'e' && argv[i][2] == '\0') {
                if ((rec & REC_OPM) == 0) {
                    rec |= REC_ENC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= REC_SM4;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '2' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= AES_128;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '3' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    rec |= AES_192;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    rec |= AES_256;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '5' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= TWO_128;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '6' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    rec |= TWO_192;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '7' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    rec |= TWO_256;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= SER_128;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '9' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    rec |= SER_192;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '0' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    rec |= SER_256;
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
    if ((rec & REC_ALG) == 0 || (rec & REC_OPM) == 0 && (rec & (REC_OFB | REC_CTR)) == 0) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr, "Description: SM4/AES Encryption/Decryption Tool\n");
        fprintf(stderr, "Usage: %s (-1 KEY | -2 ~ -4 KEY | -5 ~ -7 KEY | -8 ~ -0 KEY)\n", argv[0]);
        fprintf(stderr, "       [-e | -d] [-E | -N IV | -O IV | -C IV | -H IV] [-i INFILE] [-o OUTFILE]\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -1 KEY      SM4 (KEY: 128-bit key in hex)\n");
        fprintf(stderr, "  -2 KEY      AES-128 (KEY: 128-bit key in hex)\n");
        fprintf(stderr, "  -3 KEY      AES-192 (KEY: 192-bit key in hex)\n");
        fprintf(stderr, "  -4 KEY      AES-256 (KEY: 256-bit key in hex)\n");
        fprintf(stderr, "  -5 KEY      Twofish-128 (KEY: 128-bit key in hex)\n");
        fprintf(stderr, "  -6 KEY      Twofish-192 (KEY: 192-bit key in hex)\n");
        fprintf(stderr, "  -7 KEY      Twofish-256 (KEY: 256-bit key in hex)\n");
        fprintf(stderr, "  -8 KEY      Serpent-128 (KEY: 128-bit key in hex)\n");
        fprintf(stderr, "  -9 KEY      Serpent-192 (KEY: 192-bit key in hex)\n");
        fprintf(stderr, "  -0 KEY      Serpent-256 (KEY: 256-bit key in hex)\n");
        fprintf(stderr, "  -e          encryption\n");
        fprintf(stderr, "  -d          decryption\n");
        fprintf(stderr, "  -E          ECB mode (default)\n");
        fprintf(stderr, "  -N IV       CTR mode (IV: 128-bit IV in hex)\n");
        fprintf(stderr, "  -O IV       OFB mode (IV: 128-bit IV in hex)\n");
        fprintf(stderr, "  -C IV       CFB mode (IV: 128-bit IV in hex)\n");
        fprintf(stderr, "  -H IV       CBC mode (IV: 128-bit IV in hex)\n");
        fprintf(stderr, "  -i INFILE   input file (default: stdin)\n");
        fprintf(stderr, "  -o OUTFILE  output file (default: stdout)\n");
    } else if ((rec & REC_SM4) != 0) {
        process<SM4>(rec, ifp, ofp, civ, key);
    } else if ((rec & AES_128) != 0) {
        process<AES128>(rec, ifp, ofp, civ, key);
    } else if ((rec & AES_192) != 0) {
        process<AES192>(rec, ifp, ofp, civ, key);
    } else if ((rec & AES_256) != 0) {
        process<AES256>(rec, ifp, ofp, civ, key);
    } else if ((rec & TWO_128) != 0) {
        process<Twofish128>(rec, ifp, ofp, civ, key);
    } else if ((rec & TWO_192) != 0) {
        process<Twofish192>(rec, ifp, ofp, civ, key);
    } else if ((rec & TWO_256) != 0) {
        process<Twofish256>(rec, ifp, ofp, civ, key);
    } else if ((rec & SER_128) != 0) {
        process<Serpent128>(rec, ifp, ofp, civ, key);
    } else if ((rec & SER_192) != 0) {
        process<Serpent192>(rec, ifp, ofp, civ, key);
    } else if ((rec & SER_256) != 0) {
        process<Serpent256>(rec, ifp, ofp, civ, key);
    }
    if (key) {
        delete[] key;
    }
    if (civ) {
        delete[] civ;
    }
    if ((rec & REC_IFP) != 0) {
        fclose(ifp);
    }
    if ((rec & REC_OFP) != 0) {
        fclose(ofp);
    }
    return rec & REC_ERR;
}
