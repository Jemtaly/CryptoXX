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
    // Generate LUT for MixColumns step
    static constexpr auto generate_MCT = [](RijndaelClmn poly) {
        std::array<std::array<RijndaelClmn, 256>, 4> MCT = {};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 256; j++) {
                uint8_t p = multiply(poly.b[i], j);
                for (int k = 0; k < 4; k++) {
                    MCT[k][j].b[(i + k) % 4] = p;
                }
            }
        }
        return MCT;
    };
    static constexpr auto MCT_E = generate_MCT({.b = {0x02, 0x01, 0x01, 0x03}});
    static constexpr auto MCT_D = generate_MCT({.b = {0x0e, 0x09, 0x0d, 0x0b}});
    static void mix_clmns_enc(RijndaelClmn *q) {
        q[0].w = MCT_E[0][q[0].b[0]].w ^ MCT_E[1][q[0].b[1]].w ^ MCT_E[2][q[0].b[2]].w ^ MCT_E[3][q[0].b[3]].w;
        q[1].w = MCT_E[0][q[1].b[0]].w ^ MCT_E[1][q[1].b[1]].w ^ MCT_E[2][q[1].b[2]].w ^ MCT_E[3][q[1].b[3]].w;
        q[2].w = MCT_E[0][q[2].b[0]].w ^ MCT_E[1][q[2].b[1]].w ^ MCT_E[2][q[2].b[2]].w ^ MCT_E[3][q[2].b[3]].w;
        q[3].w = MCT_E[0][q[3].b[0]].w ^ MCT_E[1][q[3].b[1]].w ^ MCT_E[2][q[3].b[2]].w ^ MCT_E[3][q[3].b[3]].w;
    }
    static void mix_clmns_dec(RijndaelClmn *q) {
        q[0].w = MCT_D[0][q[0].b[0]].w ^ MCT_D[1][q[0].b[1]].w ^ MCT_D[2][q[0].b[2]].w ^ MCT_D[3][q[0].b[3]].w;
        q[1].w = MCT_D[0][q[1].b[0]].w ^ MCT_D[1][q[1].b[1]].w ^ MCT_D[2][q[1].b[2]].w ^ MCT_D[3][q[1].b[3]].w;
        q[2].w = MCT_D[0][q[2].b[0]].w ^ MCT_D[1][q[2].b[1]].w ^ MCT_D[2][q[2].b[2]].w ^ MCT_D[3][q[2].b[3]].w;
        q[3].w = MCT_D[0][q[3].b[0]].w ^ MCT_D[1][q[3].b[1]].w ^ MCT_D[2][q[3].b[2]].w ^ MCT_D[3][q[3].b[3]].w;
    }
    static void sub_shift_enc(RijndaelClmn *q) {
        RijndaelClmn t[4];
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        q[0].b[0] = S_BOX[t[0].b[0]]; q[0].b[1] = S_BOX[t[1].b[1]]; q[0].b[2] = S_BOX[t[2].b[2]]; q[0].b[3] = S_BOX[t[3].b[3]];
        q[1].b[0] = S_BOX[t[1].b[0]]; q[1].b[1] = S_BOX[t[2].b[1]]; q[1].b[2] = S_BOX[t[3].b[2]]; q[1].b[3] = S_BOX[t[0].b[3]];
        q[2].b[0] = S_BOX[t[2].b[0]]; q[2].b[1] = S_BOX[t[3].b[1]]; q[2].b[2] = S_BOX[t[0].b[2]]; q[2].b[3] = S_BOX[t[1].b[3]];
        q[3].b[0] = S_BOX[t[3].b[0]]; q[3].b[1] = S_BOX[t[0].b[1]]; q[3].b[2] = S_BOX[t[1].b[2]]; q[3].b[3] = S_BOX[t[2].b[3]];
    }
    static void sub_shift_dec(RijndaelClmn *q) {
        RijndaelClmn t[4];
        t[0].w = q[0].w;
        t[1].w = q[1].w;
        t[2].w = q[2].w;
        t[3].w = q[3].w;
        q[0].b[0] = I_BOX[t[0].b[0]]; q[0].b[1] = I_BOX[t[3].b[1]]; q[0].b[2] = I_BOX[t[2].b[2]]; q[0].b[3] = I_BOX[t[1].b[3]];
        q[1].b[0] = I_BOX[t[1].b[0]]; q[1].b[1] = I_BOX[t[0].b[1]]; q[1].b[2] = I_BOX[t[3].b[2]]; q[1].b[3] = I_BOX[t[2].b[3]];
        q[2].b[0] = I_BOX[t[2].b[0]]; q[2].b[1] = I_BOX[t[1].b[1]]; q[2].b[2] = I_BOX[t[0].b[2]]; q[2].b[3] = I_BOX[t[3].b[3]];
        q[3].b[0] = I_BOX[t[3].b[0]]; q[3].b[1] = I_BOX[t[2].b[1]]; q[3].b[2] = I_BOX[t[1].b[2]]; q[3].b[3] = I_BOX[t[0].b[3]];
    }
    static void add_round_key(RijndaelClmn *q, RijndaelClmn const *k) {
        q[0].w ^= k[0].w;
        q[1].w ^= k[1].w;
        q[2].w ^= k[2].w;
        q[3].w ^= k[3].w;
    }
};
template <int K, int B, int R = std::max(K, B) + 6>
requires (K == 4 || K == 6 || K == 8) && (B == 4)
class RijndaelTmpl: public RijndaelBase {
protected:
    RijndaelClmn rk[R + 1][B];
    RijndaelTmpl() = default; // not instantiable
public:
    static constexpr size_t BLOCK_SIZE = B * 4;
    RijndaelTmpl(const uint8_t *mk) {
        memcpy((RijndaelClmn *)rk, mk, K * 4);
        for (int i = K; i < (R + 1) * B; ++i) {
            RijndaelClmn t = ((RijndaelClmn *)rk)[i - 1];
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
            ((RijndaelClmn *)rk)[i].w = ((RijndaelClmn *)rk)[i - K].w ^ t.w;
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelClmn q[4];
        memcpy(q, src, 16);
        int round = 0;
        add_round_key(q, rk[round]);
        while (++round < R) {
            sub_shift_enc(q);
            mix_clmns_enc(q);
            add_round_key(q, rk[round]);
        }
        sub_shift_enc(q);
        add_round_key(q, rk[round]);
        memcpy(dst, q, 16);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        RijndaelClmn q[4];
        memcpy(q, src, 16);
        int round = R;
        add_round_key(q, rk[round]);
        while (--round > 0) {
            sub_shift_dec(q);
            add_round_key(q, rk[round]);
            mix_clmns_dec(q);
        }
        sub_shift_dec(q);
        add_round_key(q, rk[round]);
        memcpy(dst, q, 16);
    }
};
using AES128 = RijndaelTmpl<4, 4>;
using AES192 = RijndaelTmpl<6, 4>;
using AES256 = RijndaelTmpl<8, 4>;
