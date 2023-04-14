#pragma once
#include <array>
#include "block.hpp"
union RijndaelClmn {
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
        std::array<uint8_t, 11> RC = {0, 1};
        for (int i = 2; i < 11; i++) {
            RC[i] = multiply(RC[i - 1], 0x02);
        }
        return RC;
    }();
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
    static constexpr auto generate_LUT = [](RijndaelClmn poly, std::array<uint8_t, 256> const &S_BOX) {
        std::array<std::array<RijndaelClmn, 256>, 4> LUT = {};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 256; j++) {
                uint8_t p = multiply(poly.b[i], S_BOX[j]);
                for (int k = 0; k < 4; k++) {
                    LUT[k][j].b[(i + k) % 4] = p;
                }
            }
        }
        return LUT;
    };
    static constexpr auto MCT_D = generate_LUT({.b = {0x0e, 0x09, 0x0d, 0x0b}}, IDENT); // LUT for InvMixColumns step only
    static constexpr auto LUT_E = generate_LUT({.b = {0x02, 0x01, 0x01, 0x03}}, S_BOX); // LUT for SubBytes and MixColumns steps
    static constexpr auto LUT_D = generate_LUT({.b = {0x0e, 0x09, 0x0d, 0x0b}}, I_BOX); // LUT for InvSubBytes and InvMixColumns steps
    static constexpr auto S_EXT = generate_LUT({.b = {0x01, 0x00, 0x00, 0x00}}, S_BOX); // LUT for SubBytes step only
    static constexpr auto I_EXT = generate_LUT({.b = {0x01, 0x00, 0x00, 0x00}}, I_BOX); // LUT for InvSubBytes step only
};
template <int K, int B, int R = std::max(K, B) + 6>
requires (K == 4 || K == 6 || K == 8) && (B == 4)
class RijndaelTmpl: public RijndaelBase {
protected:
    RijndaelClmn rk[R + 1][B];
    RijndaelClmn ik[R + 1][B];
    RijndaelTmpl() = default; // not instantiable
public:
    static constexpr size_t BLOCK_SIZE = B * 4;
    RijndaelTmpl(const uint8_t *mk) {
        memcpy((RijndaelClmn *)rk, mk, K * 4);
        for (int i = K; i < (R + 1) * B; ++i) {
            RijndaelClmn t = ((RijndaelClmn *)rk)[i - 1];
            if (i % K == 0) {
                t.w = S_EXT[0][t.b[1]].w ^ S_EXT[1][t.b[2]].w ^ S_EXT[2][t.b[3]].w ^ S_EXT[3][t.b[0]].w ^ RC[i / K];
            } else if (K > 6 && i % K == 4) {
                t.w = S_EXT[0][t.b[0]].w ^ S_EXT[1][t.b[1]].w ^ S_EXT[2][t.b[2]].w ^ S_EXT[3][t.b[3]].w;
            }
            ((RijndaelClmn *)rk)[i].w = ((RijndaelClmn *)rk)[i - K].w ^ t.w;
        }
        ik[R][0].w = rk[0][0].w;
        ik[R][1].w = rk[0][1].w;
        ik[R][2].w = rk[0][2].w;
        ik[R][3].w = rk[0][3].w;
        for (int r = 1; r < R; ++r) {
            ik[R - r][0].w = MCT_D[0][rk[r][0].b[0]].w ^ MCT_D[1][rk[r][0].b[1]].w ^ MCT_D[2][rk[r][0].b[2]].w ^ MCT_D[3][rk[r][0].b[3]].w;
            ik[R - r][1].w = MCT_D[0][rk[r][1].b[0]].w ^ MCT_D[1][rk[r][1].b[1]].w ^ MCT_D[2][rk[r][1].b[2]].w ^ MCT_D[3][rk[r][1].b[3]].w;
            ik[R - r][2].w = MCT_D[0][rk[r][2].b[0]].w ^ MCT_D[1][rk[r][2].b[1]].w ^ MCT_D[2][rk[r][2].b[2]].w ^ MCT_D[3][rk[r][2].b[3]].w;
            ik[R - r][3].w = MCT_D[0][rk[r][3].b[0]].w ^ MCT_D[1][rk[r][3].b[1]].w ^ MCT_D[2][rk[r][3].b[2]].w ^ MCT_D[3][rk[r][3].b[3]].w;
        }
        ik[0][0].w = rk[R][0].w;
        ik[0][1].w = rk[R][1].w;
        ik[0][2].w = rk[R][2].w;
        ik[0][3].w = rk[R][3].w;
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelClmn q[4];
        RijndaelClmn t[4];
        memcpy(q, src, 16);
        q[0].w ^= rk[0][0].w;
        q[1].w ^= rk[0][1].w;
        q[2].w ^= rk[0][2].w;
        q[3].w ^= rk[0][3].w;
        for (int r = 1; r < R; ++r) {
            t[0].w = q[0].w;
            t[1].w = q[1].w;
            t[2].w = q[2].w;
            t[3].w = q[3].w;
            q[0].w = LUT_E[0][t[0].b[0]].w ^ LUT_E[1][t[1].b[1]].w ^ LUT_E[2][t[2].b[2]].w ^ LUT_E[3][t[3].b[3]].w ^ rk[r][0].w;
            q[1].w = LUT_E[0][t[1].b[0]].w ^ LUT_E[1][t[2].b[1]].w ^ LUT_E[2][t[3].b[2]].w ^ LUT_E[3][t[0].b[3]].w ^ rk[r][1].w;
            q[2].w = LUT_E[0][t[2].b[0]].w ^ LUT_E[1][t[3].b[1]].w ^ LUT_E[2][t[0].b[2]].w ^ LUT_E[3][t[1].b[3]].w ^ rk[r][2].w;
            q[3].w = LUT_E[0][t[3].b[0]].w ^ LUT_E[1][t[0].b[1]].w ^ LUT_E[2][t[1].b[2]].w ^ LUT_E[3][t[2].b[3]].w ^ rk[r][3].w;
        }
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        q[0].w = S_EXT[0][t[0].b[0]].w ^ S_EXT[1][t[1].b[1]].w ^ S_EXT[2][t[2].b[2]].w ^ S_EXT[3][t[3].b[3]].w ^ rk[R][0].w;
        q[1].w = S_EXT[0][t[1].b[0]].w ^ S_EXT[1][t[2].b[1]].w ^ S_EXT[2][t[3].b[2]].w ^ S_EXT[3][t[0].b[3]].w ^ rk[R][1].w;
        q[2].w = S_EXT[0][t[2].b[0]].w ^ S_EXT[1][t[3].b[1]].w ^ S_EXT[2][t[0].b[2]].w ^ S_EXT[3][t[1].b[3]].w ^ rk[R][2].w;
        q[3].w = S_EXT[0][t[3].b[0]].w ^ S_EXT[1][t[0].b[1]].w ^ S_EXT[2][t[1].b[2]].w ^ S_EXT[3][t[2].b[3]].w ^ rk[R][3].w;
        memcpy(dst, q, 16);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelClmn q[4];
        RijndaelClmn t[4];
        memcpy(q, src, 16);
        q[0].w ^= ik[0][0].w;
        q[1].w ^= ik[0][1].w;
        q[2].w ^= ik[0][2].w;
        q[3].w ^= ik[0][3].w;
        for (int r = 1; r < R; ++r) {
            t[0].w = q[0].w;
            t[1].w = q[1].w;
            t[2].w = q[2].w;
            t[3].w = q[3].w;
            q[0].w = LUT_D[0][t[0].b[0]].w ^ LUT_D[1][t[3].b[1]].w ^ LUT_D[2][t[2].b[2]].w ^ LUT_D[3][t[1].b[3]].w ^ ik[r][0].w;
            q[1].w = LUT_D[0][t[1].b[0]].w ^ LUT_D[1][t[0].b[1]].w ^ LUT_D[2][t[3].b[2]].w ^ LUT_D[3][t[2].b[3]].w ^ ik[r][1].w;
            q[2].w = LUT_D[0][t[2].b[0]].w ^ LUT_D[1][t[1].b[1]].w ^ LUT_D[2][t[0].b[2]].w ^ LUT_D[3][t[3].b[3]].w ^ ik[r][2].w;
            q[3].w = LUT_D[0][t[3].b[0]].w ^ LUT_D[1][t[2].b[1]].w ^ LUT_D[2][t[1].b[2]].w ^ LUT_D[3][t[0].b[3]].w ^ ik[r][3].w;
        }
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        q[0].w = I_EXT[0][t[0].b[0]].w ^ I_EXT[1][t[3].b[1]].w ^ I_EXT[2][t[2].b[2]].w ^ I_EXT[3][t[1].b[3]].w ^ ik[R][0].w;
        q[1].w = I_EXT[0][t[1].b[0]].w ^ I_EXT[1][t[0].b[1]].w ^ I_EXT[2][t[3].b[2]].w ^ I_EXT[3][t[2].b[3]].w ^ ik[R][1].w;
        q[2].w = I_EXT[0][t[2].b[0]].w ^ I_EXT[1][t[1].b[1]].w ^ I_EXT[2][t[0].b[2]].w ^ I_EXT[3][t[3].b[3]].w ^ ik[R][2].w;
        q[3].w = I_EXT[0][t[3].b[0]].w ^ I_EXT[1][t[2].b[1]].w ^ I_EXT[2][t[1].b[2]].w ^ I_EXT[3][t[0].b[3]].w ^ ik[R][3].w;
        memcpy(dst, q, 16);
    }
};
using AES128 = RijndaelTmpl<4, 4>;
using AES192 = RijndaelTmpl<6, 4>;
using AES256 = RijndaelTmpl<8, 4>;
