#pragma once
#include "hash.hpp"
#define CHO(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define MAJ(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF1(s, t, u, v, a, b, c, d, e, f, g, h, w, K, i) { \
    s = ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22);            \
    t = ROTR(e, 6) ^ ROTR(e, 11) ^ ROTR(e, 25);            \
    u = t + CHO(e, f, g) + h + K[i] + w[i];                \
    v = s + MAJ(a, b, c)                  ;                \
    d = d + u;                                             \
    h = u + v;                                             \
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
template <int DN, typename Derived>
class SHA256Tmpl: public SHA256Base {
    void compress(uint32_t *w) {
        uint32_t A = h[0];
        uint32_t B = h[1];
        uint32_t C = h[2];
        uint32_t D = h[3];
        uint32_t E = h[4];
        uint32_t F = h[5];
        uint32_t G = h[6];
        uint32_t H = h[7];
        uint32_t s, t, u, v;
        for (int i = 16; i < 64; i++) {
            s = ROTR(w[i - 15],  7) ^ ROTR(w[i - 15], 18) ^ (w[i - 15] >>  3);
            t = ROTR(w[i -  2], 17) ^ ROTR(w[i -  2], 19) ^ (w[i -  2] >> 10);
            w[i] = w[i - 16] + s + w[i - 7] + t;
        }
        for (int i = 0; i < 64; i += 8) {
            FF1(s, t, u, v, A, B, C, D, E, F, G, H, w, K, i    );
            FF1(s, t, u, v, H, A, B, C, D, E, F, G, w, K, i + 1);
            FF1(s, t, u, v, G, H, A, B, C, D, E, F, w, K, i + 2);
            FF1(s, t, u, v, F, G, H, A, B, C, D, E, w, K, i + 3);
            FF1(s, t, u, v, E, F, G, H, A, B, C, D, w, K, i + 4);
            FF1(s, t, u, v, D, E, F, G, H, A, B, C, w, K, i + 5);
            FF1(s, t, u, v, C, D, E, F, G, H, A, B, w, K, i + 6);
            FF1(s, t, u, v, B, C, D, E, F, G, H, A, w, K, i + 7);
        }
        h[0] += A;
        h[1] += B;
        h[2] += C;
        h[3] += D;
        h[4] += E;
        h[5] += F;
        h[6] += G;
        h[7] += H;
    }
    uint32_t hi = 0;
    uint32_t lo = 0;
    uint32_t h[8] = {
        Derived::IV[0], Derived::IV[1], Derived::IV[2], Derived::IV[3],
        Derived::IV[4], Derived::IV[5], Derived::IV[6], Derived::IV[7],
    };
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = DN;
    static constexpr bool NO_PADDING = false;
    void push(uint8_t const *blk) {
        uint32_t w[64];
        READB_BE(w, blk, 64);
        lo += 512;
        lo >= 512 || hi++;
        compress(w);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) {
        uint32_t w[64];
        memset(w, 0, 64);
        READB_BE(w, src, len);
        lo += len * 8;
        lo >= len * 8 || hi++;
        BYTE_BE(w, len) = 0x80;
        if (len >= 56) {
            compress(w);
            memset(w, 0, 56);
        }
        w[14] = hi;
        w[15] = lo;
        compress(w);
        WRITEB_BE(dst, h, DN);
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
#undef CHO
#undef MAJ
