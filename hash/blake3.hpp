#pragma once
#include <array>
#include "hash.hpp"
#define CHUNK_START 0x01
#define CHUNK_END   0x02
#define PARENT      0x04
#define ROOT        0x08
#define QROUND(v, m, S, i, a, b, c, d, x, y) {                      \
    v[a] += v[b] + m[S[i][x]]; v[d] ^= v[a]; v[d] = ROTR(v[d], 16); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b], 12); \
    v[a] += v[b] + m[S[i][y]]; v[d] ^= v[a]; v[d] = ROTR(v[d],  8); \
    v[c] += v[d]             ; v[b] ^= v[c]; v[b] = ROTR(v[b],  7); \
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
class BLAKE3 {
    static constexpr uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
    };
    static constexpr uint32_t iv[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
    }; // key
    static constexpr uint8_t PM[16] = {
        2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8,
    };
    static constexpr auto SIGMA = []() {
        std::array<std::array<uint8_t, 16>, 7> SIGMA = {};
        for (int i = 0; i < 16; i++) {
            SIGMA[0][i] = i;
        }
        for (int i = 1; i < 7; i++) {
            for (int j = 0; j < 16; j++) {
                SIGMA[i][j] = SIGMA[i - 1][PM[j]];
            }
        }
        return SIGMA;
    }();
    static void compress(uint32_t const *m, uint32_t *o, uint32_t const *i, uint32_t hi, uint32_t lo, uint8_t ln, uint8_t fl) {
        uint32_t v[16] = {
            i [0], i [1], i [2], i [3], i [4], i [5], i [6], i [7],
            IV[0], IV[1], IV[2], IV[3], lo   , hi   , ln   , fl   ,
        };
        DROUND(v, m, SIGMA, 0);
        DROUND(v, m, SIGMA, 1);
        DROUND(v, m, SIGMA, 2);
        DROUND(v, m, SIGMA, 3);
        DROUND(v, m, SIGMA, 4);
        DROUND(v, m, SIGMA, 5);
        DROUND(v, m, SIGMA, 6);
        o[ 0] = v[ 0] ^= v[ 8];
        o[ 1] = v[ 1] ^= v[ 9];
        o[ 2] = v[ 2] ^= v[10];
        o[ 3] = v[ 3] ^= v[11];
        o[ 4] = v[ 4] ^= v[12];
        o[ 5] = v[ 5] ^= v[13];
        o[ 6] = v[ 6] ^= v[14];
        o[ 7] = v[ 7] ^= v[15];
    }
    uint32_t stack[64][8];
    uint8_t top = 0;
    uint8_t ctr = 0;
    uint64_t hl = 0;
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 32;
    static constexpr bool NO_PADDING = true;
    void push(uint8_t const *blk) {
        uint32_t m[16] = {};
        READB_LE(m, blk, 64);
        uint32_t       *dst = stack[top];
        uint32_t const *src = stack[top];
        uint32_t const *msg = m;
        uint32_t lo = hl       & 0xFFFFFFFF;
        uint32_t hi = hl >> 32 & 0xFFFFFFFF;
        uint32_t ln = 64;
        uint32_t fl = 0;
        if (ctr == 0) { // init chunk
            src = iv;
            fl |= CHUNK_START;
        }
        ctr = (ctr + 1) % 16;
        if (ctr == 0) { // end of chunk
            fl |= CHUNK_END;
            for (uint64_t c = hl; c & 0x1; c >>= 1) {
                compress(msg, dst, src, hi, lo, ln, fl);
                top--; // pop
                src = iv;
                dst = stack[top];
                msg = stack[top];
                lo = 0;
                hi = 0;
                ln = 64;
                fl = PARENT;
            }
            top++; // push
            hl++; // increment counter
        }
        compress(msg, dst, src, hi, lo, ln, fl);
    }
    void hash(uint8_t const *blk, size_t len, uint8_t *dig) {
        uint32_t m[16] = {};
        READB_LE(m, blk, len);
        uint32_t       *dst = stack[top];
        uint32_t const *src = stack[top];
        uint32_t const *msg = m;
        uint32_t lo = hl       & 0xFFFFFFFF;
        uint32_t hi = hl >> 32 & 0xFFFFFFFF;
        uint32_t ln = len;
        uint32_t fl = CHUNK_END;
        if (ctr == 0) { // init chunk
            src = iv;
            fl |= CHUNK_START;
        }
        while (top > 0) {
            compress(msg, dst, src, hi, lo, ln, fl);
            top--; // pop
            src = iv;
            dst = stack[top];
            msg = stack[top];
            lo = 0;
            hi = 0;
            ln = 64;
            fl = PARENT;
        }
        compress(msg, dst, src, hi, lo, ln, fl | ROOT);
        WRITEB_LE(dig, dst, 32);
    }
};
