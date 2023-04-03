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
struct SHA256Inner {
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
    uint32_t h[8];
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
        for (int i =  0; i < 64; i++) {
            s = ROR32(A, 2) ^ ROR32(A, 13) ^ ROR32(A, 22);
            t = ROR32(E, 6) ^ ROR32(E, 11) ^ ROR32(E, 25);
            u = t + CHO(E, F, G) + H + K[i] + w[i];
            v = s + MAJ(A, B, C)                  ;
            H = G;
            G = F;
            F = E;
            E = D + u;
            D = C;
            C = B;
            B = A;
            A = u + v;
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
};
template <int DS, typename Derived>
class SHA256Tmpl {
    uint32_t ctr_lo = 0;
    uint32_t ctr_hi = 0;
protected:
    SHA256Inner save;
public:
    SHA256Tmpl() {
        save.h[0] = Derived::IV[0];
        save.h[1] = Derived::IV[1];
        save.h[2] = Derived::IV[2];
        save.h[3] = Derived::IV[3];
        save.h[4] = Derived::IV[4];
        save.h[5] = Derived::IV[5];
        save.h[6] = Derived::IV[6];
        save.h[7] = Derived::IV[7];
    }
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = DS;
    void push(uint8_t const *blk) {
        ctr_lo += 512;
        ctr_lo >= 512 || ctr_hi++;
        save.compress(blk);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) const {
        SHA256Inner copy = save;
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
        for (int i = 0; i < DS / 4; i++) {
            PUT32(dst + 4 * i, copy.h[i]);
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
