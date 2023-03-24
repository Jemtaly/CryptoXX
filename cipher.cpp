#include <stdio.h>
#include "block/aes.hpp"
#include "block/sm4.hpp"
#include "stream/ctr.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define BUFSIZE 65536
#define REC_ERR 1
#define REC_OFP 2
#define REC_IFP 4
#define REC_SM4 8
#define REC_128 16
#define REC_256 32
#define REC_192 64
#define REC_ENC 128
#define REC_DEC 256
#define REC_CTR 512
template <typename StreamCipher, typename... Args>
void XXC(FILE *ifp, FILE *ofp, Args &&...args) {
    StreamCipherCrypter<StreamCipher> scc(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    while (fwrite(buf, 1, scc.update(buf, buf, (uint8_t *)buf + fread(buf, 1, BUFSIZE, ifp)) - (uint8_t *)buf, ofp) == BUFSIZE) {}
}
template <typename BlockCipher, typename... Args>
void ENC(FILE *ifp, FILE *ofp, Args &&...args) {
    BlockCipherEncrypter<BlockCipher> bcf(std::forward<Args>(args)...);
    uint8_t src[BUFSIZE], dst[BUFSIZE + BLK];
    size_t read;
    while ((read = fread(src, 1, BUFSIZE, ifp)) == BUFSIZE) {
        fwrite(dst, 1, bcf.update(dst, src, (uint8_t *)src + BUFSIZE) - (uint8_t *)dst, ofp);
    }
    fwrite(dst, 1, bcf.fflush(bcf.update(dst, src, (uint8_t *)src + read)) - (uint8_t *)dst, ofp);
}
template <typename BlockCipher, typename... Args>
void DEC(FILE *ifp, FILE *ofp, Args &&...args) {
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
        XXC<CTRMode<BlockCipher>>(ifp, ofp, civ, key);
    } else if ((rec & REC_ENC) != 0) {
        ENC<BlockCipher>(ifp, ofp, key);
    } else if ((rec & REC_DEC) != 0) {
        DEC<BlockCipher>(ifp, ofp, key);
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
            if (argv[i][1] == 'c' && argv[i][2] == '\0') {
                if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 && i + 1 < argc && hex2bin(16, argv[++i], civ = new uint8_t[16])) {
                    rec |= REC_CTR;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'd' && argv[i][2] == '\0') {
                if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0) {
                    rec |= REC_DEC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'e' && argv[i][2] == '\0') {
                if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0) {
                    rec |= REC_ENC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
                if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= REC_SM4;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0 && i + 1 < argc && hex2bin(16, argv[++i], key = new uint8_t[16])) {
                    rec |= REC_128;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '6' && argv[i][2] == '\0') {
                if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0 && i + 1 < argc && hex2bin(24, argv[++i], key = new uint8_t[24])) {
                    rec |= REC_192;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if ((rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0 && i + 1 < argc && hex2bin(32, argv[++i], key = new uint8_t[32])) {
                    rec |= REC_256;
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
    if ((rec & (REC_CTR | REC_DEC | REC_ENC)) == 0 || (rec & (REC_SM4 | REC_128 | REC_192 | REC_256)) == 0) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr, "usage: %s [-i INFILE] [-o OUTFILE] (-c IV | -e | -d) (-S KEY | -4 KEY | -6 KEY | -8 KEY)\n", argv[0]);
    } else if ((rec & REC_SM4) != 0) {
        process<SM4>(rec, ifp, ofp, civ, key);
    } else if ((rec & REC_128) != 0) {
        process<AES128>(rec, ifp, ofp, civ, key);
    } else if ((rec & REC_192) != 0) {
        process<AES192>(rec, ifp, ofp, civ, key);
    } else if ((rec & REC_256) != 0) {
        process<AES256>(rec, ifp, ofp, civ, key);
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