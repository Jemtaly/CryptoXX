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
#define PUT64(a, i) {          \
    (a)[0] = (i) >> 56       ; \
    (a)[1] = (i) >> 48 & 0xff; \
    (a)[2] = (i) >> 40 & 0xff; \
    (a)[3] = (i) >> 32 & 0xff; \
    (a)[4] = (i) >> 24 & 0xff; \
    (a)[5] = (i) >> 16 & 0xff; \
    (a)[6] = (i) >>  8 & 0xff; \
    (a)[7] = (i)       & 0xff; \
}
#define PPE(x) ((x) ^ ROL32(x,  9) ^ ROL32(x, 17))
#define PPW(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define KK0 0x79CC4519U
#define KK1 0x7A879D8AU
#define HH1(N, a, b, c, d, e, f, g, h, w, j) {            \
    r = ROL32(a, 12);                                     \
    s = r + e + RLX32(KK##N, j);                          \
    t = ROL32(s,  7);                                     \
    u = FF##N(a, b, c) + d + (t ^ r) + (w[j] ^ w[j + 4]); \
    v = GG##N(e, f, g) + h +  t      +  w[j]            ; \
    b = ROL32(b,  9);                                     \
    f = ROL32(f, 19);                                     \
    d =     u ;                                           \
    h = PPE(v);                                           \
}
#define HH4(N, a, b, c, d, e, f, g, h, w, i) {            \
    HH1(N, a, b, c, d, e, f, g, h, w, i     );            \
    HH1(N, d, a, b, c, h, e, f, g, w, i +  1);            \
    HH1(N, c, d, a, b, g, h, e, f, w, i +  2);            \
    HH1(N, b, c, d, a, f, g, h, e, w, i +  3);            \
}
#define HHX(N, a, b, c, d, e, f, g, h, w, i) {            \
    HH4(N, a, b, c, d, e, f, g, h, w, i     );            \
    HH4(N, a, b, c, d, e, f, g, h, w, i +  4);            \
    HH4(N, a, b, c, d, e, f, g, h, w, i +  8);            \
    HH4(N, a, b, c, d, e, f, g, h, w, i + 12);            \
}
struct SM3Inner {
    uint32_t h[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E,
    };
    void compress(uint8_t const *src) {
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
            w[j] = GET32(src + 4 * j);
        }
        for (int j = 16; j < 68; j++) {
            t = w[j - 16] ^ w[j - 9] ^ ROL32(w[j -  3], 15);
            w[j] = PPW(t) ^ w[j - 6] ^ ROL32(w[j - 13],  7);
        }
        HHX(0, A, B, C, D, E, F, G, H, w,  0);
        HHX(1, A, B, C, D, E, F, G, H, w, 16);
        HHX(1, A, B, C, D, E, F, G, H, w, 32);
        HHX(1, A, B, C, D, E, F, G, H, w, 48);
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
    SM3Inner inner;
    uint64_t counter = 0;
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 32;
    void push(uint8_t const *src) {
        inner.compress(src);
        counter += 512;
    }
    void test(uint8_t const *src, size_t len, uint8_t *dst) const {
        SM3Inner copy = inner;
        uint8_t tmp[64];
        memcpy(tmp, src, len);
        memset(tmp + len, 0, 64 - len);
        tmp[len] = 0x80;
        if (len >= 56) {
            copy.compress(tmp);
            memset(tmp, 0, 56);
        }
        uint64_t cpad = counter + 8 * len;
        PUT64(tmp + 56, cpad);
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
#undef HH1
#undef HH4
#undef HHX
