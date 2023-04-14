#pragma once
#include "hash.hpp"
#define UNROTLL_5(F) { F(0); F(1); F(2); F(3); F(4); }
#define UNROTLL_X(F) {                           \
    F(0, 0); F(0, 1); F(0, 2); F(0, 3); F(0, 4); \
    F(1, 0); F(1, 1); F(1, 2); F(1, 3); F(1, 4); \
    F(2, 0); F(2, 1); F(2, 2); F(2, 3); F(2, 4); \
    F(3, 0); F(3, 1); F(3, 2); F(3, 3); F(3, 4); \
    F(4, 0); F(4, 1); F(4, 2); F(4, 3); F(4, 4); \
}
#define C_A(   x) C[x] = A[0][x] ^ A[1][x] ^ A[2][x] ^ A[3][x] ^ A[4][x]
#define D_C(   x) D[x] = C[(x + 4) % 5] ^ ROTL(C[(x + 1) % 5], 1)
#define A_D(y, x) A[y][x] ^= D[x]
#define B_A(y, x) B[(2 * x + 3 * y) % 5][y] = ROTL(A[y][x], R[y][x])
#define A_B(y, x) A[y][x] = ~B[y][(x + 1) % 5] & B[y][(x + 2) % 5] ^ B[y][x]
typedef uint8_t bits_t;
class KeccakBase {
protected:
    static constexpr uint64_t RC[24] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
        0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008A,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
        0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
    };
    static constexpr bits_t R[5][5] = {
        { 0,  1, 62, 28, 27},
        {36, 44,  6, 55, 20},
        { 3, 10, 43, 25, 39},
        {41, 45, 15, 21,  8},
        {18,  2, 61, 56, 14},
    };
};
template <uint8_t PAD_BYTE, size_t BLK, size_t DIG>
requires (DIG <= BLK && BLK <= 200)
class KeccakTmpl: public KeccakBase {
    void permute() {
        for (int i = 0; i < 24; i++) {
            uint64_t C[5];
            uint64_t D[5];
            uint64_t B[5][5];
            UNROTLL_5(C_A);
            UNROTLL_5(D_C);
            UNROTLL_X(A_D);
            UNROTLL_X(B_A);
            UNROTLL_X(A_B);
            A[0][0] ^= RC[i];
        }
    }
    uint64_t A[5][5] = {};
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
    static constexpr bool NO_PADDING = false;
    void push(uint8_t const *blk) {
        XORB_LE((uint64_t *)A, blk, BLK);
        permute();
    }
    void hash(uint8_t const *src, size_t len, uint8_t *out) {
        XORB_LE((uint64_t *)A, src, len);
        BYTE_LE((uint64_t *)A, len) ^= PAD_BYTE;
        BYTE_LE((uint64_t *)A, BLK - 1) ^= 0x80;
        permute();
        WRITEB_LE(out, (uint64_t *)A, DIG);
    }
};
template <size_t BIT> requires (BIT == 224 || BIT == 256 || BIT == 384 || BIT == 512)
using Keccak = KeccakTmpl<0x01, (1600 - BIT * 2) / 8, BIT / 8>;
template <size_t BIT> requires (BIT == 224 || BIT == 256 || BIT == 384 || BIT == 512)
using SHA3   = KeccakTmpl<0x06, (1600 - BIT * 2) / 8, BIT / 8>;
template <size_t BIT, size_t OUT> requires (BIT == 128 || BIT == 256)
using SHAKE  = KeccakTmpl<0x1f, (1600 - BIT * 2) / 8, OUT / 8>;
#undef C_A
#undef D_C
#undef A_D
#undef B_A
#undef A_B
