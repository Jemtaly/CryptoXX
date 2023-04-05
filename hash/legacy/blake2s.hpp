#pragma once
#include "hash.hpp"
#define QROUND(v, m, S, i, a, b, c, d, x, y) {                      \
    v[a] += v[b] + m[S[i][x]]; v[d] ^= v[a]; v[d] = ROTR(v[d], 16); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b], 12); \
    v[a] += v[b] + m[S[i][y]]; v[d] ^= v[a]; v[d] = ROTR(v[d],  8); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b],  7); \
}
typedef uint8_t bits_t;
class BLAKE2sBase {
protected:
    static constexpr bits_t SIGMA[10][16] = {
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
template <size_t DS, typename Derived>
class BLAKE2sTmpl: public BLAKE2sBase {
    uint32_t hi = 0;
    uint32_t lo = 0;
    uint32_t h[8] = {
        Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
        Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
    };
    void compress(uint8_t const *blk, bool fin) {
        uint32_t sta[16] = {
            h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7],
            Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
            Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
        };
        uint32_t m[16];
        READ_LE(m, blk, 16);
        sta[12] ^= lo;
        sta[13] ^= hi;
        sta[14] = fin ? ~sta[14] : sta[14];
        for (int i = 0; i < 10; ++i) {
            QROUND(sta, m, SIGMA, i,  0,  4,  8, 12,  0,  1);
            QROUND(sta, m, SIGMA, i,  1,  5,  9, 13,  2,  3);
            QROUND(sta, m, SIGMA, i,  2,  6, 10, 14,  4,  5);
            QROUND(sta, m, SIGMA, i,  3,  7, 11, 15,  6,  7);
            QROUND(sta, m, SIGMA, i,  0,  5, 10, 15,  8,  9);
            QROUND(sta, m, SIGMA, i,  1,  6, 11, 12, 10, 11);
            QROUND(sta, m, SIGMA, i,  2,  7,  8, 13, 12, 13);
            QROUND(sta, m, SIGMA, i,  3,  4,  9, 14, 14, 15);
        }
        h[0] ^= sta[0] ^ sta[ 8];
        h[1] ^= sta[1] ^ sta[ 9];
        h[2] ^= sta[2] ^ sta[10];
        h[3] ^= sta[3] ^ sta[11];
        h[4] ^= sta[4] ^ sta[12];
        h[5] ^= sta[5] ^ sta[13];
        h[6] ^= sta[6] ^ sta[14];
        h[7] ^= sta[7] ^ sta[15];
    }
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = DS;
    static constexpr bool NO_PADDING = true;
    BLAKE2sTmpl(uint8_t const *key, size_t len) {
        h[0] ^= 0x01010000 ^ len << 8 ^ DS;
        if (len > 0) {
            uint8_t tmp[64] = {};
            memcpy(tmp, key, len);
            lo += 64;
            lo >= 64 || ++hi;
            compress(tmp, 0);
        }
    }
    BLAKE2sTmpl(): BLAKE2sTmpl(nullptr, 0) {}
    void push(uint8_t const *blk) {
        lo += 64;
        lo >= 64 || ++hi;
        compress(blk, 0);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dig) {
        uint8_t tmp[64] = {};
        memcpy(tmp, src, len);
        lo += len;
        lo >= len || ++hi;
        compress(tmp, 1);
        WRITE_LE(dig, h, DS / 4);
    }
};
class BLAKE2s256: public BLAKE2sTmpl<32, BLAKE2s256> {
public:
    static constexpr uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
    };
};
class BLAKE2s224: public BLAKE2sTmpl<28, BLAKE2s224> {
public:
    static constexpr uint32_t IV[8] = {
        0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
        0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4,
    };
};
#undef QROUND
