#pragma once
#include "stream.hpp"
class RC4 {
    uint8_t x, y, m[256];
public:
    static constexpr size_t SECTION_SIZE = 1;
    static constexpr size_t KEY_SIZE = 256;
    static constexpr size_t CIV_SIZE = 0;
    RC4(uint8_t const *civ, uint8_t const *key, size_t len = 256):
        x(0), y(0) {
        for (int i = 0; i < 256; i++) {
            m[i] = i;
        }
        uint8_t j = 0, temp;
        for (int i = 0; i < 256; i++) {
            temp = m[i];
            j += temp + key[i % len];
            m[i] = m[j];
            m[j] = temp;
        }
    }
    void generate(uint8_t *buf) {
        uint8_t a = m[x += 1];
        uint8_t b = m[y += a];
        m[x] = b;
        m[y] = a;
        *buf = m[a + b & 255];
    }
};
