#include <stdio.h>
#include <stdlib.h>
#include "hash/md5.hpp"
#include "hash/sm3.hpp"
#include "hash/sha.hpp"
#include "hash/crc.hpp"
#include "hash/sha256.hpp"
#include "hash/sha512.hpp"
#include "hash/keccak.hpp"
#include "hash/blake2s.hpp"
#include "hash/blake2b.hpp"
#include "hash/blake3.hpp"
#include "hash/whirlpool.hpp"
#include "hash/hmac.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define BUFSIZE 65536
#define REC_224 1
#define REC_256 2
#define REC_384 4
#define REC_512 8
#define RE3_224 16
#define RE3_256 32
#define RE3_384 64
#define RE3_512 128
#define REC_C32 256
#define REC_C64 512
#define REC_BLs 1024
#define REC_BLb 2048
#define REC_BL3 4096
#define REC_WHP 8192
#define REC_MD5 16384
#define REC_SHA 32768
#define REC_SM3 65536
#define REC_ALG (REC_224 | REC_256 | REC_384 | REC_512 | RE3_224 | RE3_256 | RE3_384 | RE3_512 | REC_C32 | REC_C64 | REC_BLs | REC_BLb | REC_MD5 | REC_SHA | REC_SM3 | REC_BL3 | REC_WHP)
#define REC_IFP 131072
#define REC_MAC 262144
#define REC_ERR 524288
template <typename Hash, typename... Args>
void hash(FILE* file, Args &&...args) {
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
void process(int rec, FILE* file, uint8_t const *key, size_t len) {
    if ((rec & REC_MAC) == 0) {
        hash<Hash>(file);
    } else {
        hash<HMAC<Hash>>(file, key, len);
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
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_MD5;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'X' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_SHA;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_SM3;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '0' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_C32;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '1' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_C64;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '2' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_224;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '3' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_256;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_384;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '5' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_512;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '6' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= RE3_224;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '7' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= RE3_256;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= RE3_384;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '9' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= RE3_512;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'b' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_BLb;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 's' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_BLs;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'B' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_BL3;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'W' && argv[i][2] == '\0') {
                if ((rec & REC_ALG) == 0) {
                    rec |= REC_WHP;
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
    if ((rec & REC_ALG) == 0 || (rec & REC_MAC) != 0 && (rec & (REC_C32 | REC_C64 | REC_WHP)) != 0) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr, "Description: HMAC/Hash Calculator\n");
        fprintf(stderr, "Usage: %s [FILE] (-0 | -1 | -W)\n", argv[0]);
        fprintf(stderr, "       %s [FILE] (-2 ~ -9 | -M | -X | -S | -b | -s | -B) [-H LEN KEY]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  FILE        input file (default: stdin)\n");
        fprintf(stderr, "  -H LEN KEY  HMAC (LEN: key byte length, KEY: key in hex)\n");
        fprintf(stderr, "  -0          CRC-32\n");
        fprintf(stderr, "  -1          CRC-64\n");
        fprintf(stderr, "  -M          MD5\n");
        fprintf(stderr, "  -X          SHA-1\n");
        fprintf(stderr, "  -S          SM3\n");
        fprintf(stderr, "  -2,  -3     SHA-224, SHA-256\n");
        fprintf(stderr, "  -4,  -5     SHA-384, SHA-512\n");
        fprintf(stderr, "  -6,  -7     SHA3-224, SHA3-256\n");
        fprintf(stderr, "  -8,  -9     SHA3-384, SHA3-512\n");
        fprintf(stderr, "  -s          BLAKE2s\n");
        fprintf(stderr, "  -b          BLAKE2b\n");
        fprintf(stderr, "  -3          BLAKE3\n");
        fprintf(stderr, "  -W          Whirlpool\n");
    } else if ((rec & REC_C32) != 0) {
        hash<CRC32>(fp);
    } else if ((rec & REC_C64) != 0) {
        hash<CRC64>(fp);
    } else if ((rec & REC_WHP) != 0) {
        hash<Whirlpool>(fp);
    } else if ((rec & REC_MD5) != 0) {
        process<MD5>(rec, fp, key, len);
    } else if ((rec & REC_SHA) != 0) {
        process<SHA>(rec, fp, key, len);
    } else if ((rec & REC_SM3) != 0) {
        process<SM3>(rec, fp, key, len);
    } else if ((rec & REC_224) != 0) {
        process<SHA224>(rec, fp, key, len);
    } else if ((rec & REC_256) != 0) {
        process<SHA256>(rec, fp, key, len);
    } else if ((rec & REC_384) != 0) {
        process<SHA384>(rec, fp, key, len);
    } else if ((rec & REC_512) != 0) {
        process<SHA512>(rec, fp, key, len);
    } else if ((rec & RE3_224) != 0) {
        process<SHA3<224>>(rec, fp, key, len);
    } else if ((rec & RE3_256) != 0) {
        process<SHA3<256>>(rec, fp, key, len);
    } else if ((rec & RE3_384) != 0) {
        process<SHA3<384>>(rec, fp, key, len);
    } else if ((rec & RE3_512) != 0) {
        process<SHA3<512>>(rec, fp, key, len);
    } else if ((rec & REC_BLs) != 0) {
        process<BLAKE2s256>(rec, fp, key, len);
    } else if ((rec & REC_BLb) != 0) {
        process<BLAKE2b512>(rec, fp, key, len);
    } else if ((rec & REC_BL3) != 0) {
        process<BLAKE3>(rec, fp, key, len);
    }
    if (key) {
        delete[] key;
    }
    if (fp != stdin) {
        fclose(fp);
    }
    return rec & REC_ERR ? 1 : 0;
}
