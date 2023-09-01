#pragma once
#include "../utils.hpp"
union RijndaelWord {
    uint32_t w;
    uint8_t b[4];
};
class RijndaelBase {
protected:
    // Rijndael's Galois Field multiplication
    static constexpr auto multiply = [](uint8_t a, uint8_t b) {
        uint8_t p = 0;
        for (int i = 0; i < 8; i++) {
            p = p ^ (b >> i & 0x01 ? a    : 0x00);
            a = a << 1 ^ (a & 0x80 ? 0x1B : 0x00);
        }
        return p;
    };
    static constexpr auto RC = []() {
        std::array<uint8_t, 22> RC = {};
        uint8_t s = 1;
        for (int i = 1; i < 22; i++) {
            RC[i] = s;
            s = multiply(s, 0x02);
        }
        return RC;
    }();
    static constexpr auto RECIP = []() {
        std::array<uint8_t, 256> RECIP = {};
        uint8_t s = 1, c = 1;
        for (int i = 0; i < 255; i++) {
            RECIP[s] = c;
            s = multiply(s, 0x03);
            c = multiply(c, 0xF6);
        }
        return RECIP;
    }();
    static constexpr auto S_BOX = []() {
        std::array<uint8_t, 256> S_BOX = {};
        for (int i = 0; i < 256; i++) {
            uint8_t x = i;
            x = RECIP[x];
            x = x ^ ROTL(x, 1) ^ ROTL(x, 2) ^ ROTL(x, 3) ^ ROTL(x, 4) ^ 0x63;
            S_BOX[i] = x;
        }
        return S_BOX;
    }();
    static constexpr auto I_BOX = []() {
        std::array<uint8_t, 256> I_BOX = {};
        for (int i = 0; i < 256; i++) {
            uint8_t x = i;
            x = ROTL(x, 1) ^ ROTL(x, 3) ^ ROTL(x, 6) ^ 0x05;
            x = RECIP[x];
            I_BOX[i] = x;
        }
        return I_BOX;
    }();
    static constexpr auto IDENT = []() {
        std::array<uint8_t, 256> IDENT = {};
        for (int i = 0; i < 256; i++) {
            IDENT[i] = i;
        }
        return IDENT;
    }();
    // Generate LUT for SubBytes and MixColumns steps
    static constexpr auto generate_LUT = [](RijndaelWord poly, std::array<uint8_t, 256> const &BOX) {
        std::array<std::array<RijndaelWord, 256>, 4> LUT = {};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 256; j++) {
                uint8_t p = multiply(poly.b[i], BOX[j]);
                for (int k = 0; k < 4; k++) {
                    LUT[k][j].b[(i + k) % 4] = p;
                }
            }
        }
        return LUT;
    };
    static constexpr auto E_MCT = generate_LUT({.b = {0x02, 0x01, 0x01, 0x03}}, IDENT); // LUT for MixColumns step only
    static constexpr auto D_MCT = generate_LUT({.b = {0x0e, 0x09, 0x0d, 0x0b}}, IDENT); // LUT for InvMixColumns step only
    static constexpr auto E_LUT = generate_LUT({.b = {0x02, 0x01, 0x01, 0x03}}, S_BOX); // LUT for SubBytes and MixColumns steps
    static constexpr auto D_LUT = generate_LUT({.b = {0x0e, 0x09, 0x0d, 0x0b}}, I_BOX); // LUT for InvSubBytes and InvMixColumns steps
    static constexpr auto E_SBT = generate_LUT({.b = {0x01, 0x00, 0x00, 0x00}}, S_BOX); // LUT for SubBytes step only
    static constexpr auto D_SBT = generate_LUT({.b = {0x01, 0x00, 0x00, 0x00}}, I_BOX); // LUT for InvSubBytes step only
};
template <int K, int B, int R = std::max(K, B) + 6>
    requires (K >= 4 && K <= 8) && (B >= 4 && B <= 8)
class RijndaelTmpl: public RijndaelBase {
    RijndaelWord rk[R + 1][B];
    RijndaelWord ik[R + 1][B];
public:
    static constexpr size_t BLOCK_SIZE = B * 4;
    static constexpr size_t KEY_SIZE = K * 4;
    RijndaelTmpl(const uint8_t *mk) {
        memcpy((RijndaelWord *)rk, mk, K * 4);
        for (int i = K; i < (R + 1) * B; ++i) {
            RijndaelWord t = ((RijndaelWord *)rk)[i - 1];
            if (i % K == 0) {
                auto x = S_BOX[t.b[1]];
                t.b[1] = S_BOX[t.b[2]];
                t.b[2] = S_BOX[t.b[3]];
                t.b[3] = S_BOX[t.b[0]];
                t.b[0] = x ^ RC[i / K];
            } else if (K > 6 && i % K == 4) {
                t.b[0] = S_BOX[t.b[0]];
                t.b[1] = S_BOX[t.b[1]];
                t.b[2] = S_BOX[t.b[2]];
                t.b[3] = S_BOX[t.b[3]];
            }
            ((RijndaelWord *)rk)[i].w = ((RijndaelWord *)rk)[i - K].w ^ t.w;
        }
        // Generate decryption round key from encryption round key
        FOR(i, 0, i + 1, i < B, {
            ik[R][i].w = rk[0][i].w;
        });
        FOR(i, 0, i + 1, i < B, {
            ik[0][i].w = rk[R][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, i + 1, i < B, {
                ik[R - r][i].w =
                    D_MCT[0][rk[r][i].b[0]].w ^
                    D_MCT[1][rk[r][i].b[1]].w ^
                    D_MCT[2][rk[r][i].b[2]].w ^
                    D_MCT[3][rk[r][i].b[3]].w;
            });
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelWord q[B];
        RijndaelWord t[B];
        memcpy(q, src, B * 4);
        FOR(i, 0, i + 1, i < B, {
            q[i].w ^= rk[0][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, i + 1, i < B, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, i + 1, i < B, {
                q[i].w =
                    E_LUT[0][t[(i + 840) % B].b[0]].w ^
                    E_LUT[1][t[(i + 841) % B].b[1]].w ^
                    E_LUT[2][t[(i + 842) % B].b[2]].w ^
                    E_LUT[3][t[(i + 843) % B].b[3]].w ^ rk[r][i].w;
            });
        }
        FOR(i, 0, i + 1, i < B, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, i + 1, i < B, {
            q[i].w =
                E_SBT[0][t[(i + 840) % B].b[0]].w ^
                E_SBT[1][t[(i + 841) % B].b[1]].w ^
                E_SBT[2][t[(i + 842) % B].b[2]].w ^
                E_SBT[3][t[(i + 843) % B].b[3]].w ^ rk[R][i].w;
        });
        memcpy(dst, q, B * 4);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelWord q[B];
        RijndaelWord t[B];
        memcpy(q, src, B * 4);
        FOR(i, 0, i + 1, i < B, {
            q[i].w ^= ik[0][i].w;
        });
        for (int r = 1; r < R; ++r) {
            FOR(i, 0, i + 1, i < B, {
                t[i].w = q[i].w;
            });
            FOR(i, 0, i + 1, i < B, {
                q[i].w =
                    D_LUT[0][t[(i + 840) % B].b[0]].w ^
                    D_LUT[1][t[(i + 839) % B].b[1]].w ^
                    D_LUT[2][t[(i + 838) % B].b[2]].w ^
                    D_LUT[3][t[(i + 837) % B].b[3]].w ^ ik[r][i].w;
            });
        }
        FOR(i, 0, i + 1, i < B, {
            t[i].w = q[i].w;
        });
        FOR(i, 0, i + 1, i < B, {
            q[i].w =
                D_SBT[0][t[(i + 840) % B].b[0]].w ^
                D_SBT[1][t[(i + 839) % B].b[1]].w ^
                D_SBT[2][t[(i + 838) % B].b[2]].w ^
                D_SBT[3][t[(i + 837) % B].b[3]].w ^ ik[R][i].w;
        });
        memcpy(dst, q, B * 4);
    }
};
using AES128 = RijndaelTmpl<4, 4>;
using AES192 = RijndaelTmpl<6, 4>;
using AES256 = RijndaelTmpl<8, 4>;
