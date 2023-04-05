#pragma once
#include "hash.hpp"
#define FF0(x, y, z) ((x) & ((y) ^ (z)) ^ (z))
#define FF1(x, y, z) ((z) & ((x) ^ (y)) ^ (y))
#define FF2(x, y, z) ((x) ^ (y) ^ (z))
#define FF3(x, y, z) ((y) ^ ((x) | ~(z)))
#define GG0(i)      (i)
#define GG1(i) (5 * (i) + 1 & 0xf)
#define GG2(i) (3 * (i) + 5 & 0xf)
#define GG3(i) (7 * (i)     & 0xf)
#define HH1(N, a, b, c, d, s, w, K, R, i) {      \
    s = a + FF##N(b, c, d) + K[i] + w[GG##N(i)]; \
    a = b + ROTL(s, R[i]);                       \
}
#define HH4(N, a, b, c, d, s, w, K, R, i) {      \
    HH1(N, a, b, c, d, s, w, K, R, i     );      \
    HH1(N, d, a, b, c, s, w, K, R, i +  1);      \
    HH1(N, c, d, a, b, s, w, K, R, i +  2);      \
    HH1(N, b, c, d, a, s, w, K, R, i +  3);      \
}
#define HHX(N, a, b, c, d, s, w, K, R, i) {      \
    HH4(N, a, b, c, d, s, w, K, R, i     );      \
    HH4(N, a, b, c, d, s, w, K, R, i +  4);      \
    HH4(N, a, b, c, d, s, w, K, R, i +  8);      \
    HH4(N, a, b, c, d, s, w, K, R, i + 12);      \
}
typedef uint8_t bits_t;
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
    uint32_t lo = 0;
    uint32_t hi = 0;
    uint32_t h[4] = {
        0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476,
    };
    void compress(uint8_t const *blk) {
        uint32_t a = h[0];
        uint32_t b = h[1];
        uint32_t c = h[2];
        uint32_t d = h[3];
        uint32_t s, w[16];
        READ_LE(w, blk, 16);
        HHX(0, a, b, c, d, s, w, K, R,  0);
        HHX(1, a, b, c, d, s, w, K, R, 16);
        HHX(2, a, b, c, d, s, w, K, R, 32);
        HHX(3, a, b, c, d, s, w, K, R, 48);
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
    }
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 16;
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
        PUT_LE(tmp + 56, lo);
        PUT_LE(tmp + 60, hi);
        compress(tmp);
        WRITE_LE(dst, h, 4);
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
#undef HH1
#undef HH4
#undef HHX
