#pragma once
#include "../utils.hpp"
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((x) ^ (y) ^ (z))
#define FF2(x, y, z) ((x) & (y) | (z) & ((x) | (y)))
#define FF3(x, y, z) ((x) ^ (y) ^ (z))
#define KK0 0x5A827999U
#define KK1 0x6ED9EBA1U
#define KK2 0x8F1BBCDCU
#define KK3 0xCA62C1D6U
#define GGR(N, a, b, c, d, e, w, i) do {                 \
    e = ROTL(a,  5) + FF##N(b, c, d) + e + KK##N + w[i]; \
    b = ROTL(b, 30);                                     \
} while (0)
template <bits_t R>
class SHA {
    void compress(uint32_t *w) {
        uint32_t x[5] = {
            h[0], h[1], h[2], h[3], h[4],
        };
        FOR_(i, 16, i + 1, i < 80, { w[i] = ROTL(w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3], R); });
        FOR_(j,  0, j + 1, j < 20, { GGR(0, x[(20 - j) % 5], x[(21 - j) % 5], x[(22 - j) % 5], x[(23 - j) % 5], x[(24 - j) % 5], w, j); });
        FOR_(j, 20, j + 1, j < 40, { GGR(1, x[(40 - j) % 5], x[(41 - j) % 5], x[(42 - j) % 5], x[(43 - j) % 5], x[(44 - j) % 5], w, j); });
        FOR_(j, 40, j + 1, j < 60, { GGR(2, x[(60 - j) % 5], x[(61 - j) % 5], x[(62 - j) % 5], x[(63 - j) % 5], x[(64 - j) % 5], w, j); });
        FOR_(j, 60, j + 1, j < 80, { GGR(3, x[(80 - j) % 5], x[(81 - j) % 5], x[(82 - j) % 5], x[(83 - j) % 5], x[(84 - j) % 5], w, j); });
        h[0] += x[0];
        h[1] += x[1];
        h[2] += x[2];
        h[3] += x[3];
        h[4] += x[4];
    }
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t h[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
    };
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 20;
    static constexpr bool NOT_ALWAYS_PADDING = false;
    void input(uint8_t const *blk) {
        uint32_t w[80];
        READB_BE(w, blk, 64);
        (lo += 64 * 8) < 64 * 8 && ++hi;
        compress(w);
    }
    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        uint32_t w[80];
        memset(w, 0, 64);
        READB_BE(w, src, len);
        (lo += len * 8) < len * 8 && ++hi;
        BYTE_BE(w, len) = 0x80;
        if (len >= 56) {
            compress(w);
            memset(w, 0, 56);
        }
        w[14] = hi;
        w[15] = lo;
        compress(w);
        WRITEB_BE(dst, h, 20);
    }
};
using SHA0 = SHA<0>;
using SHA1 = SHA<1>;
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef KK0
#undef KK1
#undef KK2
#undef KK3
#undef GGR
