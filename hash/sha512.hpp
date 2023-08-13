#pragma once
#include "hash.hpp"
#define CHO(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define MAJ(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF1(s, t, u, v, a, b, c, d, e, f, g, h, w, K, i) do { \
    s = ROTR(a, 28) ^ ROTR(a, 34) ^ ROTR(a, 39);              \
    t = ROTR(e, 14) ^ ROTR(e, 18) ^ ROTR(e, 41);              \
    u = t + CHO(e, f, g) + h + K[i] + w[i];                   \
    v = s + MAJ(a, b, c)                  ;                   \
    d = d + u;                                                \
    h = u + v;                                                \
} while (0)
class SHA512Base {
protected:
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
};
template <int DN, std::array<uint64_t, 8> IV>
class SHA512Tmpl: public SHA512Base {
    void compress(uint64_t *w) {
        uint64_t A = h[0];
        uint64_t B = h[1];
        uint64_t C = h[2];
        uint64_t D = h[3];
        uint64_t E = h[4];
        uint64_t F = h[5];
        uint64_t G = h[6];
        uint64_t H = h[7];
        uint64_t s, t, u, v;
        for (int i = 16; i < 80; i++) {
            s = ROTR(w[i - 15],  1) ^ ROTR(w[i - 15],  8) ^ (w[i - 15] >> 7);
            t = ROTR(w[i -  2], 19) ^ ROTR(w[i -  2], 61) ^ (w[i -  2] >> 6);
            w[i] = w[i - 16] + s + w[i - 7] + t;
        }
        for (int i = 0; i < 80; i += 8) {
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
    uint64_t lo = 0;
    uint64_t hi = 0;
    uint64_t h[8] = {
        IV[0], IV[1], IV[2], IV[3],
        IV[4], IV[5], IV[6], IV[7],
    };
public:
    static constexpr size_t BLOCK_SIZE = 128;
    static constexpr size_t DIGEST_SIZE = DN;
    static constexpr bool NOT_ALWAYS_PADDING = false;
    void input(uint8_t const *blk) {
        uint64_t w[80];
        READB_BE(w, blk, 128);
        (lo += 128 * 8) < 128 * 8 && ++hi;
        compress(w);
    }
    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        uint64_t w[80];
        memset(w, 0, 128);
        READB_BE(w, src, len);
        (lo += len * 8) < len * 8 && ++hi;
        BYTE_BE(w, len) = 0x80;
        if (len >= 112) {
            compress(w);
            memset(w, 0, 112);
        }
        w[14] = hi;
        w[15] = lo;
        compress(w);
        WRITEB_BE(dst, h, DN);
    }
};
using SHA512 = SHA512Tmpl<64, std::array<uint64_t, 8>{
    0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
    0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
    0x510e527fade682d1, 0x9b05688c2b3e6c1f,
    0x1f83d9abfb41bd6b, 0x5be0cd19137e2179,
}>;
using SHA384 = SHA512Tmpl<48, std::array<uint64_t, 8>{
    0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
    0x9159015a3070dd17, 0x152fecd8f70e5939,
    0x67332667ffc00b31, 0x8eb44a8768581511,
    0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4,
}>;
#undef FF1
#undef CHO
#undef MAJ
