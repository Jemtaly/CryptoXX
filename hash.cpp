#include <stdio.h>
#include "hash/md5.hpp"
#include "hash/sm3.hpp"
#include "hash/sha.hpp"
#include "hash/sha256.hpp"
#include "hash/sha512.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define BUFSIZE 1024
#define REC_224 1
#define REC_256 2
#define REC_384 4
#define REC_512 8
#define REC_MD5 16
#define REC_SHA 32
#define REC_SM3 64
#define REC_IFP 128
#define REC_ERR 256
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
int main(int argc, char *argv[]) {
    int rec = 0;
    FILE *fp = stdin;
    for (int i = 1; (rec & REC_ERR) == 0 && i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'i' && argv[i][2] == '\0') {
                if ((rec & REC_IFP) == 0 && i + 1 < argc && (fp = fopen(argv[++i], "rb"))) {
                    rec |= REC_IFP;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'M' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_MD5;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'X' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_SHA;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == 'S' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_SM3;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '7' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_224;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '8' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_256;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '3' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_384;
                } else {
                    rec |= REC_ERR;
                }
            } else if (argv[i][1] == '4' && argv[i][2] == '\0') {
                if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
                    rec |= REC_512;
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
    if ((rec & (REC_MD5 | REC_SHA | REC_SM3 | REC_224 | REC_256 | REC_384 | REC_512)) == 0) {
        rec |= REC_ERR;
    }
    if ((rec & REC_ERR) != 0) {
        fprintf(stderr, "usage: %s [-i INFILE] (-M | -X | -S | -7 | -8 | -3 | -4)\n", argv[0]);
    } else if ((rec & REC_MD5) != 0) {
        hash<MD5>(fp);
    } else if ((rec & REC_SHA) != 0) {
        hash<SHA>(fp);
    } else if ((rec & REC_SM3) != 0) {
        hash<SM3>(fp);
    } else if ((rec & REC_224) != 0) {
        hash<SHA224>(fp);
    } else if ((rec & REC_256) != 0) {
        hash<SHA256>(fp);
    } else if ((rec & REC_384) != 0) {
        hash<SHA384>(fp);
    } else if ((rec & REC_512) != 0) {
        hash<SHA512>(fp);
    }
    return 0;
}
