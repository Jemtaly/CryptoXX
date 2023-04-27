#pragma once
#include "block.hpp"
#define MOD(x, y) (((x) % (y) + (y)) % (y))
union CubeSec {
    uint64_t w;
    uint8_t b[8];
};
class CubeBase {
protected:
    // Galois Field multiplication
    static constexpr auto multiply = [](uint8_t a, uint8_t b) {
        uint8_t p = 0;
        for (int i = 0; i < 8; i++) {
            p = p ^ (b >> i & 0x01 ? a    : 0x00);
            a = a << 1 ^ (a & 0x80 ? 0x1B : 0x00);
        }
        return p;
    };
    static constexpr auto RECIP = []() {
        std::array<uint8_t, 256> RECIP = {};
        uint8_t s = 1, c = 1;
        for (int i = 0; i < 255; i++) {
            s = multiply(s, 0x03);
            c = multiply(c, 0xF6);
            RECIP[s] = c;
        }
        return RECIP;
    }();
    static constexpr auto IDBOX = []() {
        std::array<uint8_t, 256> IDBOX = {};
        for (int i = 0; i < 256; i++) {
            IDBOX[i] = i;
        }
        return IDBOX;
    }();
    static constexpr auto E_BOX = []() {
        std::array<uint8_t, 256> E_BOX = {};
        for (int i = 0; i < 256; i++) {
            uint8_t x = i;
            x = RECIP[x];
            x = x ^ ROTL(x, 1) ^ ROTL(x, 2) ^ ROTL(x, 3) ^ ROTL(x, 4) ^ 0x63;
            E_BOX[i] = x;
        }
        return E_BOX;
    }();
    static constexpr auto D_BOX = []() {
        std::array<uint8_t, 256> D_BOX = {};
        for (int i = 0; i < 256; i++) {
            uint8_t x = i;
            x = ROTL(x, 1) ^ ROTL(x, 3) ^ ROTL(x, 6) ^ 0x05;
            x = RECIP[x];
            D_BOX[i] = x;
        }
        return D_BOX;
    }();
    static constexpr auto generate_MCT = [](std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<CubeSec, 256>, 8> MCT_S = {};
        for (int j = 0; j < 256; j++) {
            uint8_t p = S_BOX[j];
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    MCT_S[k][j].b[i] = (p >> i & 1) << k;
                }
            }
        }
        return MCT_S;
    };
    static constexpr auto generate_KCT = [](std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<CubeSec, 256>, 8> KCT_S = {};
        for (int j = 0; j < 256; j++) {
            uint8_t p = S_BOX[j];
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    KCT_S[k][j].b[i] = k == i ? p : 0;
                }
            }
        }
        return KCT_S;
    };
    static constexpr auto MCTID = generate_MCT(IDBOX); // MixColumns
    static constexpr auto MCT_E = generate_MCT(E_BOX); // MixColumns and EncSubBytes
    static constexpr auto MCT_D = generate_MCT(D_BOX); // MixColumns and DecSubBytes
    static constexpr auto KCT_E = generate_KCT(E_BOX); // EncSubBytes
    static constexpr auto KCT_D = generate_KCT(D_BOX); // DecSubBytes
    static constexpr auto CK = []() {
        std::array<std::array<CubeSec, 8>, 10> CK = {};
        for (int r = 0; r < 10; r++) {
            for (int i = 0; i < 8; i++) {
                for (int k = 0; k < 8; k++) {
                    CK[r][i].b[k] = r;
                }
            }
        }
        return CK;
    }();
};
template <int K, int R = 10>
    requires (K >= 4 && K <= 8)
class CubeTmpl: public CubeBase {
    CubeSec rk[R + 1][K];
    CubeSec ik[R + 1][K];
public:
    static constexpr size_t BLOCK_SIZE = K * 8;
    CubeTmpl(uint8_t const *mk) {
        memcpy(rk[0], mk, K * 8);
        for (int r = 0; r < R; ++r) {
            FOR(i, 0, 1, <, K, {
                rk[r + 1][i].w =
                    MCT_E[0][rk[r][    i        ].b[0]].w ^ MCT_E[1][rk[r][MOD(i + 1, K)].b[1]].w ^
                    MCT_E[2][rk[r][MOD(i + 2, K)].b[2]].w ^ MCT_E[3][rk[r][MOD(i + 3, K)].b[3]].w ^
                    MCT_E[4][rk[r][MOD(i + 4, K)].b[4]].w ^ MCT_E[5][rk[r][MOD(i + 5, K)].b[5]].w ^
                    MCT_E[6][rk[r][MOD(i + 6, K)].b[6]].w ^ MCT_E[7][rk[r][MOD(i + 7, K)].b[7]].w ^ CK[r][i].w;
            });
        }
        // Generate decryption round key from encryption round key
        FOR(i, 0, 1, <, K, {
            ik[R][i].w = rk[0][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, 1, <, K, {
                ik[R - r][i].w =
                    MCTID[0][rk[r][i].b[0]].w ^ MCTID[1][rk[r][i].b[1]].w ^
                    MCTID[2][rk[r][i].b[2]].w ^ MCTID[3][rk[r][i].b[3]].w ^
                    MCTID[4][rk[r][i].b[4]].w ^ MCTID[5][rk[r][i].b[5]].w ^
                    MCTID[6][rk[r][i].b[6]].w ^ MCTID[7][rk[r][i].b[7]].w;
            });
        }
        FOR(i, 0, 1, <, K, {
            ik[0][i].w = rk[R][i].w;
        });
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        CubeSec q[8];
        CubeSec t[8];
        memcpy(q, src, 64);
        FOR(i, 0, 1, <, K, {
            q[i].w ^= rk[0][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, 1, <, K, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, 1, <, K, {
                q[i].w =
                    MCT_E[0][t[    i        ].b[0]].w ^ MCT_E[1][t[MOD(i + 1, K)].b[1]].w ^
                    MCT_E[2][t[MOD(i + 2, K)].b[2]].w ^ MCT_E[3][t[MOD(i + 3, K)].b[3]].w ^
                    MCT_E[4][t[MOD(i + 4, K)].b[4]].w ^ MCT_E[5][t[MOD(i + 5, K)].b[5]].w ^
                    MCT_E[6][t[MOD(i + 6, K)].b[6]].w ^ MCT_E[7][t[MOD(i + 7, K)].b[7]].w ^ rk[r][i].w;
            });
        }
        FOR(i, 0, 1, <, K, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, 1, <, K, {
            q[i].w =
                KCT_E[0][t[    i        ].b[0]].w ^ KCT_E[1][t[MOD(i + 1, K)].b[1]].w ^
                KCT_E[2][t[MOD(i + 2, K)].b[2]].w ^ KCT_E[3][t[MOD(i + 3, K)].b[3]].w ^
                KCT_E[4][t[MOD(i + 4, K)].b[4]].w ^ KCT_E[5][t[MOD(i + 5, K)].b[5]].w ^
                KCT_E[6][t[MOD(i + 6, K)].b[6]].w ^ KCT_E[7][t[MOD(i + 7, K)].b[7]].w ^ rk[R][i].w;
        });
        memcpy(dst, q, 64);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        CubeSec q[8];
        CubeSec t[8];
        memcpy(q, src, 64);
        FOR(i, 0, 1, <, K, {
            q[i].w ^= ik[0][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, 1, <, K, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, 1, <, K, {
                q[i].w =
                    MCT_D[0][t[    i        ].b[0]].w ^ MCT_D[1][t[MOD(i - 1, K)].b[1]].w ^
                    MCT_D[2][t[MOD(i - 2, K)].b[2]].w ^ MCT_D[3][t[MOD(i - 3, K)].b[3]].w ^
                    MCT_D[4][t[MOD(i - 4, K)].b[4]].w ^ MCT_D[5][t[MOD(i - 5, K)].b[5]].w ^
                    MCT_D[6][t[MOD(i - 6, K)].b[6]].w ^ MCT_D[7][t[MOD(i - 7, K)].b[7]].w ^ ik[r][i].w;
            });
        }
        FOR(i, 0, 1, <, K, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, 1, <, K, {
            q[i].w =
                KCT_D[0][t[    i        ].b[0]].w ^ KCT_D[1][t[MOD(i - 1, K)].b[1]].w ^
                KCT_D[2][t[MOD(i - 2, K)].b[2]].w ^ KCT_D[3][t[MOD(i - 3, K)].b[3]].w ^
                KCT_D[4][t[MOD(i - 4, K)].b[4]].w ^ KCT_D[5][t[MOD(i - 5, K)].b[5]].w ^
                KCT_D[6][t[MOD(i - 6, K)].b[6]].w ^ KCT_D[7][t[MOD(i - 7, K)].b[7]].w ^ ik[R][i].w;
        });
        memcpy(dst, q, 64);
    }
};
using Cube256 = CubeTmpl<4>;
using Cube384 = CubeTmpl<6>;
using Cube512 = CubeTmpl<8>;
