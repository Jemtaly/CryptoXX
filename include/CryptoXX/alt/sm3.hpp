#pragma once
#include "../utils.hpp"
#define PPE(x) ((x) ^ ROTL(x,  9) ^ ROTL(x, 17))
#define PPW(x) ((x) ^ ROTL(x, 15) ^ ROTL(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define KK0 0x79CC4519U
#define KK1 0x7A879D8AU
#define HHR(N, a, b, c, d, e, f, g, h, w, j) do {         \
    s = ROTL(a, 12);                                      \
    t = ROTL(s + e + ROTL(KK##N, j), 7);                  \
    u = FF##N(a, b, c) + d + (t ^ s) + (w[j] ^ w[j + 4]); \
    v = GG##N(e, f, g) + h +  t      +  w[j]            ; \
    b = ROTL(b,  9);                                      \
    f = ROTL(f, 19);                                      \
    d =     u ;                                           \
    h = PPE(v);                                           \
} while (0)
class SM3 {
    void compress(uint32_t *w) {
        uint32_t x[4] = {
            h[0], h[1], h[2], h[3],
        };
        uint32_t y[4] = {
            h[4], h[5], h[6], h[7],
        };
        uint32_t t, s, u, v;
        FOR(j, 16, j + 1, j < 68, {
            t = w[j - 16] ^ w[j - 9] ^ ROTL(w[j -  3], 15);
            w[j] = PPW(t) ^ w[j - 6] ^ ROTL(w[j - 13],  7);
        });
        FOR(j,  0, j + 1, j < 16, { HHR(0, x[0 - j & 3], x[1 - j & 3], x[2 - j & 3], x[3 - j & 3], y[0 - j & 3], y[1 - j & 3], y[2 - j & 3], y[3 - j & 3], w, j); });
        FOR(j, 16, j + 1, j < 64, { HHR(1, x[0 - j & 3], x[1 - j & 3], x[2 - j & 3], x[3 - j & 3], y[0 - j & 3], y[1 - j & 3], y[2 - j & 3], y[3 - j & 3], w, j); });
        h[0] ^= x[0];
        h[1] ^= x[1];
        h[2] ^= x[2];
        h[3] ^= x[3];
        h[4] ^= y[0];
        h[5] ^= y[1];
        h[6] ^= y[2];
        h[7] ^= y[3];
    }
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t h[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E,
    };
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 32;
    static constexpr bool NOT_ALWAYS_PADDING = false;
    void input(uint8_t const *blk) {
        uint32_t w[68];
        READB_BE(w, blk, 64);
        (lo += 64 * 8) < 64 * 8 && ++hi;
        compress(w);
    }
    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        uint32_t w[68];
        memset(w, 0, 64);
        READB_BE(w, src, len);
        (lo += len * 8) < len * 8 && ++hi;
        BYTE_BE(w, len) = 0x80;
        if (len >= 56) {
            compress(w);
            memset(w, 0, 56);
        }
        w[14] = hi;
        w[15] = lo;
        compress(w);
        WRITEB_BE(dst, h, 32);
    }
};
#undef PPE
#undef PPW
#undef FF0
#undef FF1
#undef GG0
#undef GG1
#undef KK0
#undef KK1
#undef HHR
