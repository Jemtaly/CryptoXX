#pragma once
#include "../utils.hpp"
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
    static constexpr auto F_LUT = []() {
        std::array<std::array<uint64_t, 256>, 8> F_LUT = {};
        for (int j = 0; j < 256; j++) {
            uint8_t a = S_BOX[j];
            uint8_t b = S_BOX[j] << 1 | S_BOX[j] >> 7;
            uint8_t c = S_BOX[j] << 7 | S_BOX[j] >> 1;
            uint8_t d = S_BOX[j << 1 & 0xff | j >> 7];
            F_LUT[0][j] = a * 0x0101010001000001U;
            F_LUT[1][j] = b * 0x0001010101010000U;
            F_LUT[2][j] = c * 0x0100010100010100U;
            F_LUT[3][j] = d * 0x0101000100000101U;
            F_LUT[4][j] = b * 0x0001010100010101U;
            F_LUT[5][j] = c * 0x0100010101000101U;
            F_LUT[6][j] = d * 0x0101000101010001U;
            F_LUT[7][j] = a * 0x0101010001010100U;
        }
        return F_LUT;
    }();
    static uint64_t f(uint64_t qi, uint64_t qk) {
        uint64_t qx = qi ^ qk;
        return
            F_LUT[0][qx >> 56       ] ^
            F_LUT[1][qx >> 48 & 0xff] ^
            F_LUT[2][qx >> 40 & 0xff] ^
            F_LUT[3][qx >> 32 & 0xff] ^
            F_LUT[4][qx >> 24 & 0xff] ^
            F_LUT[5][qx >> 16 & 0xff] ^
            F_LUT[6][qx >>  8 & 0xff] ^
            F_LUT[7][qx       & 0xff];
    }
    static uint64_t fl(uint64_t qi, uint64_t qk) {
        uint32_t hi = qi >> 32, li = qi & 0xFFFFFFFF;
        uint32_t hk = qk >> 32, lk = qk & 0xFFFFFFFF;
        li ^= ROTL(hi & hk, 1);
        hi ^=      li | lk    ;
        return (uint64_t)hi << 32 | (uint64_t)li;
    }
    static uint64_t lf(uint64_t qi, uint64_t qk) {
        uint32_t hi = qi >> 32, li = qi & 0xFFFFFFFF;
        uint32_t hk = qk >> 32, lk = qk & 0xFFFFFFFF;
        hi ^=      li | lk    ;
        li ^= ROTL(hi & hk, 1);
        return (uint64_t)hi << 32 | (uint64_t)li;
    }
};
template <int L, int K = L == 2 ? 26 : 34>
    requires (L == 2 || L == 3 || L == 4)
class CamelliaTmpl: public CamelliaBase {
    uint64_t kx[K];
public:
    static constexpr size_t BLOCK_SIZE = 16;
    static constexpr size_t KEY_SIZE = 8 * L;
    CamelliaTmpl(uint8_t const *kxy) {
        uint64_t lh, ll;
        uint64_t rh, rl;
        uint64_t ah, al;
        uint64_t bh, bl;
        uint64_t d1, d2;
        lh = GET_BE<uint64_t>(kxy     );
        ll = GET_BE<uint64_t>(kxy +  8);
        if constexpr (L == 2) {
            rh = 0;
            rl = 0;
        } else if constexpr (L == 3) {
            rh = GET_BE<uint64_t>(kxy + 16);
            rl = ~rh;
        } else {
            rh = GET_BE<uint64_t>(kxy + 16);
            rl = GET_BE<uint64_t>(kxy + 24);
        }
        d1 = lh ^ rh;
        d2 = ll ^ rl;
        d2 ^= f(d1, SIGMA[0]);
        d1 ^= f(d2, SIGMA[1]);
        d1 ^= lh;
        d2 ^= ll;
        d2 ^= f(d1, SIGMA[2]);
        d1 ^= f(d2, SIGMA[3]);
        ah = d1;
        al = d2;
        d1 ^= rh;
        d2 ^= rl;
        d2 ^= f(d1, SIGMA[4]);
        d1 ^= f(d2, SIGMA[5]);
        bh = d1;
        bl = d2;
        if constexpr (L == 2) {
            kx[ 0] = lh;                         kx[ 1] = ll;
            kx[ 2] = ah;                         kx[ 3] = al;
            kx[ 4] = lh << 15 | ll >> (64 - 15); kx[ 5] = ll << 15 | lh >> (64 - 15);
            kx[ 6] = ah << 15 | al >> (64 - 15); kx[ 7] = al << 15 | ah >> (64 - 15);
            kx[ 8] = ah << 30 | al >> (64 - 30); kx[ 9] = al << 30 | ah >> (64 - 30);
            kx[10] = lh << 45 | ll >> (64 - 45); kx[11] = ll << 45 | lh >> (64 - 45);
            kx[12] = ah << 45 | al >> (64 - 45); kx[13] = ll << 60 | lh >> (64 - 60);
            kx[14] = ah << 60 | al >> (64 - 60); kx[15] = al << 60 | ah >> (64 - 60);
            kx[16] = lh >> 51 | ll << (64 - 51); kx[17] = ll >> 51 | lh << (64 - 51);
            kx[18] = lh >> 34 | ll << (64 - 34); kx[19] = ll >> 34 | lh << (64 - 34);
            kx[20] = ah >> 34 | al << (64 - 34); kx[21] = al >> 34 | ah << (64 - 34);
            kx[22] = lh >> 17 | ll << (64 - 17); kx[23] = ll >> 17 | lh << (64 - 17);
            kx[24] = ah >> 17 | al << (64 - 17); kx[25] = al >> 17 | ah << (64 - 17);
        } else {
            kx[ 0] = lh;                         kx[ 1] = ll;
            kx[ 2] = bh;                         kx[ 3] = bl;
            kx[ 4] = rh << 15 | rl >> (64 - 15); kx[ 5] = rl << 15 | rh >> (64 - 15);
            kx[ 6] = ah << 15 | al >> (64 - 15); kx[ 7] = al << 15 | ah >> (64 - 15);
            kx[ 8] = rh << 30 | rl >> (64 - 30); kx[ 9] = rl << 30 | rh >> (64 - 30);
            kx[10] = bh << 30 | bl >> (64 - 30); kx[11] = bl << 30 | bh >> (64 - 30);
            kx[12] = lh << 45 | ll >> (64 - 45); kx[13] = ll << 45 | lh >> (64 - 45);
            kx[14] = ah << 45 | al >> (64 - 45); kx[15] = al << 45 | ah >> (64 - 45);
            kx[16] = lh << 60 | ll >> (64 - 60); kx[17] = ll << 60 | lh >> (64 - 60);
            kx[18] = rh << 60 | rl >> (64 - 60); kx[19] = rl << 60 | rh >> (64 - 60);
            kx[20] = bh << 60 | bl >> (64 - 60); kx[21] = bl << 60 | bh >> (64 - 60);
            kx[22] = lh >> 51 | ll << (64 - 51); kx[23] = ll >> 51 | lh << (64 - 51);
            kx[24] = ah >> 51 | al << (64 - 51); kx[25] = al >> 51 | ah << (64 - 51);
            kx[26] = rh >> 34 | rl << (64 - 34); kx[27] = rl >> 34 | rh << (64 - 34);
            kx[28] = ah >> 34 | al << (64 - 34); kx[29] = al >> 34 | ah << (64 - 34);
            kx[30] = lh >> 17 | ll << (64 - 17); kx[31] = ll >> 17 | lh << (64 - 17);
            kx[32] = bh >> 17 | bl << (64 - 17); kx[33] = bl >> 17 | bh << (64 - 17);
        }
    }
    void encrypt(const uint8_t *src, uint8_t *dst) const {
        uint64_t d2, d1;
        d1 = GET_BE<uint64_t>(src    );
        d2 = GET_BE<uint64_t>(src + 8);
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
        PUT_BE(dst    , d2);
        PUT_BE(dst + 8, d1);
    }
    void decrypt(const uint8_t *src, uint8_t *dst) const {
        uint64_t d1, d2;
        d2 = GET_BE<uint64_t>(src    );
        d1 = GET_BE<uint64_t>(src + 8);
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
        PUT_BE(dst    , d1);
        PUT_BE(dst + 8, d2);
    }
};
using Camellia128 = CamelliaTmpl<2>;
using Camellia192 = CamelliaTmpl<3>;
using Camellia256 = CamelliaTmpl<4>;
