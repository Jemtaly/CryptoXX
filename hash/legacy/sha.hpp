#pragma once
#include "hash.hpp"
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((x) ^ (y) ^ (z))
#define FF2(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF3(x, y, z) ((x) ^ (y) ^ (z))
#define KK0 0x5A827999U
#define KK1 0x6ED9EBA1U
#define KK2 0x8F1BBCDCU
#define KK3 0xCA62C1D6U
#define GGN(N, a, b, c, d, e, w, t, X, Y)                    \
    for (int i = X; i < Y; i++) {                            \
        t = ROTL(a,  5) + FF##N(b, c, d) + e + KK##N + w[i]; \
        e = d;                                               \
        d = c;                                               \
        c = ROTL(b, 30);                                     \
        b = a;                                               \
        a = t;                                               \
    }
class SHA {
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t h[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
    };
    void compress(uint8_t const *blk) {
        uint32_t a = h[0];
        uint32_t b = h[1];
        uint32_t c = h[2];
        uint32_t d = h[3];
        uint32_t e = h[4];
        uint32_t w[80], t;
        READ_BE(w, blk, 16);
        for (int i = 16; i < 80; i++) {
            t = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
            w[i] = ROTL(t, 1);
        }
        GGN(0, a, b, c, d, e, w, t,  0, 20);
        GGN(1, a, b, c, d, e, w, t, 20, 40);
        GGN(2, a, b, c, d, e, w, t, 40, 60);
        GGN(3, a, b, c, d, e, w, t, 60, 80);
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
    }
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 20;
    static constexpr bool NO_PADDING = false;
    void push(uint8_t const *blk) {
        lo += 512;
        lo >= 512 || hi++;
        compress(blk);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) {
        lo += len * 8;
        lo >= len * 8 || hi++;
        uint8_t tmp[64] = {};
        memcpy(tmp, src, len);
        tmp[len] = 0x80;
        if (len >= 56) {
            compress(tmp);
            memset(tmp, 0, 56);
        }
        PUT_BE(tmp + 56, hi);
        PUT_BE(tmp + 60, lo);
        compress(tmp);
        WRITE_BE(dst, h, 5);
    }
};
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef KK0
#undef KK1
#undef KK2
#undef KK3
#undef GGN
