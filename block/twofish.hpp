#pragma once
#include "block.hpp"
#include <array>
#define G0_FUN(x) (mks[0][x & 0xff] ^ mks[1][x >> 8 & 0xff] ^ mks[2][x >> 16 & 0xff] ^ mks[3][x >> 24])
#define G1_FUN(x) (mks[1][x & 0xff] ^ mks[2][x >> 8 & 0xff] ^ mks[3][x >> 16 & 0xff] ^ mks[0][x >> 24])
#define ENCROUND(n, a, b, c, d, x, y) {      \
    x = G0_FUN(a);                           \
    y = G1_FUN(b);                           \
    x += y;                                  \
    y += x;                                  \
    c = ROTR(c ^ (x + rnk[2 * (n) + 8]), 1); \
    d = ROTL(d, 1) ^ (y + rnk[2 * (n) + 9]); \
}
#define DECROUND(n, a, b, c, d, x, y) {      \
    x = G0_FUN(a);                           \
    y = G1_FUN(b);                           \
    x += y;                                  \
    y += x;                                  \
    c = ROTL(c, 1) ^ (x + rnk[2 * (n) + 8]); \
    d = ROTR(d ^ (y + rnk[2 * (n) + 9]), 1); \
}
#define ENCCYCLE(N, a, b, c, d, x, y) {      \
    ENCROUND(2 * (N)    , a, b, c, d, x, y); \
    ENCROUND(2 * (N) + 1, c, d, a, b, x, y); \
}
#define DECCYCLE(N, a, b, c, d, x, y) {      \
    DECROUND(2 * (N) + 1, c, d, a, b, x, y); \
    DECROUND(2 * (N)    , a, b, c, d, x, y); \
}
class TwofishBase {
protected:
    static constexpr uint8_t ROR4[16] = {
        0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15,
    };
    static constexpr uint8_t ASHX[16] = {
        0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12, 5, 14, 7,
    };
    static constexpr uint8_t QT[4][2][16] = {
        8, 1, 7, 13, 6, 15, 3, 2, 0, 11, 5, 9, 14, 12, 10, 4,
        2, 8, 11, 13, 15, 7, 6, 14, 3, 1, 9, 4, 0, 10, 12, 5,
        14, 12, 11, 8, 1, 2, 3, 5, 15, 4, 10, 6, 7, 0, 9, 13,
        1, 14, 2, 11, 4, 12, 3, 7, 6, 13, 10, 5, 15, 9, 0, 8,
        11, 10, 5, 14, 6, 13, 9, 0, 12, 8, 15, 3, 2, 4, 7, 1,
        4, 12, 7, 5, 1, 6, 9, 10, 0, 14, 13, 8, 2, 11, 3, 15,
        13, 7, 15, 4, 1, 2, 6, 14, 9, 11, 3, 0, 8, 5, 12, 10,
        11, 9, 5, 1, 12, 3, 13, 14, 6, 4, 7, 15, 2, 0, 8, 10,
    };
    static constexpr auto Q = []() {
        std::array<std::array<uint32_t, 256>, 2> Q = {};
        uint8_t ha, hb, hc, hd, he, la, lb, lc, ld, le;
        for (int n = 0; n < 2; ++n) {
            for (ha = 0; ha < 16; ++ha) {
                for (la = 0; la < 16; ++la) {
                    hb =      ha  ^      la ;
                    lb = ROR4[la] ^ ASHX[ha];
                    hc = QT[0][n][hb];
                    lc = QT[1][n][lb];
                    hd =      hc  ^      lc ;
                    ld = ROR4[lc] ^ ASHX[hc];
                    he = QT[2][n][hd];
                    le = QT[3][n][ld];
                    Q[n][ha << 4 | la] = le << 4 | he;
                }
            }
        }
        return Q;
    }();
    static constexpr auto MDS = []() {
        std::array<std::array<uint32_t, 256>, 4> MDS = {};
        uint32_t f01, f5b, fef;
        for (int i = 0; i < 256; ++i) {
            f01 = Q[1][i];
            f5b = f01 ^ (f01 & 0x02 ? 0xb4 : 0x00) ^ f01 >> 2 ^ (f01 & 0x01 ? 0x5a : 0x00)           ;
            fef = f01 ^ (f01 & 0x02 ? 0xb4 : 0x00) ^ f01 >> 2 ^ (f01 & 0x01 ? 0xee : 0x00) ^ f01 >> 1;
            MDS[0][i] = f01 + (f5b << 8) + (fef << 16) + (fef << 24);
            MDS[2][i] = f5b + (fef << 8) + (f01 << 16) + (fef << 24);
            f01 = Q[0][i];
            f5b = f01 ^ (f01 & 0x02 ? 0xb4 : 0x00) ^ f01 >> 2 ^ (f01 & 0x01 ? 0x5a : 0x00)           ;
            fef = f01 ^ (f01 & 0x02 ? 0xb4 : 0x00) ^ f01 >> 2 ^ (f01 & 0x01 ? 0xee : 0x00) ^ f01 >> 1;
            MDS[1][i] = fef + (fef << 8) + (f5b << 16) + (f01 << 24);
            MDS[3][i] = f5b + (f01 << 8) + (fef << 16) + (f5b << 24);
        }
        return MDS;
    }();
};
template <int L>
requires (L == 2 || L == 3 || L == 4)
class TwofishTmpl: public TwofishBase {
    static uint32_t h_fun(uint8_t b, uint32_t const *key) {
        uint32_t x = (uint32_t)b * 0x01010101;
        switch (L) {
        case 4:
            x = Q[1][x & 0xff] | Q[0][x >> 8 & 0xff] << 8 | Q[0][x >> 16 & 0xff] << 16 | Q[1][x >> 24] << 24 | key[6];
        case 3:
            x = Q[1][x & 0xff] | Q[1][x >> 8 & 0xff] << 8 | Q[0][x >> 16 & 0xff] << 16 | Q[0][x >> 24] << 24 | key[4];
        case 2:
            x = Q[0][x & 0xff] | Q[1][x >> 8 & 0xff] << 8 | Q[0][x >> 16 & 0xff] << 16 | Q[1][x >> 24] << 24 | key[2];
            x = Q[0][x & 0xff] | Q[0][x >> 8 & 0xff] << 8 | Q[1][x >> 16 & 0xff] << 16 | Q[1][x >> 24] << 24 | key[0];
        }
        return x;
    }
    uint32_t rnk[40];
    uint32_t mks[4][256];
public:
    static constexpr size_t BLOCK_SIZE = 16;
    TwofishTmpl(uint8_t const *kin) {
        uint32_t key[2 * L];
        uint32_t vec[2 * L];
        READ_LE(key, kin, 2 * L);
        for (int i = 0; i < L; i++) {
            uint32_t h = key[2 * i + 1];
            uint32_t l = key[2 * i    ];
            for (int i = 0; i < 8; i++) {
                uint8_t t, u, v;
                t =          h >> 24;
                h = h << 8 | l >> 24;
                l = l << 8          ;
                u = t << 1 ^ (t & 0x80 ? 0x4d : 0x00)    ;
                v = t >> 1 ^ (t & 0x01 ? 0xa6 : 0x00) ^ u;
                h ^= v << 24 | u << 16 | v << 8 | t;
            }
            vec[2 * (L - 1 - i)] = h;
        }
        for (int i = 0; i < 256; i++) {
            uint32_t t = h_fun(i    , vec    );
            mks[0][i] = MDS[0][t       & 0xff];
            mks[1][i] = MDS[1][t >>  8 & 0xff];
            mks[2][i] = MDS[2][t >> 16 & 0xff];
            mks[3][i] = MDS[3][t >> 24       ];
        }
        for (int i = 0; i < 40; i += 2) {
            uint32_t a = h_fun(i    , key    );
            uint32_t b = h_fun(i + 1, key + 1);
            a = MDS[0][a & 0xff] ^ MDS[1][a >> 8 & 0xff] ^ MDS[2][a >> 16 & 0xff] ^ MDS[3][a >> 24];
            b = MDS[0][b & 0xff] ^ MDS[1][b >> 8 & 0xff] ^ MDS[2][b >> 16 & 0xff] ^ MDS[3][b >> 24];
            b = ROTL(b, 8), a += b, rnk[i    ] = a;
            b += a, b = ROTL(b, 9), rnk[i + 1] = b;
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t a, b, c, d, x, y;
        a = GET_LE<uint32_t>(src     );
        b = GET_LE<uint32_t>(src +  4);
        c = GET_LE<uint32_t>(src +  8);
        d = GET_LE<uint32_t>(src + 12);
        a ^= rnk[0];
        b ^= rnk[1];
        c ^= rnk[2];
        d ^= rnk[3];
        ENCCYCLE(0, a, b, c, d, x, y);
        ENCCYCLE(1, a, b, c, d, x, y);
        ENCCYCLE(2, a, b, c, d, x, y);
        ENCCYCLE(3, a, b, c, d, x, y);
        ENCCYCLE(4, a, b, c, d, x, y);
        ENCCYCLE(5, a, b, c, d, x, y);
        ENCCYCLE(6, a, b, c, d, x, y);
        ENCCYCLE(7, a, b, c, d, x, y);
        c ^= rnk[4];
        d ^= rnk[5];
        a ^= rnk[6];
        b ^= rnk[7];
        PUT_LE(dst     , c);
        PUT_LE(dst +  4, d);
        PUT_LE(dst +  8, a);
        PUT_LE(dst + 12, b);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t a, b, c, d, x, y;
        c = GET_LE<uint32_t>(src     );
        d = GET_LE<uint32_t>(src +  4);
        a = GET_LE<uint32_t>(src +  8);
        b = GET_LE<uint32_t>(src + 12);
        c ^= rnk[4];
        d ^= rnk[5];
        a ^= rnk[6];
        b ^= rnk[7];
        DECCYCLE(7, a, b, c, d, x, y);
        DECCYCLE(6, a, b, c, d, x, y);
        DECCYCLE(5, a, b, c, d, x, y);
        DECCYCLE(4, a, b, c, d, x, y);
        DECCYCLE(3, a, b, c, d, x, y);
        DECCYCLE(2, a, b, c, d, x, y);
        DECCYCLE(1, a, b, c, d, x, y);
        DECCYCLE(0, a, b, c, d, x, y);
        a ^= rnk[0];
        b ^= rnk[1];
        c ^= rnk[2];
        d ^= rnk[3];
        PUT_LE(dst     , a);
        PUT_LE(dst +  4, b);
        PUT_LE(dst +  8, c);
        PUT_LE(dst + 12, d);
    }
};
using Twofish128 = TwofishTmpl<2>;
using Twofish192 = TwofishTmpl<3>;
using Twofish256 = TwofishTmpl<4>;
#undef G0_FUN
#undef G1_FUN
#undef ENCROUND
#undef DECROUND
#undef ENCCYCLE
#undef DECCYCLE
