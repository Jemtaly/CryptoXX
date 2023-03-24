#pragma once
#include "hash.hpp"
#define ROR64(x, n) ((x) >> (n) | (x) << (64 - (n)))
#define CHO(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define MAJ(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
struct SHA512Inner {
    static constexpr uint64_t K[80] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
        0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
        0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
        0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
        0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
        0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
        0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
        0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
        0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
        0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
        0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
    };
    uint64_t h[8];
    void compress(uint8_t const *src) {
        uint64_t A = h[0];
        uint64_t B = h[1];
        uint64_t C = h[2];
        uint64_t D = h[3];
        uint64_t E = h[4];
        uint64_t F = h[5];
        uint64_t G = h[6];
        uint64_t H = h[7];
        uint64_t w[80], s, t, u, v;
        for (int i =  0; i < 16; i++) {
            uint8_t const *ref = &src[i << 3];
            w[i] =
                (uint64_t)ref[0] << 56 | (uint64_t)ref[1] << 48 | (uint64_t)ref[2] << 40 | (uint64_t)ref[3] << 32 |
                (uint64_t)ref[4] << 24 | (uint64_t)ref[5] << 16 | (uint64_t)ref[6] <<  8 | (uint64_t)ref[7]      ;
        }
        for (int i = 16; i < 80; i++) {
            s = ROR64(w[i - 15],  1) ^ ROR64(w[i - 15],  8) ^ (w[i - 15] >> 7);
            t = ROR64(w[i -  2], 19) ^ ROR64(w[i -  2], 61) ^ (w[i -  2] >> 6);
            w[i] = w[i - 16] + s + w[i - 7] + t;
        }
        for (int i =  0; i < 80; i++) {
            s = ROR64(A, 28) ^ ROR64(A, 34) ^ ROR64(A, 39);
            t = ROR64(E, 14) ^ ROR64(E, 18) ^ ROR64(E, 41);
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
template <int DS>
class SHA512Tmpl {
    uint64_t ctr_lo = 0;
    uint64_t ctr_hi = 0;
protected:
    SHA512Inner inner;
    SHA512Tmpl() = default; // not instantiable
public:
    static const int BLOCK_SIZE = 128;
    static const int DIGEST_SIZE = DS;
    void push(uint8_t const *src) {
        inner.compress(src);
        (ctr_lo += 1024) < 1024 && (ctr_hi++);
    }
    void test(uint8_t const *src, size_t len, uint8_t *dst) const {
        SHA512Inner copy = inner;
        uint8_t tmp[128];
        memcpy(tmp, src, len);
        memset(tmp + len, 0, 128 - len);
        tmp[len] = 0x80;
        if (len >= 112) {
            copy.compress(tmp);
            memset(tmp, 0, 112);
        }
        uint64_t lo = ctr_lo + len * 8;
        uint64_t hi = ctr_hi + (lo < len * 8);
        for (int i = 0; i < 8; i++) {
            tmp[112 + i] = hi >> (56 - i * 8);
            tmp[120 + i] = lo >> (56 - i * 8);
        }
        copy.compress(tmp);
        for (int i = 0; i < DS; i++) {
            dst[i] = copy.h[i >> 3] >> (56 - (i & 7) * 8);
        }
    }
};
class SHA512: public SHA512Tmpl<64> {
public:
    SHA512() {
        inner.h[0] = 0x6a09e667f3bcc908;
        inner.h[1] = 0xbb67ae8584caa73b;
        inner.h[2] = 0x3c6ef372fe94f82b;
        inner.h[3] = 0xa54ff53a5f1d36f1;
        inner.h[4] = 0x510e527fade682d1;
        inner.h[5] = 0x9b05688c2b3e6c1f;
        inner.h[6] = 0x1f83d9abfb41bd6b;
        inner.h[7] = 0x5be0cd19137e2179;
    }
};
class SHA384: public SHA512Tmpl<48> {
public:
    SHA384() {
        inner.h[0] = 0xcbbb9d5dc1059ed8;
        inner.h[1] = 0x629a292a367cd507;
        inner.h[2] = 0x9159015a3070dd17;
        inner.h[3] = 0x152fecd8f70e5939;
        inner.h[4] = 0x67332667ffc00b31;
        inner.h[5] = 0x8eb44a8768581511;
        inner.h[6] = 0xdb0c2e0d64f98fa7;
        inner.h[7] = 0x47b5481dbefa4fa4;
    }
};
