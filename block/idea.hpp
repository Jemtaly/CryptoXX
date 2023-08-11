#pragma once
#include "block.hpp"
class IDEA {
    static uint16_t add(uint16_t a, uint16_t b) {
        return a + b;
    }
    static uint16_t neg(uint16_t a) {
        return 0 - a;
    }
    static uint16_t mul(uint16_t a, uint16_t b) {
        if (a == 0) {
            return 1 - b;
        }
        if (b == 0) {
            return 1 - a;
        }
        uint32_t c = a * b;
        uint16_t h = c >> 16;
        uint16_t l = c;
        return l - h + (l < h);
    }
    static uint16_t inv(uint16_t a) {
        if (a <= 1) {
            return a;
        }
        uint32_t x = a;
        uint32_t y = 0x10001;
        uint32_t u = 1;
        uint32_t v = 0;
        for (;;) {
            v += y / x * u;
            y %= x;
            if (y == 1) {
                return 1 - v;
            }
            u += x / y * v;
            x %= y;
            if (x == 1) {
                return 0 + u;
            }
        }
    }
    uint16_t e[52];
    uint16_t d[52];
public:
    static constexpr size_t BLOCK_SIZE = 8;
    static constexpr size_t KEY_SIZE = 16;
    IDEA(uint8_t const *key) {
        READ_BE(e, key, 8);
        for (int i = 8; i < 52; i++) {
            uint16_t a = e[(i + 1) % 8 < 1 ? i - 15 : i - 7];
            uint16_t b = e[(i + 2) % 8 < 2 ? i - 14 : i - 6];
            e[i] = (a << 9) + (b >> 7);
        }
        // decrypt key schedule
        for (int i = 0; i < 52; i += 6) {
            int x = i == 0 || i == 48 ? 1 : 2;
            int y = i == 0 || i == 48 ? 2 : 1;
            d[i    ] = inv(e[48 - i]);
            d[i + x] = neg(e[49 - i]);
            d[i + y] = neg(e[50 - i]);
            d[i + 3] = inv(e[51 - i]);
        }
        for (int i = 0; i < 48; i += 6) {
            d[i + 4] = e[46 - i];
            d[i + 5] = e[47 - i];
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        uint16_t x[4], y[4], t[6];
        READ_BE(x, src, 4);
        for (int i = 0; i < 48; i += 6) {
            y[0] = mul(x[0], e[i    ]);
            y[1] = add(x[1], e[i + 1]);
            y[2] = add(x[2], e[i + 2]);
            y[3] = mul(x[3], e[i + 3]);
            t[0] = y[0] ^ y[2];
            t[1] = y[1] ^ y[3];
            t[2] = mul(t[0], e[i + 4]);
            t[3] = add(t[1], t[2]);
            t[4] = mul(t[3], e[i + 5]);
            t[5] = add(t[2], t[4]);
            x[0] = y[0] ^ t[4];
            x[1] = y[2] ^ t[4];
            x[2] = y[1] ^ t[5];
            x[3] = y[3] ^ t[5];
        }
        y[0] = mul(x[0], e[48]);
        y[1] = add(x[2], e[49]);
        y[2] = add(x[1], e[50]);
        y[3] = mul(x[3], e[51]);
        WRITE_BE(dst, y, 4);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        uint16_t x[4], y[4], t[6];
        READ_BE(x, src, 4);
        for (int i = 0; i < 48; i += 6) {
            y[0] = mul(x[0], d[i    ]);
            y[1] = add(x[1], d[i + 1]);
            y[2] = add(x[2], d[i + 2]);
            y[3] = mul(x[3], d[i + 3]);
            t[0] = y[0] ^ y[2];
            t[1] = y[1] ^ y[3];
            t[2] = mul(t[0], d[i + 4]);
            t[3] = add(t[1], t[2]);
            t[4] = mul(t[3], d[i + 5]);
            t[5] = add(t[2], t[4]);
            x[0] = y[0] ^ t[4];
            x[1] = y[2] ^ t[4];
            x[2] = y[1] ^ t[5];
            x[3] = y[3] ^ t[5];
        }
        y[0] = mul(x[0], d[48]);
        y[1] = add(x[2], d[49]);
        y[2] = add(x[1], d[50]);
        y[3] = mul(x[3], d[51]);
        WRITE_BE(dst, y, 4);
    }
};
