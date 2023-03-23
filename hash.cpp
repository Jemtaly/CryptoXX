#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash/md5.hpp"
#include "hash/sm3.hpp"
#include "hash/sha.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define BUFSIZE 1024
template <typename Hash>
void printh(char const *name, HashWrapper<Hash> const &hash) {
    uint8_t digest[DIG];
    hash.digest(digest);
    printf("%s: ", name);
    for (int i = 0; i < DIG; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
int main(int argc, char *argv[]) {
    HashWrapper<MD5> md5;
    HashWrapper<SM3> sm3;
    HashWrapper<SHA> sha;
    uint8_t buf[BUFSIZE];
    size_t read;
    while ((read = fread(buf, 1, BUFSIZE, stdin)) == BUFSIZE) {
        md5.update(buf, (uint8_t *)buf + BUFSIZE);
        sm3.update(buf, (uint8_t *)buf + BUFSIZE);
        sha.update(buf, (uint8_t *)buf + BUFSIZE);
    }
    md5.update(buf, (uint8_t *)buf + read);
    sm3.update(buf, (uint8_t *)buf + read);
    sha.update(buf, (uint8_t *)buf + read);
    printh("MD5", md5);
    printh("SM3", sm3);
    printh("SHA", sha);
    return 0;
}
