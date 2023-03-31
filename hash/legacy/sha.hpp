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
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((x) ^ (y) ^ (z))
#define FF2(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF3(x, y, z) ((x) ^ (y) ^ (z))
#define KK0 0x5A827999U
#define KK1 0x6ED9EBA1U
#define KK2 0x8F1BBCDCU
#define KK3 0xCA62C1D6U
#define GGN(N, a, b, c, d, e, w, t, X, Y)                     \
    for (int i = X; i < Y; i++) {                             \
        t = ROL32(a,  5) + FF##N(b, c, d) + e + KK##N + w[i]; \
        e = d;                                                \
        d = c;                                                \
        c = ROL32(b, 30);                                     \
        b = a;                                                \
        a = t;                                                \
    }
struct SHAInner {
    uint32_t h[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
    };
    void compress(uint8_t const *src) {
        uint32_t a = h[0];
        uint32_t b = h[1];
        uint32_t c = h[2];
        uint32_t d = h[3];
        uint32_t e = h[4];
        uint32_t w[80], t;
        for (int i =  0; i < 16; i++) {
            w[i] = GET32(src + 4 * i);
        }
        for (int i = 16; i < 80; i++) {
            t = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
            w[i] = ROL32(t, 1);
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
};
class SHA {
    SHAInner inner;
    uint64_t counter = 0;
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 20;
    void push(uint8_t const *src) {
        inner.compress(src);
        counter += 512;
    }
    void test(uint8_t const *src, size_t len, uint8_t *dst) const {
        SHAInner copy = inner;
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
        for (int i = 0; i < 5; i++) {
            PUT32(dst + 4 * i, copy.h[i]);
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
#undef GGN
