#pragma once
#include <array>
#include "block.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define ROL128(hi, lo, x, n) {                   \
    (hi) = (x)[0] << (n) | (x)[1] >> (64 - (n)); \
    (lo) = (x)[1] << (n) | (x)[0] >> (64 - (n)); \
}
#define GET64(p) (                                    \
    (uint64_t)(p)[0] << 56 | (uint64_t)(p)[1] << 48 | \
    (uint64_t)(p)[2] << 40 | (uint64_t)(p)[3] << 32 | \
    (uint64_t)(p)[4] << 24 | (uint64_t)(p)[5] << 16 | \
    (uint64_t)(p)[6] <<  8 | (uint64_t)(p)[7]         \
)
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
class CamelliaBase {
protected:
    static constexpr uint64_t sigma[6] = {
        0xA09E667F3BCC908B, 0xB67AE8584CAA73B2,
        0xC6EF372FE94F82BE, 0x54FF53A5F1D36F1C,
        0x10E527FADE682D1D, 0xB05688C2B3E6C1FD,
    };
    static constexpr std::array<uint8_t, 256> SBOX_1 = {
        0x70, 0x82, 0x2c, 0xec, 0xb3, 0x27, 0xc0, 0xe5, 0xe4, 0x85, 0x57, 0x35, 0xea, 0x0c, 0xae, 0x41,
        0x23, 0xef, 0x6b, 0x93, 0x45, 0x19, 0xa5, 0x21, 0xed, 0x0e, 0x4f, 0x4e, 0x1d, 0x65, 0x92, 0xbd,
        0x86, 0xb8, 0xaf, 0x8f, 0x7c, 0xeb, 0x1f, 0xce, 0x3e, 0x30, 0xdc, 0x5f, 0x5e, 0xc5, 0x0b, 0x1a,
        0xa6, 0xe1, 0x39, 0xca, 0xd5, 0x47, 0x5d, 0x3d, 0xd9, 0x01, 0x5a, 0xd6, 0x51, 0x56, 0x6c, 0x4d,
        0x8b, 0x0d, 0x9a, 0x66, 0xfb, 0xcc, 0xb0, 0x2d, 0x74, 0x12, 0x2b, 0x20, 0xf0, 0xb1, 0x84, 0x99,
        0xdf, 0x4c, 0xcb, 0xc2, 0x34, 0x7e, 0x76, 0x05, 0x6d, 0xb7, 0xa9, 0x31, 0xd1, 0x17, 0x04, 0xd7,
        0x14, 0x58, 0x3a, 0x61, 0xde, 0x1b, 0x11, 0x1c, 0x32, 0x0f, 0x9c, 0x16, 0x53, 0x18, 0xf2, 0x22,
        0xfe, 0x44, 0xcf, 0xb2, 0xc3, 0xb5, 0x7a, 0x91, 0x24, 0x08, 0xe8, 0xa8, 0x60, 0xfc, 0x69, 0x50,
        0xaa, 0xd0, 0xa0, 0x7d, 0xa1, 0x89, 0x62, 0x97, 0x54, 0x5b, 0x1e, 0x95, 0xe0, 0xff, 0x64, 0xd2,
        0x10, 0xc4, 0x00, 0x48, 0xa3, 0xf7, 0x75, 0xdb, 0x8a, 0x03, 0xe6, 0xda, 0x09, 0x3f, 0xdd, 0x94,
        0x87, 0x5c, 0x83, 0x02, 0xcd, 0x4a, 0x90, 0x33, 0x73, 0x67, 0xf6, 0xf3, 0x9d, 0x7f, 0xbf, 0xe2,
        0x52, 0x9b, 0xd8, 0x26, 0xc8, 0x37, 0xc6, 0x3b, 0x81, 0x96, 0x6f, 0x4b, 0x13, 0xbe, 0x63, 0x2e,
        0xe9, 0x79, 0xa7, 0x8c, 0x9f, 0x6e, 0xbc, 0x8e, 0x29, 0xf5, 0xf9, 0xb6, 0x2f, 0xfd, 0xb4, 0x59,
        0x78, 0x98, 0x06, 0x6a, 0xe7, 0x46, 0x71, 0xba, 0xd4, 0x25, 0xab, 0x42, 0x88, 0xa2, 0x8d, 0xfa,
        0x72, 0x07, 0xb9, 0x55, 0xf8, 0xee, 0xac, 0x0a, 0x36, 0x49, 0x2a, 0x68, 0x3c, 0x38, 0xf1, 0xa4,
        0x40, 0x28, 0xd3, 0x7b, 0xbb, 0xc9, 0x43, 0xc1, 0x15, 0xe3, 0xad, 0xf4, 0x77, 0xc7, 0x80, 0x9e,
    };
    static constexpr std::array<uint8_t, 256> SBOX_2 = []() {
        std::array<uint8_t, 256> s;
        for (int i = 0; i < 256; i++) {
            s[i] = SBOX_1[i] << 1 | SBOX_1[i] >> 7;
        }
        return s;
    }();
    static constexpr std::array<uint8_t, 256> SBOX_3 = []() {
        std::array<uint8_t, 256> s;
        for (int i = 0; i < 256; i++) {
            s[i] = SBOX_1[i] << 7 | SBOX_1[i] >> 1;
        }
        return s;
    }();
    static constexpr std::array<uint8_t, 256> SBOX_4 = []() {
        std::array<uint8_t, 256> s;
        for (int i = 0; i < 256; i++) {
            s[i] = SBOX_1[(i << 1 | i >> 7) & 255];
        }
        return s;
    }();
    static uint64_t feistel(uint64_t in, uint64_t kx) {
        uint64_t x = in ^ kx;
        uint8_t t[8], y[8];
        t[0] = SBOX_1[x >> 56       ];
        t[1] = SBOX_2[x >> 48 & 0xff];
        t[2] = SBOX_3[x >> 40 & 0xff];
        t[3] = SBOX_4[x >> 32 & 0xff];
        t[4] = SBOX_2[x >> 24 & 0xff];
        t[5] = SBOX_3[x >> 16 & 0xff];
        t[6] = SBOX_4[x >>  8 & 0xff];
        t[7] = SBOX_1[x       & 0xff];
        y[0] = t[0] ^ t[2] ^ t[3] ^ t[5] ^ t[6] ^ t[7];
        y[1] = t[0] ^ t[1] ^ t[3] ^ t[4] ^ t[6] ^ t[7];
        y[2] = t[0] ^ t[1] ^ t[2] ^ t[4] ^ t[5] ^ t[7];
        y[3] = t[1] ^ t[2] ^ t[3] ^ t[4] ^ t[5] ^ t[6];
        y[4] = t[0] ^ t[1] ^ t[5] ^ t[6] ^ t[7];
        y[5] = t[1] ^ t[2] ^ t[4] ^ t[6] ^ t[7];
        y[6] = t[2] ^ t[3] ^ t[4] ^ t[5] ^ t[7];
        y[7] = t[0] ^ t[3] ^ t[4] ^ t[5] ^ t[6];
        return GET64(y);
    }
    static uint64_t fl(uint64_t in, uint64_t kx) {
        uint32_t xh = in >> 32, xl = in & 0xffffffff;
        uint32_t kh = kx >> 32, kl = kx & 0xffffffff;
        xl ^= ROL32(xh & kh, 1);
        xh ^=       xl | kl    ;
        return (uint64_t)xh << 32 | (uint64_t)xl;
    }
    static uint64_t lf(uint64_t in, uint64_t kx) {
        uint32_t yh = in >> 32, yl = in & 0xffffffff;
        uint32_t kh = kx >> 32, kl = kx & 0xffffffff;
        yh ^=       yl | kl    ;
        yl ^= ROL32(yh & kh, 1);
        return (uint64_t)yh << 32 | (uint64_t)yl;
    }
public:
    static constexpr size_t BLOCK_SIZE = 16;
};
template <int L, int K = L == 2 ? 26 : 34>
requires (L == 2 || L == 3 || L == 4)
class CamelliaTmpl : public CamelliaBase {
    uint64_t kx[K];
public:
    CamelliaTmpl(const uint8_t *kxy) {
        uint64_t kl[ 2];
        uint64_t kr[ 2];
        uint64_t ka[ 2];
        uint64_t kb[ 2];
        uint64_t d1, d2;
        uint64_t holder;
        kl[0] = GET64(kxy     );
        kl[1] = GET64(kxy +  8);
        if (L == 2) {
            kr[0] = 0;
            kr[1] = 0;
        } else if (L == 3) {
            kr[0] = GET64(kxy + 16);
            kr[1] = ~kr[0];
        } else {
            kr[0] = GET64(kxy + 16);
            kr[1] = GET64(kxy + 24);
        }
        d1 = kl[0] ^ kr[0];
        d2 = kl[1] ^ kr[1];
        d2 ^= feistel(d1, sigma[0]);
        d1 ^= feistel(d2, sigma[1]);
        d1 ^= kl[0];
        d2 ^= kl[1];
        d2 ^= feistel(d1, sigma[2]);
        d1 ^= feistel(d2, sigma[3]);
        ka[0] = d1;
        ka[1] = d2;
        d1 ^= kr[0];
        d2 ^= kr[1];
        d2 ^= feistel(d1, sigma[4]);
        d1 ^= feistel(d2, sigma[5]);
        kb[0] = d1;
        kb[1] = d2;
        if (L == 2) {
            kx[ 0] = kl[0], kx[ 1] = kl[1];
            kx[ 2] = ka[0], kx[ 3] = ka[1];
            ROL128(kx[ 4], kx[ 5], kl, 15);
            ROL128(kx[ 6], kx[ 7], ka, 15);
            ROL128(kx[ 8], kx[ 9], ka, 30);
            ROL128(kx[10], kx[11], kl, 45);
            ROL128(kx[12], holder, ka, 45);
            ROL128(holder, kx[13], kl, 60);
            ROL128(kx[14], kx[15], ka, 60);
            ROL128(kx[17], kx[16], kl, 13);
            ROL128(kx[19], kx[18], kl, 30);
            ROL128(kx[21], kx[20], ka, 30);
            ROL128(kx[23], kx[22], kl, 47);
            ROL128(kx[25], kx[24], ka, 47);
        } else {
            kx[ 0] = kl[0], kx[ 1] = kl[1];
            kx[ 2] = kb[0], kx[ 3] = kb[1];
            ROL128(kx[ 4], kx[ 5], kr, 15);
            ROL128(kx[ 6], kx[ 7], ka, 15);
            ROL128(kx[ 8], kx[ 9], kr, 30);
            ROL128(kx[10], kx[11], kb, 30);
            ROL128(kx[12], kx[13], kl, 45);
            ROL128(kx[14], kx[15], ka, 45);
            ROL128(kx[16], kx[17], kl, 60);
            ROL128(kx[18], kx[19], kr, 60);
            ROL128(kx[20], kx[21], kb, 60);
            ROL128(kx[23], kx[22], kl, 13);
            ROL128(kx[25], kx[24], ka, 13);
            ROL128(kx[27], kx[26], kr, 30);
            ROL128(kx[29], kx[28], ka, 30);
            ROL128(kx[31], kx[30], kl, 47);
            ROL128(kx[33], kx[32], kb, 47);
        }
    }
    void encrypt(const uint8_t *src, uint8_t *dst) const {
        uint64_t d1, d2;
        d1 = GET64(src    );
        d2 = GET64(src + 8);
        int i = 0;
        d1 ^= kx[i++];
        d2 ^= kx[i++];
        for (;;) {
            d2 ^= feistel(d1, kx[i++]);
            d1 ^= feistel(d2, kx[i++]);
            d2 ^= feistel(d1, kx[i++]);
            d1 ^= feistel(d2, kx[i++]);
            d2 ^= feistel(d1, kx[i++]);
            d1 ^= feistel(d2, kx[i++]);
            if (i + 2 == K) { break; }
            d1 = fl(d1, kx[i++]);
            d2 = lf(d2, kx[i++]);
        }
        d2 ^= kx[i++];
        d1 ^= kx[i++];
        PUT64(dst    , d2);
        PUT64(dst + 8, d1);
    }
    void decrypt(const uint8_t *src, uint8_t *dst) const {
        uint64_t d1, d2;
        d2 = GET64(src    );
        d1 = GET64(src + 8);
        int i = K;
        d1 ^= kx[--i];
        d2 ^= kx[--i];
        for (;;) {
            d1 ^= feistel(d2, kx[--i]);
            d2 ^= feistel(d1, kx[--i]);
            d1 ^= feistel(d2, kx[--i]);
            d2 ^= feistel(d1, kx[--i]);
            d1 ^= feistel(d2, kx[--i]);
            d2 ^= feistel(d1, kx[--i]);
            if (i - 2 == 0) { break; }
            d2 = fl(d2, kx[--i]);
            d1 = lf(d1, kx[--i]);
        }
        d2 ^= kx[--i];
        d1 ^= kx[--i];
        PUT64(dst    , d1);
        PUT64(dst + 8, d2);
    }
};
using Camellia128 = CamelliaTmpl<2>;
using Camellia192 = CamelliaTmpl<3>;
using Camellia256 = CamelliaTmpl<4>;
