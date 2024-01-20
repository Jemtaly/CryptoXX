#pragma once
#include "../utils.hpp"
#define QROUND(s, a, b, c, d) do { \
    s[b] ^= ROTL(s[a] + s[d],  7); \
    s[c] ^= ROTL(s[b] + s[a],  9); \
    s[d] ^= ROTL(s[c] + s[b], 13); \
    s[a] ^= ROTL(s[d] + s[c], 18); \
} while (0)
template <int RND>
    requires (RND == 8 || RND == 12 || RND == 20)
class Salsa {
    uint32_t input[16];
public:
    static constexpr size_t SECTION_SIZE = 64;
    static constexpr size_t KEY_SIZE = 32;
    static constexpr size_t CIV_SIZE = 8;
    Salsa(uint8_t const *civ, uint8_t const *key) {
        input[0x0] = 0x61707865;
        input[0x5] = 0x3320646e;
        input[0xa] = 0x79622d32;
        input[0xf] = 0x6b206574;
        input[0x1] = GET_LE<uint32_t>(key +  0);
        input[0x2] = GET_LE<uint32_t>(key +  4);
        input[0x3] = GET_LE<uint32_t>(key +  8);
        input[0x4] = GET_LE<uint32_t>(key + 12);
        input[0xb] = GET_LE<uint32_t>(key + 16);
        input[0xc] = GET_LE<uint32_t>(key + 20);
        input[0xd] = GET_LE<uint32_t>(key + 24);
        input[0xe] = GET_LE<uint32_t>(key + 28);
        input[0x8] = 0x00000000;
        input[0x9] = 0x00000000;
        input[0x6] = GET_LE<uint32_t>(civ +  0);
        input[0x7] = GET_LE<uint32_t>(civ +  4);
    }
    void generate(uint8_t *buf) {
        uint32_t state[16] = {
            input[0x0], input[0x1], input[0x2], input[0x3],
            input[0x4], input[0x5], input[0x6], input[0x7],
            input[0x8], input[0x9], input[0xa], input[0xb],
            input[0xc], input[0xd], input[0xe], input[0xf],
        };
        FOR_(i, 0, i + 1, i < RND / 2, {
            QROUND(state,  0,  4,  8, 12);
            QROUND(state,  5,  9, 13,  1);
            QROUND(state, 10, 14,  2,  6);
            QROUND(state, 15,  3,  7, 11);
            QROUND(state,  0,  1,  2,  3);
            QROUND(state,  5,  6,  7,  4);
            QROUND(state, 10, 11,  8,  9);
            QROUND(state, 15, 12, 13, 14);
        });
        FOR_(i, 0, i + 1, i < 16, {
            state[i] += input[i];
        });
        WRITE_LE(buf, state, 16);
        ++input[ 8] == 0 && ++input[ 9];
    }
};
using Salsa8 = Salsa<8>;
using Salsa12 = Salsa<12>;
using Salsa20 = Salsa<20>;
#undef QROUND
