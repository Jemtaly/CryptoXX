#pragma once
#include <array>
#include "block.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define RLX128(dh, dl, sh, sl, n) {              \
    (dh) = sh << ((n) & 63) | sl >> (-(n) & 63); \
    (dl) = sl << ((n) & 63) | sh >> (-(n) & 63); \
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
    static constexpr uint64_t SIGMA[6] = {
        0xA09E667F3BCC908B, 0xB67AE8584CAA73B2,
        0xC6EF372FE94F82BE, 0x54FF53A5F1D36F1C,
        0x10E527FADE682D1D, 0xB05688C2B3E6C1FD,
    };
    static constexpr uint8_t S_BOX[256] = {
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
    static constexpr auto LUT = []() {
        std::array<std::array<uint64_t, 256>, 8> LUT;
        for (int j = 0; j < 256; j++) {
            uint8_t a = S_BOX[j];
            uint8_t b = S_BOX[j] << 1 | S_BOX[j] >> 7;
            uint8_t c = S_BOX[j] << 7 | S_BOX[j] >> 1;
            uint8_t d = S_BOX[j << 1 & 0xff | j >> 7];
            LUT[0][j] = a * 0x0101010001000001U;
            LUT[1][j] = b * 0x0001010101010000U;
            LUT[2][j] = c * 0x0100010100010100U;
            LUT[3][j] = d * 0x0101000100000101U;
            LUT[4][j] = b * 0x0001010100010101U;
            LUT[5][j] = c * 0x0100010101000101U;
            LUT[6][j] = d * 0x0101000101010001U;
            LUT[7][j] = a * 0x0101010001010100U;
        }
        return LUT;
    }();
    static uint64_t f(uint64_t in, uint64_t kx) {
        uint64_t x = in ^ kx;
        return
            LUT[0][x >> 56       ] ^
            LUT[1][x >> 48 & 0xff] ^
            LUT[2][x >> 40 & 0xff] ^
            LUT[3][x >> 32 & 0xff] ^
            LUT[4][x >> 24 & 0xff] ^
            LUT[5][x >> 16 & 0xff] ^
            LUT[6][x >>  8 & 0xff] ^
            LUT[7][x       & 0xff];
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
        uint64_t lh, ll;
        uint64_t rh, rl;
        uint64_t ah, al;
        uint64_t bh, bl;
        uint64_t dh, dl;
        uint64_t holder;
        lh = GET64(kxy     );
        ll = GET64(kxy +  8);
        if (L == 2) {
            rh = 0;
            rl = 0;
        } else if (L == 3) {
            rh = GET64(kxy + 16);
            rl = ~rh;
        } else {
            rh = GET64(kxy + 16);
            rl = GET64(kxy + 24);
        }
        dh = lh ^ rh;
        dl = ll ^ rl;
        dl ^= f(dh, SIGMA[0]);
        dh ^= f(dl, SIGMA[1]);
        dh ^= lh;
        dl ^= ll;
        dl ^= f(dh, SIGMA[2]);
        dh ^= f(dl, SIGMA[3]);
        ah = dh;
        al = dl;
        dh ^= rh;
        dl ^= rl;
        dl ^= f(dh, SIGMA[4]);
        dh ^= f(dl, SIGMA[5]);
        bh = dh;
        bl = dl;
        if (L == 2) {
            RLX128(kx[ 0], kx[ 1], lh, ll,  0);
            RLX128(kx[ 2], kx[ 3], ah, al,  0);
            RLX128(kx[ 4], kx[ 5], lh, ll, 15);
            RLX128(kx[ 6], kx[ 7], ah, al, 15);
            RLX128(kx[ 8], kx[ 9], ah, al, 30);
            RLX128(kx[10], kx[11], lh, ll, 45);
            RLX128(kx[12], holder, ah, al, 45);
            RLX128(holder, kx[13], lh, ll, 60);
            RLX128(kx[14], kx[15], ah, al, 60);
            RLX128(kx[16], kx[17], ll, lh, 13);
            RLX128(kx[18], kx[19], ll, lh, 30);
            RLX128(kx[20], kx[21], al, ah, 30);
            RLX128(kx[22], kx[23], ll, lh, 47);
            RLX128(kx[24], kx[25], al, ah, 47);
        } else {
            RLX128(kx[ 0], kx[ 1], lh, ll,  0);
            RLX128(kx[ 2], kx[ 3], bh, bl,  0);
            RLX128(kx[ 4], kx[ 5], rh, rl, 15);
            RLX128(kx[ 6], kx[ 7], ah, al, 15);
            RLX128(kx[ 8], kx[ 9], rh, rl, 30);
            RLX128(kx[10], kx[11], bh, bl, 30);
            RLX128(kx[12], kx[13], lh, ll, 45);
            RLX128(kx[14], kx[15], ah, al, 45);
            RLX128(kx[16], kx[17], lh, ll, 60);
            RLX128(kx[18], kx[19], rh, rl, 60);
            RLX128(kx[20], kx[21], bh, bl, 60);
            RLX128(kx[22], kx[23], ll, lh, 13);
            RLX128(kx[24], kx[25], al, ah, 13);
            RLX128(kx[26], kx[27], rl, rh, 30);
            RLX128(kx[28], kx[29], al, ah, 30);
            RLX128(kx[30], kx[31], ll, lh, 47);
            RLX128(kx[32], kx[33], bl, bh, 47);
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
            d2 ^= f(d1, kx[i++]);
            d1 ^= f(d2, kx[i++]);
            d2 ^= f(d1, kx[i++]);
            d1 ^= f(d2, kx[i++]);
            d2 ^= f(d1, kx[i++]);
            d1 ^= f(d2, kx[i++]);
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
            d1 ^= f(d2, kx[--i]);
            d2 ^= f(d1, kx[--i]);
            d1 ^= f(d2, kx[--i]);
            d2 ^= f(d1, kx[--i]);
            d1 ^= f(d2, kx[--i]);
            d2 ^= f(d1, kx[--i]);
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