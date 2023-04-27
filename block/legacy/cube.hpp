#pragma once
#include "block.hpp"
#define SB_SC_MC_INV(t) (                     \
    MCTID[0][t.b[0]].w ^ MCTID[1][t.b[1]].w ^ \
    MCTID[2][t.b[2]].w ^ MCTID[3][t.b[3]].w ^ \
    MCTID[4][t.b[4]].w ^ MCTID[5][t.b[5]].w ^ \
    MCTID[6][t.b[6]].w ^ MCTID[7][t.b[7]].w   \
)
#define SB_SC_MC_ENC(t, j) (                                            \
    MCT_E[0][t[ j         ].b[0]].w ^ MCT_E[1][t[(j + 1) % 8].b[1]].w ^ \
    MCT_E[2][t[(j + 2) % 8].b[2]].w ^ MCT_E[3][t[(j + 3) % 8].b[3]].w ^ \
    MCT_E[4][t[(j + 4) % 8].b[4]].w ^ MCT_E[5][t[(j + 5) % 8].b[5]].w ^ \
    MCT_E[6][t[(j + 6) % 8].b[6]].w ^ MCT_E[7][t[(j + 7) % 8].b[7]].w   \
)
#define SB_SC_MC_DEC(t, j) (                                            \
    MCT_D[0][t[ j         ].b[0]].w ^ MCT_D[1][t[(j + 7) % 8].b[1]].w ^ \
    MCT_D[2][t[(j + 6) % 8].b[2]].w ^ MCT_D[3][t[(j + 5) % 8].b[3]].w ^ \
    MCT_D[4][t[(j + 4) % 8].b[4]].w ^ MCT_D[5][t[(j + 3) % 8].b[5]].w ^ \
    MCT_D[6][t[(j + 2) % 8].b[6]].w ^ MCT_D[7][t[(j + 1) % 8].b[7]].w   \
)
#define SB_SC_KC_ENC(t, j) (                                            \
    KCT_E[0][t[ j         ].b[0]].w ^ KCT_E[1][t[(j + 1) % 8].b[1]].w ^ \
    KCT_E[2][t[(j + 2) % 8].b[2]].w ^ KCT_E[3][t[(j + 3) % 8].b[3]].w ^ \
    KCT_E[4][t[(j + 4) % 8].b[4]].w ^ KCT_E[5][t[(j + 5) % 8].b[5]].w ^ \
    KCT_E[6][t[(j + 6) % 8].b[6]].w ^ KCT_E[7][t[(j + 7) % 8].b[7]].w   \
)
#define SB_SC_KC_DEC(t, j) (                                            \
    KCT_D[0][t[ j         ].b[0]].w ^ KCT_D[1][t[(j + 7) % 8].b[1]].w ^ \
    KCT_D[2][t[(j + 6) % 8].b[2]].w ^ KCT_D[3][t[(j + 5) % 8].b[3]].w ^ \
    KCT_D[4][t[(j + 4) % 8].b[4]].w ^ KCT_D[5][t[(j + 3) % 8].b[5]].w ^ \
    KCT_D[6][t[(j + 2) % 8].b[6]].w ^ KCT_D[7][t[(j + 1) % 8].b[7]].w   \
)
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
    static constexpr auto IDENT = []() {
        std::array<uint8_t, 256> IDENT = {};
        for (int i = 0; i < 256; i++) {
            IDENT[i] = i;
        }
        return IDENT;
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
    static constexpr auto MCTID = generate_MCT(IDENT);
    static constexpr auto MCT_E = generate_MCT(E_BOX);
    static constexpr auto MCT_D = generate_MCT(D_BOX);
    static constexpr auto KCT_E = generate_KCT(E_BOX);
    static constexpr auto KCT_D = generate_KCT(D_BOX);
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
template <int R = 10>
class CubeTmpl: public CubeBase {
    CubeSec rk[R + 1][8];
    CubeSec ik[R + 1][8];
public:
    static constexpr size_t BLOCK_SIZE = 64;
    CubeTmpl(const uint8_t *mk) {
        memcpy(rk[0], mk, 64);
        for (int r = 0; r < R; ++r) {
            rk[r + 1][0].w = SB_SC_MC_ENC(rk[r], 0) ^ CK[r][0].w;
            rk[r + 1][1].w = SB_SC_MC_ENC(rk[r], 1) ^ CK[r][1].w;
            rk[r + 1][2].w = SB_SC_MC_ENC(rk[r], 2) ^ CK[r][2].w;
            rk[r + 1][3].w = SB_SC_MC_ENC(rk[r], 3) ^ CK[r][3].w;
            rk[r + 1][4].w = SB_SC_MC_ENC(rk[r], 4) ^ CK[r][4].w;
            rk[r + 1][5].w = SB_SC_MC_ENC(rk[r], 5) ^ CK[r][5].w;
            rk[r + 1][6].w = SB_SC_MC_ENC(rk[r], 6) ^ CK[r][6].w;
            rk[r + 1][7].w = SB_SC_MC_ENC(rk[r], 7) ^ CK[r][7].w;
        }
        // Generate decryption round key from encryption round key
        ik[R][0].w = rk[0][0].w;
        ik[R][1].w = rk[0][1].w;
        ik[R][2].w = rk[0][2].w;
        ik[R][3].w = rk[0][3].w;
        ik[R][4].w = rk[0][4].w;
        ik[R][5].w = rk[0][5].w;
        ik[R][6].w = rk[0][6].w;
        ik[R][7].w = rk[0][7].w;
        for (int r = 1; r < R; ++r) {
            ik[R - r][0].w = SB_SC_MC_INV(rk[R][0]);
            ik[R - r][1].w = SB_SC_MC_INV(rk[R][1]);
            ik[R - r][2].w = SB_SC_MC_INV(rk[R][2]);
            ik[R - r][3].w = SB_SC_MC_INV(rk[R][3]);
            ik[R - r][4].w = SB_SC_MC_INV(rk[R][4]);
            ik[R - r][5].w = SB_SC_MC_INV(rk[R][5]);
            ik[R - r][6].w = SB_SC_MC_INV(rk[R][6]);
            ik[R - r][7].w = SB_SC_MC_INV(rk[R][7]);
        }
        ik[0][0].w = rk[R][0].w;
        ik[0][1].w = rk[R][1].w;
        ik[0][2].w = rk[R][2].w;
        ik[0][3].w = rk[R][3].w;
        ik[0][4].w = rk[R][4].w;
        ik[0][5].w = rk[R][5].w;
        ik[0][6].w = rk[R][6].w;
        ik[0][7].w = rk[R][7].w;
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        CubeSec q[8];
        CubeSec t[8];
        memcpy(q, src, 64);
        q[0].w ^= rk[0][0].w;
        q[1].w ^= rk[0][1].w;
        q[2].w ^= rk[0][2].w;
        q[3].w ^= rk[0][3].w;
        q[4].w ^= rk[0][4].w;
        q[5].w ^= rk[0][5].w;
        q[6].w ^= rk[0][6].w;
        q[7].w ^= rk[0][7].w;
        for (int r = 1; r < R; ++r) {
            t[0].w = q[0].w;
            t[1].w = q[1].w;
            t[2].w = q[2].w;
            t[3].w = q[3].w;
            t[4].w = q[4].w;
            t[5].w = q[5].w;
            t[6].w = q[6].w;
            t[7].w = q[7].w;
            q[0].w = SB_SC_MC_ENC(t, 0) ^ rk[r][0].w;
            q[1].w = SB_SC_MC_ENC(t, 1) ^ rk[r][1].w;
            q[2].w = SB_SC_MC_ENC(t, 2) ^ rk[r][2].w;
            q[3].w = SB_SC_MC_ENC(t, 3) ^ rk[r][3].w;
            q[4].w = SB_SC_MC_ENC(t, 4) ^ rk[r][4].w;
            q[5].w = SB_SC_MC_ENC(t, 5) ^ rk[r][5].w;
            q[6].w = SB_SC_MC_ENC(t, 6) ^ rk[r][6].w;
            q[7].w = SB_SC_MC_ENC(t, 7) ^ rk[r][7].w;
        }
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        t[4].w = q[4].w;
        t[5].w = q[5].w;
        t[6].w = q[6].w;
        t[7].w = q[7].w;
        q[0].w = SB_SC_KC_ENC(t, 0) ^ rk[R][0].w;
        q[1].w = SB_SC_KC_ENC(t, 1) ^ rk[R][1].w;
        q[2].w = SB_SC_KC_ENC(t, 2) ^ rk[R][2].w;
        q[3].w = SB_SC_KC_ENC(t, 3) ^ rk[R][3].w;
        q[4].w = SB_SC_KC_ENC(t, 4) ^ rk[R][4].w;
        q[5].w = SB_SC_KC_ENC(t, 5) ^ rk[R][5].w;
        q[6].w = SB_SC_KC_ENC(t, 6) ^ rk[R][6].w;
        q[7].w = SB_SC_KC_ENC(t, 7) ^ rk[R][7].w;
        memcpy(dst, q, 64);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        CubeSec q[8];
        CubeSec t[8];
        memcpy(q, src, 64);
        q[0].w ^= ik[0][0].w;
        q[1].w ^= ik[0][1].w;
        q[2].w ^= ik[0][2].w;
        q[3].w ^= ik[0][3].w;
        q[4].w ^= ik[0][4].w;
        q[5].w ^= ik[0][5].w;
        q[6].w ^= ik[0][6].w;
        q[7].w ^= ik[0][7].w;
        for (int r = 1; r < R; ++r) {
            t[0].w = q[0].w;
            t[1].w = q[1].w;
            t[2].w = q[2].w;
            t[3].w = q[3].w;
            t[4].w = q[4].w;
            t[5].w = q[5].w;
            t[6].w = q[6].w;
            t[7].w = q[7].w;
            q[0].w = SB_SC_MC_DEC(t, 0) ^ ik[r][0].w;
            q[1].w = SB_SC_MC_DEC(t, 1) ^ ik[r][1].w;
            q[2].w = SB_SC_MC_DEC(t, 2) ^ ik[r][2].w;
            q[3].w = SB_SC_MC_DEC(t, 3) ^ ik[r][3].w;
            q[4].w = SB_SC_MC_DEC(t, 4) ^ ik[r][4].w;
            q[5].w = SB_SC_MC_DEC(t, 5) ^ ik[r][5].w;
            q[6].w = SB_SC_MC_DEC(t, 6) ^ ik[r][6].w;
            q[7].w = SB_SC_MC_DEC(t, 7) ^ ik[r][7].w;
        }
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        t[4].w = q[4].w;
        t[5].w = q[5].w;
        t[6].w = q[6].w;
        t[7].w = q[7].w;
        q[0].w = SB_SC_KC_DEC(t, 0) ^ ik[R][0].w;
        q[1].w = SB_SC_KC_DEC(t, 1) ^ ik[R][1].w;
        q[2].w = SB_SC_KC_DEC(t, 2) ^ ik[R][2].w;
        q[3].w = SB_SC_KC_DEC(t, 3) ^ ik[R][3].w;
        q[4].w = SB_SC_KC_DEC(t, 4) ^ ik[R][4].w;
        q[5].w = SB_SC_KC_DEC(t, 5) ^ ik[R][5].w;
        q[6].w = SB_SC_KC_DEC(t, 6) ^ ik[R][6].w;
        q[7].w = SB_SC_KC_DEC(t, 7) ^ ik[R][7].w;
        memcpy(dst, q, 64);
    }
};
using Cube = CubeTmpl<>;
