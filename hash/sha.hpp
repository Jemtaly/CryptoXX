#pragma once
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
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
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((x) ^ (y) ^ (z))
#define FF2(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF3(x, y, z) ((x) ^ (y) ^ (z))
#define KK0 0x5A827999U
#define KK1 0x6ED9EBA1U
#define KK2 0x8F1BBCDCU
#define KK3 0xCA62C1D6U
#define GG1(N, a, b, c, d, e, w, i) {                     \
    e = ROL32(a,  5) + FF##N(b, c, d) + e + KK##N + w[i]; \
    b = ROL32(b, 30);                                     \
}
#define GG5(N, a, b, c, d, e, w, i) {                     \
    GG1(N, a, b, c, d, e, w, i     );                     \
    GG1(N, e, a, b, c, d, w, i +  1);                     \
    GG1(N, d, e, a, b, c, w, i +  2);                     \
    GG1(N, c, d, e, a, b, w, i +  3);                     \
    GG1(N, b, c, d, e, a, w, i +  4);                     \
}
#define GGX(N, a, b, c, d, e, w, i) {                     \
    GG5(N, a, b, c, d, e, w, i     );                     \
    GG5(N, a, b, c, d, e, w, i +  5);                     \
    GG5(N, a, b, c, d, e, w, i + 10);                     \
    GG5(N, a, b, c, d, e, w, i + 15);                     \
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
        for (int i =  0; i < 16; i++) {
            w[i] = GET32(blk + 4 * i);
        }
        for (int i = 16; i < 80; i++) {
            t = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
            w[i] = ROL32(t, 1);
        }
        GGX(0, a, b, c, d, e, w,  0);
        GGX(1, a, b, c, d, e, w, 20);
        GGX(2, a, b, c, d, e, w, 40);
        GGX(3, a, b, c, d, e, w, 60);
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
        PUT32(tmp + 56, hi);
        PUT32(tmp + 60, lo);
        compress(tmp);
        for (int i = 0; i < 5; i++) {
            PUT32(dst + 4 * i, h[i]);
        }
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
#undef GG1
#undef GG5
#undef GGX
