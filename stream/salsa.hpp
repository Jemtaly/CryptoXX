#pragma once
#include "stream.hpp"
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
    Salsa(uint32_t const *key, uint32_t const *counter):
        input{
            0x61707865, key    [0], key    [1], key    [2], // "expa"
            key    [3], 0x3320646e, counter[2], counter[3], // "nd 3"
            counter[0], counter[1], 0x79622d32, key    [4], // "2-by"
            key    [5], key    [6], key    [7], 0x6b206574, // "te k"
        } {}
    void generate(uint8_t *buf) {
        uint32_t state[16] = {
            input[0x0], input[0x1], input[0x2], input[0x3],
            input[0x4], input[0x5], input[0x6], input[0x7],
            input[0x8], input[0x9], input[0xa], input[0xb],
            input[0xc], input[0xd], input[0xe], input[0xf],
        };
        for (int i = 0; i < RND / 2; i++) {
            QROUND(state,  0,  4,  8, 12);
            QROUND(state,  5,  9, 13,  1);
            QROUND(state, 10, 14,  2,  6);
            QROUND(state, 15,  3,  7, 11);
            QROUND(state,  0,  1,  2,  3);
            QROUND(state,  5,  6,  7,  4);
            QROUND(state, 10, 11,  8,  9);
            QROUND(state, 15, 12, 13, 14);
        }
        for (int i = 0; i < 16; i++) {
            PUT_LE(buf, state[i] + input[i]);
        }
        ++input[ 8] == 0 && ++input[ 9] == 0;
    }
};
#undef QROUND
