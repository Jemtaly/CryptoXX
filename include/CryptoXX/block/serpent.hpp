#pragma once
#include "../utils.hpp"
#define LTROTL(x) do {                        \
    x[0] = ROTL(x[0]                   , 13); \
    x[2] = ROTL(x[2]                   ,  3); \
    x[3] = ROTL(x[3] ^ x[2] ^ x[0] << 3,  7); \
    x[1] = ROTL(x[1] ^ x[0] ^ x[2]     ,  1); \
    x[0] = ROTL(x[0] ^ x[1] ^ x[3]     ,  5); \
    x[2] = ROTL(x[2] ^ x[3] ^ x[1] << 7, 22); \
} while (0)
#define LTROTR(x) do {                        \
    x[2] = ROTR(x[2], 22) ^ x[3] ^ x[1] << 7; \
    x[0] = ROTR(x[0],  5) ^ x[1] ^ x[3]     ; \
    x[1] = ROTR(x[1],  1) ^ x[0] ^ x[2]     ; \
    x[3] = ROTR(x[3],  7) ^ x[2] ^ x[0] << 3; \
    x[2] = ROTR(x[2],  3)                   ; \
    x[0] = ROTR(x[0], 13)                   ; \
} while (0)
#define XK(r, x) do {       \
    x[0] ^= rk[4 * r +  8]; \
    x[1] ^= rk[4 * r +  9]; \
    x[2] ^= rk[4 * r + 10]; \
    x[3] ^= rk[4 * r + 11]; \
} while (0)
#define LK(r, x) do {       \
    x[0]  = rk[4 * r +  8]; \
    x[1]  = rk[4 * r +  9]; \
    x[2]  = rk[4 * r + 10]; \
    x[3]  = rk[4 * r + 11]; \
} while (0)
#define SK(r, x) do {       \
    rk[4 * r +  8]  = x[0]; \
    rk[4 * r +  9]  = x[1]; \
    rk[4 * r + 10]  = x[2]; \
    rk[4 * r + 11]  = x[3]; \
} while (0)
#define SB0(R) do { \
    R[3] ^= R[0];   \
    R[4] =  R[1];   \
    R[1] &= R[3];   \
    R[4] ^= R[2];   \
    R[1] ^= R[0];   \
    R[0] |= R[3];   \
    R[0] ^= R[4];   \
    R[4] ^= R[3];   \
    R[3] ^= R[2];   \
    R[2] |= R[1];   \
    R[2] ^= R[4];   \
    R[4] = ~R[4];   \
    R[4] |= R[1];   \
    R[1] ^= R[3];   \
    R[1] ^= R[4];   \
    R[3] |= R[0];   \
    R[1] ^= R[3];   \
    R[4] ^= R[3];   \
    R[3] =  R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[4];   \
} while (0)
#define IB0(R) do { \
    R[2] = ~R[2];   \
    R[4] =  R[1];   \
    R[1] |= R[0];   \
    R[4] = ~R[4];   \
    R[1] ^= R[2];   \
    R[2] |= R[4];   \
    R[1] ^= R[3];   \
    R[0] ^= R[4];   \
    R[2] ^= R[0];   \
    R[0] &= R[3];   \
    R[4] ^= R[0];   \
    R[0] |= R[1];   \
    R[0] ^= R[2];   \
    R[3] ^= R[4];   \
    R[2] ^= R[1];   \
    R[3] ^= R[0];   \
    R[3] ^= R[1];   \
    R[2] &= R[3];   \
    R[4] ^= R[2];   \
    R[2] =  R[1];   \
    R[1] =  R[4];   \
} while (0)
#define SB1(R) do { \
    R[0] = ~R[0];   \
    R[2] = ~R[2];   \
    R[4] =  R[0];   \
    R[0] &= R[1];   \
    R[2] ^= R[0];   \
    R[0] |= R[3];   \
    R[3] ^= R[2];   \
    R[1] ^= R[0];   \
    R[0] ^= R[4];   \
    R[4] |= R[1];   \
    R[1] ^= R[3];   \
    R[2] |= R[0];   \
    R[2] &= R[4];   \
    R[0] ^= R[1];   \
    R[1] &= R[2];   \
    R[1] ^= R[0];   \
    R[0] &= R[2];   \
    R[4] ^= R[0];   \
    R[0] =  R[2];   \
    R[2] =  R[3];   \
    R[3] =  R[1];   \
    R[1] =  R[4];   \
} while (0)
#define IB1(R) do { \
    R[4] =  R[1];   \
    R[1] ^= R[3];   \
    R[3] &= R[1];   \
    R[4] ^= R[2];   \
    R[3] ^= R[0];   \
    R[0] |= R[1];   \
    R[2] ^= R[3];   \
    R[0] ^= R[4];   \
    R[0] |= R[2];   \
    R[1] ^= R[3];   \
    R[0] ^= R[1];   \
    R[1] |= R[3];   \
    R[1] ^= R[0];   \
    R[4] = ~R[4];   \
    R[4] ^= R[1];   \
    R[1] |= R[0];   \
    R[1] ^= R[0];   \
    R[1] |= R[4];   \
    R[3] ^= R[1];   \
    R[1] =  R[0];   \
    R[0] =  R[4];   \
    R[4] =  R[2];   \
    R[2] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
#define SB2(R) do { \
    R[4] =  R[0];   \
    R[0] &= R[2];   \
    R[0] ^= R[3];   \
    R[2] ^= R[1];   \
    R[2] ^= R[0];   \
    R[3] |= R[4];   \
    R[3] ^= R[1];   \
    R[4] ^= R[2];   \
    R[1] =  R[3];   \
    R[3] |= R[4];   \
    R[3] ^= R[0];   \
    R[0] &= R[1];   \
    R[4] ^= R[0];   \
    R[1] ^= R[3];   \
    R[1] ^= R[4];   \
    R[0] =  R[2];   \
    R[2] =  R[1];   \
    R[1] =  R[3];   \
    R[3] = ~R[4];   \
} while (0)
#define IB2(R) do { \
    R[2] ^= R[3];   \
    R[3] ^= R[0];   \
    R[4] =  R[3];   \
    R[3] &= R[2];   \
    R[3] ^= R[1];   \
    R[1] |= R[2];   \
    R[1] ^= R[4];   \
    R[4] &= R[3];   \
    R[2] ^= R[3];   \
    R[4] &= R[0];   \
    R[4] ^= R[2];   \
    R[2] &= R[1];   \
    R[2] |= R[0];   \
    R[3] = ~R[3];   \
    R[2] ^= R[3];   \
    R[0] ^= R[3];   \
    R[0] &= R[1];   \
    R[3] ^= R[4];   \
    R[3] ^= R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[4];   \
} while (0)
#define SB3(R) do { \
    R[4] =  R[0];   \
    R[0] |= R[3];   \
    R[3] ^= R[1];   \
    R[1] &= R[4];   \
    R[4] ^= R[2];   \
    R[2] ^= R[3];   \
    R[3] &= R[0];   \
    R[4] |= R[1];   \
    R[3] ^= R[4];   \
    R[0] ^= R[1];   \
    R[4] &= R[0];   \
    R[1] ^= R[3];   \
    R[4] ^= R[2];   \
    R[1] |= R[0];   \
    R[1] ^= R[2];   \
    R[0] ^= R[3];   \
    R[2] =  R[1];   \
    R[1] |= R[3];   \
    R[0] ^= R[1];   \
    R[1] =  R[2];   \
    R[2] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
#define IB3(R) do { \
    R[4] =  R[2];   \
    R[2] ^= R[1];   \
    R[0] ^= R[2];   \
    R[4] &= R[2];   \
    R[4] ^= R[0];   \
    R[0] &= R[1];   \
    R[1] ^= R[3];   \
    R[3] |= R[4];   \
    R[2] ^= R[3];   \
    R[0] ^= R[3];   \
    R[1] ^= R[4];   \
    R[3] &= R[2];   \
    R[3] ^= R[1];   \
    R[1] ^= R[0];   \
    R[1] |= R[2];   \
    R[0] ^= R[3];   \
    R[1] ^= R[4];   \
    R[0] ^= R[1];   \
    R[4] =  R[0];   \
    R[0] =  R[2];   \
    R[2] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
#define SB4(R) do { \
    R[1] ^= R[3];   \
    R[3] = ~R[3];   \
    R[2] ^= R[3];   \
    R[3] ^= R[0];   \
    R[4] =  R[1];   \
    R[1] &= R[3];   \
    R[1] ^= R[2];   \
    R[4] ^= R[3];   \
    R[0] ^= R[4];   \
    R[2] &= R[4];   \
    R[2] ^= R[0];   \
    R[0] &= R[1];   \
    R[3] ^= R[0];   \
    R[4] |= R[1];   \
    R[4] ^= R[0];   \
    R[0] |= R[3];   \
    R[0] ^= R[2];   \
    R[2] &= R[3];   \
    R[0] = ~R[0];   \
    R[4] ^= R[2];   \
    R[2] =  R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[4];   \
} while (0)
#define IB4(R) do { \
    R[4] =  R[2];   \
    R[2] &= R[3];   \
    R[2] ^= R[1];   \
    R[1] |= R[3];   \
    R[1] &= R[0];   \
    R[4] ^= R[2];   \
    R[4] ^= R[1];   \
    R[1] &= R[2];   \
    R[0] = ~R[0];   \
    R[3] ^= R[4];   \
    R[1] ^= R[3];   \
    R[3] &= R[0];   \
    R[3] ^= R[2];   \
    R[0] ^= R[1];   \
    R[2] &= R[0];   \
    R[3] ^= R[0];   \
    R[2] ^= R[4];   \
    R[2] |= R[3];   \
    R[3] ^= R[0];   \
    R[2] ^= R[1];   \
    R[1] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
#define SB5(R) do { \
    R[0] ^= R[1];   \
    R[1] ^= R[3];   \
    R[3] = ~R[3];   \
    R[4] =  R[1];   \
    R[1] &= R[0];   \
    R[2] ^= R[3];   \
    R[1] ^= R[2];   \
    R[2] |= R[4];   \
    R[4] ^= R[3];   \
    R[3] &= R[1];   \
    R[3] ^= R[0];   \
    R[4] ^= R[1];   \
    R[4] ^= R[2];   \
    R[2] ^= R[0];   \
    R[0] &= R[3];   \
    R[2] = ~R[2];   \
    R[0] ^= R[4];   \
    R[4] |= R[3];   \
    R[4] ^= R[2];   \
    R[2] =  R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
#define IB5(R) do { \
    R[1] = ~R[1];   \
    R[4] =  R[3];   \
    R[2] ^= R[1];   \
    R[3] |= R[0];   \
    R[3] ^= R[2];   \
    R[2] |= R[1];   \
    R[2] &= R[0];   \
    R[4] ^= R[3];   \
    R[2] ^= R[4];   \
    R[4] |= R[0];   \
    R[4] ^= R[1];   \
    R[1] &= R[2];   \
    R[1] ^= R[3];   \
    R[4] ^= R[2];   \
    R[3] &= R[4];   \
    R[4] ^= R[1];   \
    R[3] ^= R[4];   \
    R[4] = ~R[4];   \
    R[3] ^= R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[4];   \
    R[4] =  R[3];   \
    R[3] =  R[2];   \
    R[2] =  R[4];   \
} while (0)
#define SB6(R) do { \
    R[2] = ~R[2];   \
    R[4] =  R[3];   \
    R[3] &= R[0];   \
    R[0] ^= R[4];   \
    R[3] ^= R[2];   \
    R[2] |= R[4];   \
    R[1] ^= R[3];   \
    R[2] ^= R[0];   \
    R[0] |= R[1];   \
    R[2] ^= R[1];   \
    R[4] ^= R[0];   \
    R[0] |= R[3];   \
    R[0] ^= R[2];   \
    R[4] ^= R[3];   \
    R[4] ^= R[0];   \
    R[3] = ~R[3];   \
    R[2] &= R[4];   \
    R[3] ^= R[2];   \
    R[2] =  R[4];   \
} while (0)
#define IB6(R) do { \
    R[0] ^= R[2];   \
    R[4] =  R[2];   \
    R[2] &= R[0];   \
    R[4] ^= R[3];   \
    R[2] = ~R[2];   \
    R[3] ^= R[1];   \
    R[2] ^= R[3];   \
    R[4] |= R[0];   \
    R[0] ^= R[2];   \
    R[3] ^= R[4];   \
    R[4] ^= R[1];   \
    R[1] &= R[3];   \
    R[1] ^= R[0];   \
    R[0] ^= R[3];   \
    R[0] |= R[2];   \
    R[3] ^= R[1];   \
    R[4] ^= R[0];   \
    R[0] =  R[1];   \
    R[1] =  R[2];   \
    R[2] =  R[4];   \
} while (0)
#define SB7(R) do { \
    R[4] =  R[1];   \
    R[1] |= R[2];   \
    R[1] ^= R[3];   \
    R[4] ^= R[2];   \
    R[2] ^= R[1];   \
    R[3] |= R[4];   \
    R[3] &= R[0];   \
    R[4] ^= R[2];   \
    R[3] ^= R[1];   \
    R[1] |= R[4];   \
    R[1] ^= R[0];   \
    R[0] |= R[4];   \
    R[0] ^= R[2];   \
    R[1] ^= R[4];   \
    R[2] ^= R[1];   \
    R[1] &= R[0];   \
    R[1] ^= R[4];   \
    R[2] = ~R[2];   \
    R[2] |= R[0];   \
    R[4] ^= R[2];   \
    R[2] =  R[1];   \
    R[1] =  R[3];   \
    R[3] =  R[0];   \
    R[0] =  R[4];   \
} while (0)
#define IB7(R) do { \
    R[4] =  R[2];   \
    R[2] ^= R[0];   \
    R[0] &= R[3];   \
    R[4] |= R[3];   \
    R[2] = ~R[2];   \
    R[3] ^= R[1];   \
    R[1] |= R[0];   \
    R[0] ^= R[2];   \
    R[2] &= R[4];   \
    R[3] &= R[4];   \
    R[1] ^= R[2];   \
    R[2] ^= R[0];   \
    R[0] |= R[2];   \
    R[4] ^= R[1];   \
    R[0] ^= R[3];   \
    R[3] ^= R[4];   \
    R[4] |= R[0];   \
    R[3] ^= R[2];   \
    R[4] ^= R[2];   \
    R[2] =  R[1];   \
    R[1] =  R[0];   \
    R[0] =  R[3];   \
    R[3] =  R[4];   \
} while (0)
class SerpentBase {
protected:
    uint32_t rk[140] = {};
public:
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t x[5]; // 5th element is used as a temporary
        READ_LE(x, src, 4);
        XK( 0, x); SB0(x); LTROTL(x);
        XK( 1, x); SB1(x); LTROTL(x);
        XK( 2, x); SB2(x); LTROTL(x);
        XK( 3, x); SB3(x); LTROTL(x);
        XK( 4, x); SB4(x); LTROTL(x);
        XK( 5, x); SB5(x); LTROTL(x);
        XK( 6, x); SB6(x); LTROTL(x);
        XK( 7, x); SB7(x); LTROTL(x);
        XK( 8, x); SB0(x); LTROTL(x);
        XK( 9, x); SB1(x); LTROTL(x);
        XK(10, x); SB2(x); LTROTL(x);
        XK(11, x); SB3(x); LTROTL(x);
        XK(12, x); SB4(x); LTROTL(x);
        XK(13, x); SB5(x); LTROTL(x);
        XK(14, x); SB6(x); LTROTL(x);
        XK(15, x); SB7(x); LTROTL(x);
        XK(16, x); SB0(x); LTROTL(x);
        XK(17, x); SB1(x); LTROTL(x);
        XK(18, x); SB2(x); LTROTL(x);
        XK(19, x); SB3(x); LTROTL(x);
        XK(20, x); SB4(x); LTROTL(x);
        XK(21, x); SB5(x); LTROTL(x);
        XK(22, x); SB6(x); LTROTL(x);
        XK(23, x); SB7(x); LTROTL(x);
        XK(24, x); SB0(x); LTROTL(x);
        XK(25, x); SB1(x); LTROTL(x);
        XK(26, x); SB2(x); LTROTL(x);
        XK(27, x); SB3(x); LTROTL(x);
        XK(28, x); SB4(x); LTROTL(x);
        XK(29, x); SB5(x); LTROTL(x);
        XK(30, x); SB6(x); LTROTL(x);
        XK(31, x); SB7(x); XK(32, x);
        WRITE_LE(dst, x, 4);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t x[5]; // 5th element is used as a temporary
        READ_LE(x, src, 4);
        XK(32, x); IB7(x); XK(31, x);
        LTROTR(x); IB6(x); XK(30, x);
        LTROTR(x); IB5(x); XK(29, x);
        LTROTR(x); IB4(x); XK(28, x);
        LTROTR(x); IB3(x); XK(27, x);
        LTROTR(x); IB2(x); XK(26, x);
        LTROTR(x); IB1(x); XK(25, x);
        LTROTR(x); IB0(x); XK(24, x);
        LTROTR(x); IB7(x); XK(23, x);
        LTROTR(x); IB6(x); XK(22, x);
        LTROTR(x); IB5(x); XK(21, x);
        LTROTR(x); IB4(x); XK(20, x);
        LTROTR(x); IB3(x); XK(19, x);
        LTROTR(x); IB2(x); XK(18, x);
        LTROTR(x); IB1(x); XK(17, x);
        LTROTR(x); IB0(x); XK(16, x);
        LTROTR(x); IB7(x); XK(15, x);
        LTROTR(x); IB6(x); XK(14, x);
        LTROTR(x); IB5(x); XK(13, x);
        LTROTR(x); IB4(x); XK(12, x);
        LTROTR(x); IB3(x); XK(11, x);
        LTROTR(x); IB2(x); XK(10, x);
        LTROTR(x); IB1(x); XK( 9, x);
        LTROTR(x); IB0(x); XK( 8, x);
        LTROTR(x); IB7(x); XK( 7, x);
        LTROTR(x); IB6(x); XK( 6, x);
        LTROTR(x); IB5(x); XK( 5, x);
        LTROTR(x); IB4(x); XK( 4, x);
        LTROTR(x); IB3(x); XK( 3, x);
        LTROTR(x); IB2(x); XK( 2, x);
        LTROTR(x); IB1(x); XK( 1, x);
        LTROTR(x); IB0(x); XK( 0, x);
        WRITE_LE(dst, x, 4);
    }
};
template <int L>
    requires (L == 4 || L == 6 || L == 8)
class SerpentTmpl: public SerpentBase {
public:
    static constexpr size_t BLOCK_SIZE = 16;
    static constexpr size_t KEY_SIZE = 4 * L;
    SerpentTmpl(uint8_t const *kin) {
        uint32_t x[5]; // 5th element is used as a temporary
        READ_LE(rk, kin, L);
        if constexpr (L < 8) {
            rk[L] = 1; // padding with 1 bit and 0 bits to make 256 bits
        }
        for (int i = 0; i < 132; ++i) {
            rk[i + 8] = ROTL(rk[i] ^ rk[i + 3] ^ rk[i + 5] ^ rk[i + 7] ^ 0x9E3779B9 ^ i, 11);
        }
        LK( 0, x); SB3(x); SK( 0, x);
        LK( 1, x); SB2(x); SK( 1, x);
        LK( 2, x); SB1(x); SK( 2, x);
        LK( 3, x); SB0(x); SK( 3, x);
        LK( 4, x); SB7(x); SK( 4, x);
        LK( 5, x); SB6(x); SK( 5, x);
        LK( 6, x); SB5(x); SK( 6, x);
        LK( 7, x); SB4(x); SK( 7, x);
        LK( 8, x); SB3(x); SK( 8, x);
        LK( 9, x); SB2(x); SK( 9, x);
        LK(10, x); SB1(x); SK(10, x);
        LK(11, x); SB0(x); SK(11, x);
        LK(12, x); SB7(x); SK(12, x);
        LK(13, x); SB6(x); SK(13, x);
        LK(14, x); SB5(x); SK(14, x);
        LK(15, x); SB4(x); SK(15, x);
        LK(16, x); SB3(x); SK(16, x);
        LK(17, x); SB2(x); SK(17, x);
        LK(18, x); SB1(x); SK(18, x);
        LK(19, x); SB0(x); SK(19, x);
        LK(20, x); SB7(x); SK(20, x);
        LK(21, x); SB6(x); SK(21, x);
        LK(22, x); SB5(x); SK(22, x);
        LK(23, x); SB4(x); SK(23, x);
        LK(24, x); SB3(x); SK(24, x);
        LK(25, x); SB2(x); SK(25, x);
        LK(26, x); SB1(x); SK(26, x);
        LK(27, x); SB0(x); SK(27, x);
        LK(28, x); SB7(x); SK(28, x);
        LK(29, x); SB6(x); SK(29, x);
        LK(30, x); SB5(x); SK(30, x);
        LK(31, x); SB4(x); SK(31, x);
        LK(32, x); SB3(x); SK(32, x);
    }
};
using Serpent128 = SerpentTmpl<4>;
using Serpent192 = SerpentTmpl<6>;
using Serpent256 = SerpentTmpl<8>;
#undef LK
#undef SK
#undef XK
#undef LTROTR
#undef LTROTL
#undef SB0
#undef SB1
#undef SB2
#undef SB3
#undef SB4
#undef SB5
#undef SB6
#undef SB7
#undef IB0
#undef IB1
#undef IB2
#undef IB3
#undef IB4
#undef IB5
#undef IB6
#undef IB7
