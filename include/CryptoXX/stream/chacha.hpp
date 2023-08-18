#pragma once
#include "../utils.hpp"
#define QROUND(s, a, b, c, d) do {                     \
    s[a] += s[b], s[d] ^= s[a], s[d] = ROTL(s[d], 16); \
    s[c] += s[d], s[b] ^= s[c], s[b] = ROTL(s[b], 12); \
    s[a] += s[b], s[d] ^= s[a], s[d] = ROTL(s[d],  8); \
    s[c] += s[d], s[b] ^= s[c], s[b] = ROTL(s[b],  7); \
} while (0)
template <int RND>
    requires (RND == 8 || RND == 12 || RND == 20)
class ChaCha {
    uint32_t input[16];
public:
    static constexpr size_t SECTION_SIZE = 64;
    static constexpr size_t KEY_SIZE = 32;
    static constexpr size_t CIV_SIZE = 8;
    ChaCha(uint8_t const *civ, uint8_t const *key) {
        input[0x0] = 0x61707865;
        input[0x1] = 0x3320646e;
        input[0x2] = 0x79622d32;
        input[0x3] = 0x6b206574;
        input[0x4] = GET_LE<uint32_t>(key +  0);
        input[0x5] = GET_LE<uint32_t>(key +  4);
        input[0x6] = GET_LE<uint32_t>(key +  8);
        input[0x7] = GET_LE<uint32_t>(key + 12);
        input[0x8] = GET_LE<uint32_t>(key + 16);
        input[0x9] = GET_LE<uint32_t>(key + 20);
        input[0xa] = GET_LE<uint32_t>(key + 24);
        input[0xb] = GET_LE<uint32_t>(key + 28);
        input[0xc] = 0x00000000;
        input[0xd] = 0x00000000;
        input[0xe] = GET_LE<uint32_t>(civ +  0);
        input[0xf] = GET_LE<uint32_t>(civ +  4);
    }
    void generate(uint8_t *buf) {
        uint32_t state[16] = {
            input[0x0], input[0x1], input[0x2], input[0x3],
            input[0x4], input[0x5], input[0x6], input[0x7],
            input[0x8], input[0x9], input[0xa], input[0xb],
            input[0xc], input[0xd], input[0xe], input[0xf],
        };
        for (int i = 0; i < RND / 2; i++) {
            QROUND(state,  0,  4,  8, 12);
            QROUND(state,  1,  5,  9, 13);
            QROUND(state,  2,  6, 10, 14);
            QROUND(state,  3,  7, 11, 15);
            QROUND(state,  0,  5, 10, 15);
            QROUND(state,  1,  6, 11, 12);
            QROUND(state,  2,  7,  8, 13);
            QROUND(state,  3,  4,  9, 14);
        }
        PUT_LE(buf +  0, state[0x0] += input[0x0]);
        PUT_LE(buf +  4, state[0x1] += input[0x1]);
        PUT_LE(buf +  8, state[0x2] += input[0x2]);
        PUT_LE(buf + 12, state[0x3] += input[0x3]);
        PUT_LE(buf + 16, state[0x4] += input[0x4]);
        PUT_LE(buf + 20, state[0x5] += input[0x5]);
        PUT_LE(buf + 24, state[0x6] += input[0x6]);
        PUT_LE(buf + 28, state[0x7] += input[0x7]);
        PUT_LE(buf + 32, state[0x8] += input[0x8]);
        PUT_LE(buf + 36, state[0x9] += input[0x9]);
        PUT_LE(buf + 40, state[0xa] += input[0xa]);
        PUT_LE(buf + 44, state[0xb] += input[0xb]);
        PUT_LE(buf + 48, state[0xc] += input[0xc]);
        PUT_LE(buf + 52, state[0xd] += input[0xd]);
        PUT_LE(buf + 56, state[0xe] += input[0xe]);
        PUT_LE(buf + 60, state[0xf] += input[0xf]);
        ++input[12] == 0 && ++input[13];
    }
};
using ChaCha8 = ChaCha<8>;
using ChaCha12 = ChaCha<12>;
using ChaCha20 = ChaCha<20>;
#undef QROUND
