#pragma once
#include <array>
#include "hash.hpp"
#define SB_SC_MR_NRK(t, j) (                                        \
    LUT[0][t[ j         ].b[0]].w ^ LUT[1][t[(j + 7) % 8].b[1]].w ^ \
    LUT[2][t[(j + 6) % 8].b[2]].w ^ LUT[3][t[(j + 5) % 8].b[3]].w ^ \
    LUT[4][t[(j + 4) % 8].b[4]].w ^ LUT[5][t[(j + 3) % 8].b[5]].w ^ \
    LUT[6][t[(j + 2) % 8].b[6]].w ^ LUT[7][t[(j + 1) % 8].b[7]].w   \
)
union WhirlpoolRow {
    uint64_t w;
    uint8_t b[8];
};
class Whirlpool {
    static constexpr uint8_t L_INI[16] = {
        0x1, 0xB, 0x9, 0xC, 0xD, 0x6, 0xF, 0x3, 0xE, 0x8, 0x7, 0x4, 0xA, 0x2, 0x5, 0x0,
    };
    static constexpr uint8_t H_INI[16] = {
        0xF, 0x0, 0xD, 0x7, 0xB, 0xE, 0x5, 0xA, 0x9, 0x2, 0xC, 0x1, 0x3, 0x4, 0x8, 0x6,
    };
    static constexpr uint8_t S_INI[16] = {
        0x7, 0xC, 0xB, 0xD, 0xE, 0x4, 0x9, 0xF, 0x6, 0x3, 0x8, 0xA, 0x2, 0x5, 0x1, 0x0,
    };
    static constexpr auto S_BOX = []() {
        std::array<uint8_t, 256> S_BOX = {};
        uint8_t ha, la, hb, lb, tt;
        for (ha = 0; ha < 16; ha++) {
            for (la = 0; la < 16; la++) {
                hb = H_INI[ha];
                lb = L_INI[la];
                tt = S_INI[hb ^ lb];
                hb = H_INI[hb ^ tt];
                lb = L_INI[lb ^ tt];
                S_BOX[ha | la << 4] = hb | lb << 4;
            }
        }
        return S_BOX;
    }();
    static constexpr auto RC = []() {
        std::array<WhirlpoolRow, 10> RC = {};
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 8; j++) {
                RC[i].b[j] = S_BOX[8 * i + j];
            }
        }
        return RC;
    }();
    // Whirlpool's Galois Field multiplication
    static constexpr auto multiply = [](uint8_t a, uint8_t b) {
        uint8_t p = 0;
        for (int i = 0; i < 8; i++) {
            p = p ^ (b >> i & 0x01 ? a    : 0x00);
            a = a << 1 ^ (a & 0x80 ? 0x1D : 0x00);
        }
        return p;
    };
    // Generate LUT for SubBytes and MixRows steps
    static constexpr auto generate_LUT = [](WhirlpoolRow poly, std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<WhirlpoolRow, 256>, 8> LUT = {};
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 256; j++) {
                uint8_t p = multiply(poly.b[i], S_BOX[j]);
                for (int k = 0; k < 8; k++) {
                    LUT[k][j].b[(i + k) % 8] = p;
                }
            }
        }
        return LUT;
    };
    static constexpr auto LUT = generate_LUT({.b = {1, 1, 4, 1, 8, 5, 2, 9}}, S_BOX);
    void compress(WhirlpoolRow const *b) {
        WhirlpoolRow s[8], k[8], t[8];
        s[0].w = b[0].w ^ (k[0].w = h[0].w);
        s[1].w = b[1].w ^ (k[1].w = h[1].w);
        s[2].w = b[2].w ^ (k[2].w = h[2].w);
        s[3].w = b[3].w ^ (k[3].w = h[3].w);
        s[4].w = b[4].w ^ (k[4].w = h[4].w);
        s[5].w = b[5].w ^ (k[5].w = h[5].w);
        s[6].w = b[6].w ^ (k[6].w = h[6].w);
        s[7].w = b[7].w ^ (k[7].w = h[7].w);
        for (int i = 0; i < 10; i++) {
            t[0].w = k[0].w;
            t[1].w = k[1].w;
            t[2].w = k[2].w;
            t[3].w = k[3].w;
            t[4].w = k[4].w;
            t[5].w = k[5].w;
            t[6].w = k[6].w;
            t[7].w = k[7].w;
            k[0].w = SB_SC_MR_NRK(t, 0) ^ RC[i].w;
            k[1].w = SB_SC_MR_NRK(t, 1);
            k[2].w = SB_SC_MR_NRK(t, 2);
            k[3].w = SB_SC_MR_NRK(t, 3);
            k[4].w = SB_SC_MR_NRK(t, 4);
            k[5].w = SB_SC_MR_NRK(t, 5);
            k[6].w = SB_SC_MR_NRK(t, 6);
            k[7].w = SB_SC_MR_NRK(t, 7);
            t[0].w = s[0].w;
            t[1].w = s[1].w;
            t[2].w = s[2].w;
            t[3].w = s[3].w;
            t[4].w = s[4].w;
            t[5].w = s[5].w;
            t[6].w = s[6].w;
            t[7].w = s[7].w;
            s[0].w = SB_SC_MR_NRK(t, 0) ^ k[0].w;
            s[1].w = SB_SC_MR_NRK(t, 1) ^ k[1].w;
            s[2].w = SB_SC_MR_NRK(t, 2) ^ k[2].w;
            s[3].w = SB_SC_MR_NRK(t, 3) ^ k[3].w;
            s[4].w = SB_SC_MR_NRK(t, 4) ^ k[4].w;
            s[5].w = SB_SC_MR_NRK(t, 5) ^ k[5].w;
            s[6].w = SB_SC_MR_NRK(t, 6) ^ k[6].w;
            s[7].w = SB_SC_MR_NRK(t, 7) ^ k[7].w;
        }
        h[0].w ^= s[0].w ^ b[0].w;
        h[1].w ^= s[1].w ^ b[1].w;
        h[2].w ^= s[2].w ^ b[2].w;
        h[3].w ^= s[3].w ^ b[3].w;
        h[4].w ^= s[4].w ^ b[4].w;
        h[5].w ^= s[5].w ^ b[5].w;
        h[6].w ^= s[6].w ^ b[6].w;
        h[7].w ^= s[7].w ^ b[7].w;
    }
    WhirlpoolRow h[8] = {};
    uint64_t ctr[4] = {};
public:
    static constexpr size_t BLOCK_SIZE = 64;
    static constexpr size_t DIGEST_SIZE = 64;
    static constexpr bool NO_PADDING = false;
    void push(uint8_t const *data) {
        WhirlpoolRow w[8];
        memcpy(w, data, 64);
        (ctr[3] += 512) < 512
            && ++ctr[2] == 0
            && ++ctr[1] == 0
            && ++ctr[0] == 0;
        compress(w);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *hash) {
        WhirlpoolRow w[8];
        memset(w, 0, 64);
        memcpy(w, src, len);
        (ctr[3] += len * 8) < len * 8
            && ++ctr[2] == 0
            && ++ctr[1] == 0
            && ++ctr[0] == 0;
        ((uint8_t *)w)[len] = 0x80;
        if (len >= 32) {
            compress(w);
            memset(w, 0, 32);
        }
        PUT_BE(w[4].b, ctr[0]);
        PUT_BE(w[5].b, ctr[1]);
        PUT_BE(w[6].b, ctr[2]);
        PUT_BE(w[7].b, ctr[3]);
        compress(w);
        memcpy(hash, h, 64);
    }
};
#undef SB_SC_MR_NRK
