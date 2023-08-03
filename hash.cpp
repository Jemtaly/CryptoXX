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
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define BUFSIZE 65536
#define REC_ERR 1
#define REC_IFP 2
#define REC_MAC 4
template <typename Hash, typename... Args>
void hash(FILE *file, Args &&...args) {
    HashWrapper<Hash> hash(std::forward<Args>(args)...);
    uint8_t buf[BUFSIZE];
    size_t read;
    while ((read = fread(buf, 1, BUFSIZE, file)) == BUFSIZE) {
        hash.update(buf, (uint8_t *)buf + BUFSIZE);
    }
    hash.update(buf, (uint8_t *)buf + read);
    uint8_t digest[DIG];
    hash.digest(digest);
    for (int i = 0; i < DIG; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
template <typename Hash>
void process(int rec, FILE *file, uint8_t const *key, size_t len) {
    if ((rec & REC_MAC) == 0) {
        hash<Hash>(file);
    } else {
        hash<HMAC<Hash>>(file, key, len);
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
    FILE *fp = stdin;
    uint8_t *key = NULL;
    size_t len = 0;
    for (int i = 1; (rec & REC_ERR) == 0 && i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'H' && argv[i][2] == '\0') {
                if ((rec & REC_MAC) == 0 && i + 2 < argc && (len = atoll(argv[++i]), hex2bin(len, argv[++i], key = new uint8_t[len]))) {
                    rec |= REC_MAC;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'M' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'M';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'X' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'X';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'S';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '0' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '0';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '1' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '1';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '2' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '2';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '3' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '3';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '4';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '5' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '5';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '6' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '6';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '7' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '7';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '8';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '9' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = '9';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'b' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'b';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 's' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 's';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'B' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'B';
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'W' && argv[i][2] == '\0') {
                if (alg == 0) {
                    alg = 'W';
                } else {
                    rec |= REC_ERR;
                }
            } else {
                rec |= REC_ERR;
            }
        } else if ((rec & REC_IFP) == 0 && (fp = fopen(argv[i], "rb"))) {
            rec |= REC_IFP;
        } else {
            rec |= REC_ERR;
        }
    }
    if (alg == 0 || (rec & REC_MAC) != 0 && (alg == '0' || alg == '1' || alg == 'W')) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr,
                "Description: HMAC/Hash Calculator\n"
                "Usage: %s [FILE] (-0 | -1 | -W)\n"
                "       %s [FILE] (-2 ~ -9 | -M | -X | -S | -b | -s | -B) [-H LEN KEY]\n"
                "Options:\n"
                "  FILE        input file (default: stdin)\n"
                "  -H LEN KEY  HMAC (LEN: key byte length, KEY: key in hex)\n"
                "  -0          CRC-32\n"
                "  -1          CRC-64\n"
                "  -M          MD5\n"
                "  -X          SHA-1\n"
                "  -S          SM3\n"
                "  -2,  -3     SHA-224, SHA-256\n"
                "  -4,  -5     SHA-384, SHA-512\n"
                "  -6,  -7     SHA3-224, SHA3-256\n"
                "  -8,  -9     SHA3-384, SHA3-512\n"
                "  -s          BLAKE2s\n"
                "  -b          BLAKE2b\n"
                "  -3          BLAKE3\n"
                "  -W          Whirlpool\n",
                argv[0], argv[0]);
    } else {
        switch (alg) {
        case '0': process<CRC32>(rec, fp, key, len); break;
        case '1': process<CRC64>(rec, fp, key, len); break;
        case 'M': process<MD5>(rec, fp, key, len); break;
        case 'X': process<SHA>(rec, fp, key, len); break;
        case 'S': process<SM3>(rec, fp, key, len); break;
        case '2': process<SHA224>(rec, fp, key, len); break;
        case '3': process<SHA256>(rec, fp, key, len); break;
        case '4': process<SHA384>(rec, fp, key, len); break;
        case '5': process<SHA512>(rec, fp, key, len); break;
        case '6': process<SHA3<224>>(rec, fp, key, len); break;
        case '7': process<SHA3<256>>(rec, fp, key, len); break;
        case '8': process<SHA3<384>>(rec, fp, key, len); break;
        case '9': process<SHA3<512>>(rec, fp, key, len); break;
        case 's': process<BLAKE2s256>(rec, fp, key, len); break;
        case 'b': process<BLAKE2b512>(rec, fp, key, len); break;
        case 'B': process<BLAKE3>(rec, fp, key, len); break;
        case 'W': process<Whirlpool>(rec, fp, key, len); break;
        }
    }
    if (key) {
        delete[] key;
    }
    if (fp != stdin) {
        fclose(fp);
    }
    return rec & REC_ERR;
}
