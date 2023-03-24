#pragma once
#include <array>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define PPE(x) ((x) ^ ROL32(x,  9) ^ ROL32(x, 17))
#define PPW(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define HHN(N, a, b, c, d, e, f, g, h, w, K, X, Y)            \
    for (int j = X; j < Y; j++) {                             \
        r = ROL32(a, 12);                                     \
        s = r + e + K[j];                                     \
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
    static constexpr auto K = [](uint32_t KK0, uint32_t KK1) {
        std::array<uint32_t, 64> K = {};
        for (int j =  0; j < 16; j++) {
            K[j] = KK0 << j % 32 | KK0 >> (64 - j) % 32;
        }
        for (int j = 16; j < 64; j++) {
            K[j] = KK1 << j % 32 | KK1 >> (64 - j) % 32;
        }
        return K;
    }(0x79cc4519, 0x7a879d8a);
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
            uint8_t const *ref = &src[j << 2];
            w[j] = ref[0] << 24 | ref[1] << 16 | ref[2] << 8 | ref[3];
        }
        for (int j = 16; j < 68; j++) {
            t = w[j - 16] ^ w[j - 9] ^ ROL32(w[j -  3], 15);
            w[j] = PPW(t) ^ w[j - 6] ^ ROL32(w[j - 13],  7);
        }
        HHN(0, A, B, C, D, E, F, G, H, w, K,  0, 16);
        HHN(1, A, B, C, D, E, F, G, H, w, K, 16, 64);
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
        uint64_t ctrxx = counter + 8 * len;
        uint8_t *ctref = (uint8_t *)&ctrxx;
        tmp[63] = ctref[0];
        tmp[62] = ctref[1];
        tmp[61] = ctref[2];
        tmp[60] = ctref[3];
        tmp[59] = ctref[4];
        tmp[58] = ctref[5];
        tmp[57] = ctref[6];
        tmp[56] = ctref[7];
        copy.compress(tmp);
        for (int j = 0; j < 8; j++) {
            uint8_t *ref = &dst[j << 2];
            ref[0] = copy.h[j] >> 24;
            ref[1] = copy.h[j] >> 16;
            ref[2] = copy.h[j] >>  8;
            ref[3] = copy.h[j]      ;
        }
    }
};
#undef PPE
#undef PPW
#undef FF0
#undef FF1
#undef GG0
#undef GG1
#undef HHN
