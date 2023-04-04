#pragma once
#include "hash.hpp"
#define ROR32(x, n) ((x) >> (n) | (x) << (32 - (n)))
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
#define CHO(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define MAJ(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF1(s, t, u, v, a, b, c, d, e, f, g, h, w, K, i) { \
    s = ROR32(a, 2) ^ ROR32(a, 13) ^ ROR32(a, 22);         \
    t = ROR32(e, 6) ^ ROR32(e, 11) ^ ROR32(e, 25);         \
    u = t + CHO(e, f, g) + h + K[i] + w[i];                \
    v = s + MAJ(a, b, c)                  ;                \
    d = d + u;                                             \
    h = u + v;                                             \
}
#define FF8(s, t, u, v, a, b, c, d, e, f, g, h, w, K, i) { \
    FF1(s, t, u, v, a, b, c, d, e, f, g, h, w, K, i    );  \
    FF1(s, t, u, v, h, a, b, c, d, e, f, g, w, K, i + 1);  \
    FF1(s, t, u, v, g, h, a, b, c, d, e, f, w, K, i + 2);  \
    FF1(s, t, u, v, f, g, h, a, b, c, d, e, w, K, i + 3);  \
    FF1(s, t, u, v, e, f, g, h, a, b, c, d, w, K, i + 4);  \
    FF1(s, t, u, v, d, e, f, g, h, a, b, c, w, K, i + 5);  \
    FF1(s, t, u, v, c, d, e, f, g, h, a, b, w, K, i + 6);  \
    FF1(s, t, u, v, b, c, d, e, f, g, h, a, w, K, i + 7);  \
}
class SHA256Base {
protected:
    static constexpr uint32_t K[64] = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };
};
template <int DS, typename Derived>
class SHA256Tmpl: public SHA256Base {
    uint32_t hi = 0;
    uint32_t lo = 0;
    uint32_t h[8] = {
        Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
        Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
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
        uint32_t w[64], s, t, u, v;
        for (int i =  0; i < 16; i++) {
            w[i] = GET32(blk + 4 * i);
        }
        for (int i = 16; i < 64; i++) {
            s = ROR32(w[i - 15],  7) ^ ROR32(w[i - 15], 18) ^ (w[i - 15] >>  3);
            t = ROR32(w[i -  2], 17) ^ ROR32(w[i -  2], 19) ^ (w[i -  2] >> 10);
            w[i] = w[i - 16] + s + w[i - 7] + t;
        }
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K,  0);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K,  8);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 16);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 24);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 32);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 40);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 48);
        FF8(s, t, u, v, A, B, C, D, E, F, G, H, w, K, 56);
        h[0] += A;
        h[1] += B;
        h[2] += C;
        h[3] += D;
        h[4] += E;
        h[5] += F;
        h[6] += G;
        h[7] += H;
    }
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = DS;
    static constexpr bool NO_PADDING = false;
    SHA256Tmpl() {}
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
        for (int i = 0; i < DS / 4; i++) {
            PUT32(dst + 4 * i, h[i]);
        }
    }
};
class SHA256: public SHA256Tmpl<32, SHA256> {
public:
    static constexpr uint32_t IV[8] = {
        0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
        0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
    };
};
class SHA224: public SHA256Tmpl<28, SHA224> {
public:
    static constexpr uint32_t IV[8] = {
        0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
        0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4,
    };
};
#undef FF1
#undef FF8
