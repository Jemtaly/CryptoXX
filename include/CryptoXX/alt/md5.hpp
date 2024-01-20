#pragma once
#include "../utils.hpp"
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((z) & ((x) ^ (y)) ^ (y))
#define FF2(x, y, z) ((x) ^ (y) ^ (z))
#define FF3(x, y, z) ((y) ^ ((x) | ~(z)))
#define GG0(i)      (i)
#define GG1(i) (5 * (i) + 1 & 0xf)
#define GG2(i) (3 * (i) + 5 & 0xf)
#define GG3(i) (7 * (i)     & 0xf)
#define HHR(N, a, b, c, d, s, w, K, R, i) a = b + ROTL(a + FF##N(b, c, d) + K[i] + w[GG##N(i)], R[i])
class MD5 {
    static constexpr uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };
    static constexpr bits_t R[64] = {
        0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
        0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
        0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
        0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
        0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
        0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
        0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
        0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
    };
    void compress(uint32_t const *w) {
        uint32_t x[4] = {
            h[0], h[1], h[2], h[3],
        };
        FOR_(j,  0, j + 1, j < 16, { HHR(0, x[(16 - j) % 4], x[(17 - j) % 4], x[(18 - j) % 4], x[(19 - j) % 4], s, w, K, R, j); });
        FOR_(j, 16, j + 1, j < 32, { HHR(1, x[(32 - j) % 4], x[(33 - j) % 4], x[(34 - j) % 4], x[(35 - j) % 4], s, w, K, R, j); });
        FOR_(j, 32, j + 1, j < 48, { HHR(2, x[(48 - j) % 4], x[(49 - j) % 4], x[(50 - j) % 4], x[(51 - j) % 4], s, w, K, R, j); });
        FOR_(j, 48, j + 1, j < 64, { HHR(3, x[(64 - j) % 4], x[(65 - j) % 4], x[(66 - j) % 4], x[(67 - j) % 4], s, w, K, R, j); });
        h[0] += x[0];
        h[1] += x[1];
        h[2] += x[2];
        h[3] += x[3];
    }
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t h[4] = {
        0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476,
    };
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 16;
    static constexpr bool NOT_ALWAYS_PADDING = false;
    void input(uint8_t const *blk) {
        uint32_t w[16];
        READB_LE(w, blk, 64);
        (lo += 64 * 8) < 64 * 8 && ++hi;
        compress(w);
    }
    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        uint32_t w[16];
        memset(w, 0, 64);
        READB_LE(w, src, len);
        (lo += len * 8) < len * 8 && ++hi;
        BYTE_LE(w, len) = 0x80;
        if (len >= 56) {
            compress(w);
            memset(w, 0, 56);
        }
        w[14] = lo;
        w[15] = hi;
        compress(w);
        WRITEB_LE(dst, h, 16);
    }
};
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef GG0
#undef GG1
#undef GG2
#undef GG3
#undef HHR
