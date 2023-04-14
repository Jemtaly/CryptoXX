#pragma once
#include "hash.hpp"
#define QROUND(v, m, S, i, a, b, c, d, x, y) {                      \
    v[a] += v[b] + m[S[i][x]]; v[d] ^= v[a]; v[d] = ROTR(v[d], 32); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b], 24); \
    v[a] += v[b] + m[S[i][y]]; v[d] ^= v[a]; v[d] = ROTR(v[d], 16); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b], 63); \
}
#define DROUND(v, m, S, i) {                    \
    QROUND(v, m, S, i,  0,  4,  8, 12,  0,  1); \
    QROUND(v, m, S, i,  1,  5,  9, 13,  2,  3); \
    QROUND(v, m, S, i,  2,  6, 10, 14,  4,  5); \
    QROUND(v, m, S, i,  3,  7, 11, 15,  6,  7); \
    QROUND(v, m, S, i,  0,  5, 10, 15,  8,  9); \
    QROUND(v, m, S, i,  1,  6, 11, 12, 10, 11); \
    QROUND(v, m, S, i,  2,  7,  8, 13, 12, 13); \
    QROUND(v, m, S, i,  3,  4,  9, 14, 14, 15); \
}
typedef uint8_t index_t;
class BLAKE2bBase {
protected:
    static constexpr index_t SIGMA[10][16] = {
        { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
        {14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3},
        {11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4},
        { 7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8},
        { 9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13},
        { 2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9},
        {12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11},
        {13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10},
        { 6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5},
        {10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0},
    };
};
template <size_t DN, typename Derived>
class BLAKE2bTmpl: public BLAKE2bBase {
    void compress(uint64_t const *m, bool fin) {
        uint64_t v[16] = {
            h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7],
            Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
            Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
        };
        v[12] ^= lo;
        v[13] ^= hi;
        v[14] = fin ? ~v[14] : v[14];
        DROUND(v, m, SIGMA, 0);
        DROUND(v, m, SIGMA, 1);
        DROUND(v, m, SIGMA, 2);
        DROUND(v, m, SIGMA, 3);
        DROUND(v, m, SIGMA, 4);
        DROUND(v, m, SIGMA, 5);
        DROUND(v, m, SIGMA, 6);
        DROUND(v, m, SIGMA, 7);
        DROUND(v, m, SIGMA, 8);
        DROUND(v, m, SIGMA, 9);
        DROUND(v, m, SIGMA, 0);
        DROUND(v, m, SIGMA, 1);
        h[ 0] ^= v[ 0] ^ v[ 8];
        h[ 1] ^= v[ 1] ^ v[ 9];
        h[ 2] ^= v[ 2] ^ v[10];
        h[ 3] ^= v[ 3] ^ v[11];
        h[ 4] ^= v[ 4] ^ v[12];
        h[ 5] ^= v[ 5] ^ v[13];
        h[ 6] ^= v[ 6] ^ v[14];
        h[ 7] ^= v[ 7] ^ v[15];
    }
    uint64_t hi = 0;
    uint64_t lo = 0;
    uint64_t h[8] = {
        Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
        Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
    };
public:
    static constexpr size_t BLOCK_SIZE = 128;
    static constexpr size_t DIGEST_SIZE = DN;
    static constexpr bool NO_PADDING = true;
    BLAKE2bTmpl(uint8_t const *key, size_t len) {
        h[0] ^= 0x01010000 ^ len << 8 ^ DN;
        if (len > 0) {
            uint64_t m[16] = {};
            READB_LE(m, key, len);
            lo += 128;
            lo >= 128 || ++hi;
            compress(m, 0);
        }
    }
    BLAKE2bTmpl(): BLAKE2bTmpl(nullptr, 0) {}
    void push(uint8_t const *blk) {
        uint64_t m[16] = {};
        READB_LE(m, blk, 128);
        lo += 128;
        lo >= 128 || ++hi;
        compress(m, 0);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dig) {
        uint64_t m[16] = {};
        READB_LE(m, src, len);
        lo += len;
        lo >= len || ++hi;
        compress(m, 1);
        WRITEB_LE(dig, h, DN);
    }
};
class BLAKE2b512: public BLAKE2bTmpl<64, BLAKE2b512> {
public:
    static constexpr uint64_t IV[8] = {
        0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
        0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179,
    };
};
class BLAKE2b384: public BLAKE2bTmpl<48, BLAKE2b384> {
public:
    static constexpr uint64_t IV[8] = {
        0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
        0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4,
    };
};
#undef DROUND
#undef QROUND
