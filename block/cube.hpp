#pragma once
#include "block.hpp"
union CubeWord {
    uint64_t w;
    uint8_t b[8];
};
class Cube {
protected:
    static constexpr uint8_t H_INI[16] = {
        0x1, 0xB, 0x9, 0xC, 0xD, 0x6, 0xF, 0x3, 0xE, 0x8, 0x7, 0x4, 0xA, 0x2, 0x5, 0x0,
    };
    static constexpr uint8_t L_INI[16] = {
        0xF, 0x0, 0xD, 0x7, 0xB, 0xE, 0x5, 0xA, 0x9, 0x2, 0xC, 0x1, 0x3, 0x4, 0x8, 0x6,
    };
    static constexpr uint8_t S_INI[16] = {
        0x7, 0xC, 0xB, 0xD, 0xE, 0x4, 0x9, 0xF, 0x6, 0x3, 0x8, 0xA, 0x2, 0x5, 0x1, 0x0,
    };
    static constexpr auto generate_BOX = [](uint8_t const *H_INI, uint8_t const *L_INI, uint8_t const *S_INI) {
        std::array<uint8_t, 256> S_BOX = {};
        uint8_t li, hi, lo, ho, tt;
        for (li = 0; li < 16; li++) {
            for (hi = 0; hi < 16; hi++) {
                lo = L_INI[li];
                ho = H_INI[hi];
                tt = S_INI[lo ^ ho];
                lo = L_INI[lo ^ tt];
                ho = H_INI[ho ^ tt];
                S_BOX[li | hi << 4] = lo | ho << 4;
            }
        }
        return S_BOX;
    };
    static constexpr auto E_BOX = generate_BOX(H_INI, L_INI, S_INI);
    static constexpr auto D_BOX = generate_BOX(L_INI, H_INI, S_INI);
    static constexpr auto IDBOX = []() {
        std::array<uint8_t, 256> IDBOX = {};
        for (int i = 0; i < 256; i++) {
            IDBOX[i] = i;
        }
        return IDBOX;
    }();
    static constexpr auto generate_TWT = [](std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<CubeWord, 256>, 8> TWT_S = {};
        for (int j = 0; j < 256; j++) {
            uint8_t p = S_BOX[j];
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    TWT_S[k][j].b[i] = (p >> i & 1) << k;
                }
            }
        }
        return TWT_S;
    };
    static constexpr auto generate_LUT = [](std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<CubeWord, 256>, 8> LUT_S = {};
        for (int j = 0; j < 256; j++) {
            uint8_t p = S_BOX[j];
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    LUT_S[k][j].b[i] = k == i ? p : 0;
                }
            }
        }
        return LUT_S;
    };
    static constexpr auto TWTID = generate_TWT(IDBOX); // MixColumns
    static constexpr auto TWT_E = generate_TWT(E_BOX); // MixColumns and EncSubBytes
    static constexpr auto TWT_D = generate_TWT(D_BOX); // MixColumns and DecSubBytes
    static constexpr auto LUT_E = generate_LUT(E_BOX); // EncSubBytes
    static constexpr auto LUT_D = generate_LUT(D_BOX); // DecSubBytes
    static constexpr auto Nr = 10;
    static constexpr auto CK = []() {
        std::array<std::array<CubeWord, 8>, 10> CK = {};
        for (int r = 0; r < 10; r++) {
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    CK[r][i].b[k] = r;
                }
            }
        }
        return CK;
    }();
    CubeWord rk[Nr + 1][8];
    CubeWord ik[Nr + 1][8];
public:
    static constexpr size_t BLOCK_SIZE = 64;
    Cube(uint8_t const *mk) {
        CubeWord q[8];
        CubeWord t[8];
        memcpy(rk[0], mk, 64);
        for (int r = 0; r < Nr; ++r) {
            FOR(i, 0, i + 1, i < 8, {
                t[i].w = rk[r][i].w ^ CK[r][i].w;
            });
            FOR(i, 0, i + 1, i < 8, {
                q[i].w =
                    TWT_E[0][t[0].b[i]].w ^ TWT_E[1][t[1].b[i]].w ^
                    TWT_E[2][t[2].b[i]].w ^ TWT_E[3][t[3].b[i]].w ^
                    TWT_E[4][t[4].b[i]].w ^ TWT_E[5][t[5].b[i]].w ^
                    TWT_E[6][t[6].b[i]].w ^ TWT_E[7][t[7].b[i]].w;
            });
            FOR(i, 0, i + 1, i < 8, {
                t[i].w =
                    TWT_E[0][q[0].b[i]].w ^ TWT_E[1][q[1].b[i]].w ^
                    TWT_E[2][q[2].b[i]].w ^ TWT_E[3][q[3].b[i]].w ^
                    TWT_E[4][q[4].b[i]].w ^ TWT_E[5][q[5].b[i]].w ^
                    TWT_E[6][q[6].b[i]].w ^ TWT_E[7][q[7].b[i]].w;
            });
            FOR(i, 0, i + 1, i < 8, {
                rk[r + 1][i].w =
                    LUT_E[0][t[0].b[i]].w ^ LUT_E[1][t[1].b[i]].w ^
                    LUT_E[2][t[2].b[i]].w ^ LUT_E[3][t[3].b[i]].w ^
                    LUT_E[4][t[4].b[i]].w ^ LUT_E[5][t[5].b[i]].w ^
                    LUT_E[6][t[6].b[i]].w ^ LUT_E[7][t[7].b[i]].w;
            });
        }
        // Generate decryption round key from encryption round key
        FOR(i, 0, i + 1, i < 8, {
            ik[Nr][i].w = rk[0][i].w;
        });
        for (int r = 1; r < Nr; ++r) {
            FOR(i, 0, i + 1, i < 8, {
                ik[Nr - r][i].w =
                    TWTID[0][rk[r][i].b[0]].w ^ TWTID[1][rk[r][i].b[1]].w ^
                    TWTID[2][rk[r][i].b[2]].w ^ TWTID[3][rk[r][i].b[3]].w ^
                    TWTID[4][rk[r][i].b[4]].w ^ TWTID[5][rk[r][i].b[5]].w ^
                    TWTID[6][rk[r][i].b[6]].w ^ TWTID[7][rk[r][i].b[7]].w;
            });
        }
        FOR(i, 0, i + 1, i < 8, {
            ik[0][i].w = rk[Nr][i].w;
        });
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        CubeWord q[8];
        CubeWord t[8];
        memcpy(q, src, 64);
        FOR(i, 0, i + 1, i < 8, {
            q[i].w ^= rk[0][i].w;
        });
        for (int r = 1; r < Nr; ++r) {
            FOR(i, 0, i + 1, i < 8, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, i + 1, i < 8, {
                q[i].w =
                    TWT_E[0][t[0].b[i]].w ^ TWT_E[1][t[1].b[i]].w ^
                    TWT_E[2][t[2].b[i]].w ^ TWT_E[3][t[3].b[i]].w ^
                    TWT_E[4][t[4].b[i]].w ^ TWT_E[5][t[5].b[i]].w ^
                    TWT_E[6][t[6].b[i]].w ^ TWT_E[7][t[7].b[i]].w ^ rk[r][i].w;
            });
        }
        FOR(i, 0, i + 1, i < 8, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, i + 1, i < 8, {
            q[i].w =
                LUT_E[0][t[0].b[i]].w ^ LUT_E[1][t[1].b[i]].w ^
                LUT_E[2][t[2].b[i]].w ^ LUT_E[3][t[3].b[i]].w ^
                LUT_E[4][t[4].b[i]].w ^ LUT_E[5][t[5].b[i]].w ^
                LUT_E[6][t[6].b[i]].w ^ LUT_E[7][t[7].b[i]].w ^ rk[Nr][i].w;
        });
        memcpy(dst, q, 64);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        CubeWord q[8];
        CubeWord t[8];
        memcpy(q, src, 64);
        FOR(i, 0, i + 1, i < 8, {
            q[i].w ^= ik[0][i].w;
        });
        for (int r = 1; r < Nr; ++r) {
            FOR(i, 0, i + 1, i < 8, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, i + 1, i < 8, {
                q[i].w =
                    TWT_D[0][t[0].b[i]].w ^ TWT_D[1][t[1].b[i]].w ^
                    TWT_D[2][t[2].b[i]].w ^ TWT_D[3][t[3].b[i]].w ^
                    TWT_D[4][t[4].b[i]].w ^ TWT_D[5][t[5].b[i]].w ^
                    TWT_D[6][t[6].b[i]].w ^ TWT_D[7][t[7].b[i]].w ^ ik[r][i].w;
            });
        }
        FOR(i, 0, i + 1, i < 8, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, i + 1, i < 8, {
            q[i].w =
                LUT_D[0][t[0].b[i]].w ^ LUT_D[1][t[1].b[i]].w ^
                LUT_D[2][t[2].b[i]].w ^ LUT_D[3][t[3].b[i]].w ^
                LUT_D[4][t[4].b[i]].w ^ LUT_D[5][t[5].b[i]].w ^
                LUT_D[6][t[6].b[i]].w ^ LUT_D[7][t[7].b[i]].w ^ ik[Nr][i].w;
        });
        memcpy(dst, q, 64);
    }
};
