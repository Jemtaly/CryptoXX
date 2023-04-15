#pragma once
#include <array>
#include "hash.hpp"
#define CHUNK_START 0x01
#define CHUNK_END   0x02
#define PARENT      0x04
#define ROOT        0x08
#define QROUND(v, m, S, a, b, c, d, x, y) {                      \
    v[a] += v[b] + m[S[x]]; v[d] ^= v[a]; v[d] = ROTR(v[d], 16); \
    v[c] += v[d]          ; v[b] ^= v[c]; v[b] = ROTR(v[b], 12); \
    v[a] += v[b] + m[S[y]]; v[d] ^= v[a]; v[d] = ROTR(v[d],  8); \
    v[c] += v[d]          ; v[b] ^= v[c]; v[b] = ROTR(v[b],  7); \
}
#define DROUND(v, m, S) {                    \
    QROUND(v, m, S,  0,  4,  8, 12,  0,  1); \
    QROUND(v, m, S,  1,  5,  9, 13,  2,  3); \
    QROUND(v, m, S,  2,  6, 10, 14,  4,  5); \
    QROUND(v, m, S,  3,  7, 11, 15,  6,  7); \
    QROUND(v, m, S,  0,  5, 10, 15,  8,  9); \
    QROUND(v, m, S,  1,  6, 11, 12, 10, 11); \
    QROUND(v, m, S,  2,  7,  8, 13, 12, 13); \
    QROUND(v, m, S,  3,  4,  9, 14, 14, 15); \
}
typedef uint8_t index_t;
struct BLAKE3Compressor {
    static constexpr uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
    };
    static constexpr index_t PERM[16] = {
        2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8,
    };
    static constexpr auto SIGMA = []() {
        std::array<std::array<index_t, 16>, 7> SIGMA = {};
        for (int i = 0; i < 16; i++) {
            SIGMA[0][i] = i;
        }
        for (int i = 1; i < 7; i++) {
            for (int j = 0; j < 16; j++) {
                SIGMA[i][j] = SIGMA[i - 1][PERM[j]];
            }
        }
        return SIGMA;
    }();
    uint32_t hi;
    uint32_t lo;
    uint32_t ln;
    uint32_t fl;
    uint32_t const *i;
    uint32_t       *o;
    uint32_t const *m;
    void operate() {
        uint32_t v[16] = {
            i [0], i [1], i [2], i [3], i [4], i [5], i [6], i [7],
            IV[0], IV[1], IV[2], IV[3], lo   , hi   , ln   , fl   ,
        };
        DROUND(v, m, SIGMA[0]);
        DROUND(v, m, SIGMA[1]);
        DROUND(v, m, SIGMA[2]);
        DROUND(v, m, SIGMA[3]);
        DROUND(v, m, SIGMA[4]);
        DROUND(v, m, SIGMA[5]);
        DROUND(v, m, SIGMA[6]);
        o[ 0] = v[ 0] ^= v[ 8];
        o[ 1] = v[ 1] ^= v[ 9];
        o[ 2] = v[ 2] ^= v[10];
        o[ 3] = v[ 3] ^= v[11];
        o[ 4] = v[ 4] ^= v[12];
        o[ 5] = v[ 5] ^= v[13];
        o[ 6] = v[ 6] ^= v[14];
        o[ 7] = v[ 7] ^= v[15];
    }
};
class BLAKE3 {
    uint32_t const iv[8] = {
        BLAKE3Compressor::IV[0], BLAKE3Compressor::IV[1],
        BLAKE3Compressor::IV[2], BLAKE3Compressor::IV[3],
        BLAKE3Compressor::IV[4], BLAKE3Compressor::IV[5],
        BLAKE3Compressor::IV[6], BLAKE3Compressor::IV[7],
    }; // key
    uint64_t hl = 0;
    uint32_t stack[64][8];
    uint8_t top = 0;
    uint8_t idx = 0;
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 32;
    static constexpr bool NO_PADDING = true;
    void push(uint8_t const *blk) {
        uint32_t m[16] = {};
        READB_LE(m, blk, 64);
        BLAKE3Compressor comp;
        comp.lo = hl       & 0xFFFFFFFF;
        comp.hi = hl >> 32 & 0xFFFFFFFF;
        comp.ln = 64;
        comp.fl = idx ? 0 : CHUNK_START;
        comp.i  = idx ? stack[top] : iv;
        comp.o  = stack[top];
        comp.m  = m;
        if ((++idx %= 16) == 0) {
            comp.fl |= CHUNK_END;
            for (uint64_t c = hl; c & 0x1; c >>= 1) {
                comp.operate();
                top--; // pop
                comp.lo = 0;
                comp.hi = 0;
                comp.ln = 64;
                comp.fl = PARENT;
                comp.i  = iv;
                comp.o  = stack[top];
                comp.m  = stack[top];
            }
            hl++; // increment counter
            top++; // push
        }
        comp.operate();
    }
    void hash(uint8_t const *blk, size_t len, uint8_t *dig) {
        uint32_t m[16] = {};
        READB_LE(m, blk, len);
        BLAKE3Compressor comp;
        comp.lo = hl       & 0xFFFFFFFF;
        comp.hi = hl >> 32 & 0xFFFFFFFF;
        comp.ln = len;
        comp.fl = idx ? 0 : CHUNK_START;
        comp.i  = idx ? stack[top] : iv;
        comp.o  = stack[top];
        comp.m  = m;
        comp.fl |= CHUNK_END;
        while (top > 0) {
            comp.operate();
            top--; // pop
            comp.lo = 0;
            comp.hi = 0;
            comp.ln = 64;
            comp.fl = PARENT;
            comp.i  = iv;
            comp.o  = stack[top];
            comp.m  = stack[top];
        }
        comp.fl |= ROOT;
        comp.operate();
        WRITEB_LE(dig, stack[top], 32);
    }
};
