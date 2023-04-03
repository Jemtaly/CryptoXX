#pragma once
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define RLX32(x, n) ((x) << ((n) & 31) | (x) >> (-(n) & 31))
#define GET32(a) (                                    \
    (uint32_t)(a)[0] << 24 | (uint32_t)(a)[1] << 16 | \
    (uint32_t)(a)[2] <<  8 | (uint32_t)(a)[3]         \
)
#define PUT32(a, i) {          \
    (a)[0] = (i) >> 24       ; \
    (a)[1] = (i) >> 16 & 0xff; \
    (a)[2] = (i) >>  8 & 0xff; \
    (a)[3] = (i)       & 0xff; \
}
#define PPE(x) ((x) ^ ROL32(x,  9) ^ ROL32(x, 17))
#define PPW(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define KK0 0x79CC4519U
#define KK1 0x7A879D8AU
#define HHN(N, a, b, c, d, e, f, g, h, w, X, Y)               \
    for (int j = X; j < Y; j++) {                             \
        r = ROL32(a, 12);                                     \
        s = r + e + RLX32(KK##N, j);                          \
        t = ROL32(s,  7);                                     \
        u = FF##N(a, b, c) + d + (t ^ r) + (w[j] ^ w[j + 4]); \
        v = GG##N(e, f, g) + h +  t      +  w[j]            ; \
        d = c;                                                \
        h = g;                                                \
        c = ROL32(b,  9);                                     \
        g = ROL32(f, 19);                                     \
        b = a;                                                \
        f = e;                                                \
        a =     u ;                                           \
        e = PPE(v);                                           \
    }
struct SM3Inner {
    uint32_t h[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E,
    };
    void compress(uint8_t const *blk) {
        uint32_t A = h[0];
        uint32_t B = h[1];
        uint32_t C = h[2];
        uint32_t D = h[3];
        uint32_t E = h[4];
        uint32_t F = h[5];
        uint32_t G = h[6];
        uint32_t H = h[7];
        uint32_t w[68], t, r, s, u, v;
        for (int j =  0; j < 16; j++) {
            w[j] = GET32(blk + 4 * j);
        }
        for (int j = 16; j < 68; j++) {
            t = w[j - 16] ^ w[j - 9] ^ ROL32(w[j -  3], 15);
            w[j] = PPW(t) ^ w[j - 6] ^ ROL32(w[j - 13],  7);
        }
        HHN(0, A, B, C, D, E, F, G, H, w,  0, 16);
        HHN(1, A, B, C, D, E, F, G, H, w, 16, 64);
        h[0] ^= A;
        h[1] ^= B;
        h[2] ^= C;
        h[3] ^= D;
        h[4] ^= E;
        h[5] ^= F;
        h[6] ^= G;
        h[7] ^= H;
    }
};
class SM3 {
    SM3Inner save;
    uint32_t ctr_lo = 0;
    uint32_t ctr_hi = 0;
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 32;
    void push(uint8_t const *blk) {
        ctr_lo += 512;
        ctr_lo >= 512 || ctr_hi++;
        save.compress(blk);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) const {
        SM3Inner copy = save;
        uint32_t cpy_lo = ctr_lo;
        uint32_t cpy_hi = ctr_hi;
        cpy_lo += len * 8;
        cpy_lo >= len * 8 || cpy_hi++;
        for (; len >= 64; src += 64, len -= 64) {
            copy.compress(src);
        }
        uint8_t tmp[64] = {};
        memcpy(tmp, src, len);
        tmp[len] = 0x80;
        if (len >= 56) {
            copy.compress(tmp);
            memset(tmp, 0, 56);
        }
        PUT32(tmp + 56, cpy_hi);
        PUT32(tmp + 60, cpy_lo);
        copy.compress(tmp);
        for (int j = 0; j < 8; j++) {
            PUT32(dst + 4 * j, copy.h[j]);
        }
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
#undef HHN