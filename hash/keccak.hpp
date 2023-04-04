#pragma once
#include "hash.hpp"
#define ROL64(x, n) ((x) << (n) | (x) >> (64 - (n)))
#define RLX64(x, n) ((x) << ((n) & 63) | (x) >> (-(n) & 63))
#define UNROLL_5(F) { F(0); F(1); F(2); F(3); F(4); }
#define UNROLL_X(F) {                            \
    F(0, 0); F(0, 1); F(0, 2); F(0, 3); F(0, 4); \
    F(1, 0); F(1, 1); F(1, 2); F(1, 3); F(1, 4); \
    F(2, 0); F(2, 1); F(2, 2); F(2, 3); F(2, 4); \
    F(3, 0); F(3, 1); F(3, 2); F(3, 3); F(3, 4); \
    F(4, 0); F(4, 1); F(4, 2); F(4, 3); F(4, 4); \
}
#define C_A(   x) C[x] = A[0][x] ^ A[1][x] ^ A[2][x] ^ A[3][x] ^ A[4][x]
#define D_C(   x) D[x] = C[(x + 4) % 5] ^ ROL64(C[(x + 1) % 5], 1)
#define A_D(y, x) A[y][x] ^= D[x]
#define B_A(y, x) B[(2 * x + 3 * y) % 5][y] = RLX64(A[y][x], r[y][x])
#define A_B(y, x) A[y][x] = ~B[y][(x + 1) % 5] & B[y][(x + 2) % 5] ^ B[y][x]
#define XOR(x, y) A[y][x] ^= ((uint64_t (*)[5])blk)[y][x]
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
    static constexpr bits_t r[5][5] = {
        { 0,  1, 62, 28, 27},
        {36, 44,  6, 55, 20},
        { 3, 10, 43, 25, 39},
        {41, 45, 15, 21,  8},
        {18,  2, 61, 56, 14},
    };
};
template <uint8_t PAD_BYTE, int BLK, int DIG>
class KeccakTmpl: public KeccakBase {
    uint64_t A[5][5] = {};
    void permute() {
        for (int i = 0; i < 24; i++) {
            uint64_t C[5];
            uint64_t D[5];
            uint64_t B[5][5];
            UNROLL_5(C_A);
            UNROLL_5(D_C);
            UNROLL_X(A_D);
            UNROLL_X(B_A);
            UNROLL_X(A_B);
            A[0][0] ^= RC[i];
        }
    }
public:
    constexpr static int BLOCK_SIZE = BLK;
    constexpr static int DIGEST_SIZE = DIG;
    static constexpr bool NO_PADDING = false;
    void push(uint8_t const *blk) {
        for (int i = 0; i < BLK; i++) {
            ((uint8_t *)A)[i] ^= blk[i];
        }
        permute();
    }
    void hash(uint8_t const *src, int len, uint8_t *out) {
        for (int i = 0; i < len; i++) {
            ((uint8_t *)A)[i] ^= src[i];
        }
        ((uint8_t *)A)[len] ^= PAD_BYTE;
        ((uint8_t *)A)[BLK - 1] ^= 0x80;
        permute();
        for (int i = 0; i < DIG; i++) {
            out[i] = ((uint8_t *)A)[i];
        }
    }
};
template <int BIT> requires (BIT == 224 || BIT == 256 || BIT == 384 || BIT == 512)
using Keccak = KeccakTmpl<0x01, (1600 - BIT * 2) / 8, BIT / 8>;
template <int BIT> requires (BIT == 224 || BIT == 256 || BIT == 384 || BIT == 512)
using SHA3   = KeccakTmpl<0x06, (1600 - BIT * 2) / 8, BIT / 8>;
template <int BIT, int OUT> requires (BIT == 128 || BIT == 256)
using SHAKE  = KeccakTmpl<0x1f, (1600 - BIT * 2) / 8, OUT / 8>;
#undef C_A
#undef D_C
#undef A_D
#undef B_A
#undef A_B
#undef XOR
