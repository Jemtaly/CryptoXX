#pragma once
#include "block.hpp"
#define XXROTL(x) {                           \
    x[0] = ROTL(x[0]                   , 13); \
    x[2] = ROTL(x[2]                   ,  3); \
    x[3] = ROTL(x[3] ^ x[2] ^ x[0] << 3,  7); \
    x[1] = ROTL(x[1] ^ x[0] ^ x[2]     ,  1); \
    x[0] = ROTL(x[0] ^ x[1] ^ x[3]     ,  5); \
    x[2] = ROTL(x[2] ^ x[3] ^ x[1] << 7, 22); \
}
#define XXROTR(x) {                           \
    x[2] = ROTR(x[2], 22) ^ x[3] ^ x[1] << 7; \
    x[0] = ROTR(x[0],  5) ^ x[1] ^ x[3]     ; \
    x[1] = ROTR(x[1],  1) ^ x[0] ^ x[2]     ; \
    x[3] = ROTR(x[3],  7) ^ x[2] ^ x[0] << 3; \
    x[2] = ROTR(x[2],  3)                   ; \
    x[0] = ROTR(x[0], 13)                   ; \
}
#define XK(r, x) {          \
    x[0] ^= rk[4 * r +  8]; \
    x[1] ^= rk[4 * r +  9]; \
    x[2] ^= rk[4 * r + 10]; \
    x[3] ^= rk[4 * r + 11]; \
}
#define LK(r, x) {          \
    x[0]  = rk[4 * r +  8]; \
    x[1]  = rk[4 * r +  9]; \
    x[2]  = rk[4 * r + 10]; \
    x[3]  = rk[4 * r + 11]; \
}
#define SK(r, x) {          \
    rk[4 * r +  8]  = x[0]; \
    rk[4 * r +  9]  = x[1]; \
    rk[4 * r + 10]  = x[2]; \
    rk[4 * r + 11]  = x[3]; \
}
#define SB0(s, d, t) {     \
    t[ 1] = s[ 0] ^ s[ 3]; \
    t[ 2] = s[ 0] & s[ 3]; \
    t[ 3] = s[ 2] ^ t[ 1]; \
    t[ 6] = s[ 1] & t[ 1]; \
    t[ 4] = s[ 1] ^ t[ 3]; \
    t[10] =       ~ t[ 3]; \
    d[ 3] = t[ 2] ^ t[ 4]; \
    t[ 7] = s[ 0] ^ t[ 6]; \
    t[14] =       ~ t[ 7]; \
    t[ 8] = s[ 2] | t[ 7]; \
    t[11] = t[ 3] ^ t[ 7]; \
    d[ 2] = t[ 4] ^ t[ 8]; \
    t[12] = d[ 3] & t[11]; \
    d[ 1] = t[10] ^ t[12]; \
    d[ 0] = t[12] ^ t[14]; \
}
#define IB0(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 0] ^ s[ 1]; \
    t[ 3] = t[ 1] | t[ 2]; \
    t[ 4] = s[ 3] ^ t[ 3]; \
    t[ 7] = s[ 3] & t[ 2]; \
    t[ 5] = s[ 2] ^ t[ 4]; \
    t[ 8] = t[ 1] ^ t[ 7]; \
    d[ 2] = t[ 2] ^ t[ 5]; \
    t[11] = s[ 0] & t[ 4]; \
    t[ 9] = d[ 2] & t[ 8]; \
    t[14] = t[ 5] ^ t[ 8]; \
    d[ 1] = t[ 4] ^ t[ 9]; \
    t[12] = t[ 5] | d[ 1]; \
    d[ 3] = t[11] ^ t[12]; \
    d[ 0] = d[ 3] ^ t[14]; \
}
#define SB1(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 1] ^ t[ 1]; \
    t[ 3] = s[ 0] | t[ 2]; \
    t[ 4] = s[ 3] | t[ 2]; \
    t[ 5] = s[ 2] ^ t[ 3]; \
    d[ 2] = s[ 3] ^ t[ 5]; \
    t[ 7] = s[ 1] ^ t[ 4]; \
    t[ 8] = t[ 2] ^ d[ 2]; \
    t[ 9] = t[ 5] & t[ 7]; \
    d[ 3] = t[ 8] ^ t[ 9]; \
    t[11] = t[ 5] ^ t[ 7]; \
    d[ 1] = d[ 3] ^ t[11]; \
    t[13] = t[ 8] & t[11]; \
    d[ 0] = t[ 5] ^ t[13]; \
}
#define IB1(s, d, t) {     \
    t[ 1] = s[ 0] ^ s[ 3]; \
    t[ 2] = s[ 0] & s[ 1]; \
    t[ 3] = s[ 1] ^ s[ 2]; \
    t[ 4] = s[ 0] ^ t[ 3]; \
    t[ 5] = s[ 1] | s[ 3]; \
    t[ 7] = s[ 2] | t[ 1]; \
    d[ 3] = t[ 4] ^ t[ 5]; \
    t[ 8] = s[ 1] ^ t[ 7]; \
    t[11] =       ~ t[ 2]; \
    t[ 9] = t[ 4] & t[ 8]; \
    d[ 1] = t[ 1] ^ t[ 9]; \
    t[13] = t[ 9] ^ t[11]; \
    t[12] = d[ 3] & d[ 1]; \
    d[ 2] = t[12] ^ t[13]; \
    t[15] = s[ 0] & s[ 3]; \
    t[ 0] = s[ 2] ^ t[13]; \
    d[ 0] = t[15] ^ t[ 0]; \
}
#define SB2(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 1] ^ s[ 3]; \
    t[ 3] = s[ 2] & t[ 1]; \
    t[13] = s[ 3] | t[ 1]; \
    d[ 0] = t[ 2] ^ t[ 3]; \
    t[ 5] = s[ 2] ^ t[ 1]; \
    t[ 6] = s[ 2] ^ d[ 0]; \
    t[ 7] = s[ 1] & t[ 6]; \
    t[10] = d[ 0] | t[ 5]; \
    d[ 3] = t[ 5] ^ t[ 7]; \
    t[ 9] = s[ 3] | t[ 7]; \
    t[11] = t[ 9] & t[10]; \
    t[14] = t[ 2] ^ d[ 3]; \
    d[ 2] = s[ 0] ^ t[11]; \
    t[15] = d[ 2] ^ t[13]; \
    d[ 1] = t[14] ^ t[15]; \
}
#define IB2(s, d, t) {     \
    t[ 1] = s[ 1] ^ s[ 3]; \
    t[ 2] =       ~ t[ 1]; \
    t[ 3] = s[ 0] ^ s[ 2]; \
    t[ 4] = s[ 2] ^ t[ 1]; \
    t[ 7] = s[ 0] | t[ 2]; \
    t[ 5] = s[ 1] & t[ 4]; \
    t[ 8] = s[ 3] ^ t[ 7]; \
    t[11] =       ~ t[ 4]; \
    d[ 0] = t[ 3] ^ t[ 5]; \
    t[ 9] = t[ 3] | t[ 8]; \
    t[14] = s[ 3] & t[11]; \
    d[ 3] = t[ 1] ^ t[ 9]; \
    t[12] = d[ 0] | d[ 3]; \
    d[ 1] = t[11] ^ t[12]; \
    t[15] = t[ 3] ^ t[12]; \
    d[ 2] = t[14] ^ t[15]; \
}
#define SB3(s, d, t) {     \
    t[ 1] = s[ 0] ^ s[ 1]; \
    t[ 2] = s[ 0] & s[ 2]; \
    t[ 3] = s[ 0] | s[ 3]; \
    t[ 4] = s[ 2] ^ s[ 3]; \
    t[ 5] = t[ 1] & t[ 3]; \
    t[ 6] = t[ 2] | t[ 5]; \
    d[ 2] = t[ 4] ^ t[ 6]; \
    t[ 8] = s[ 1] ^ t[ 3]; \
    t[ 9] = t[ 6] ^ t[ 8]; \
    t[10] = t[ 4] & t[ 9]; \
    d[ 0] = t[ 1] ^ t[10]; \
    t[12] = d[ 2] & d[ 0]; \
    d[ 1] = t[ 9] ^ t[12]; \
    t[14] = s[ 1] | s[ 3]; \
    t[15] = t[ 4] ^ t[12]; \
    d[ 3] = t[14] ^ t[15]; \
}
#define IB3(s, d, t) {     \
    t[ 1] = s[ 1] ^ s[ 2]; \
    t[ 2] = s[ 1] | s[ 2]; \
    t[ 3] = s[ 0] ^ s[ 2]; \
    t[ 7] = s[ 0] ^ s[ 3]; \
    t[ 4] = t[ 2] ^ t[ 3]; \
    t[ 5] = s[ 3] | t[ 4]; \
    t[ 9] = t[ 2] ^ t[ 7]; \
    d[ 0] = t[ 1] ^ t[ 5]; \
    t[ 8] = t[ 1] | t[ 5]; \
    t[11] = s[ 0] & t[ 4]; \
    d[ 2] = t[ 8] ^ t[ 9]; \
    t[12] = d[ 0] | t[ 9]; \
    d[ 1] = t[11] ^ t[12]; \
    t[14] = s[ 0] & d[ 2]; \
    t[15] = t[ 2] ^ t[14]; \
    t[ 0] = d[ 0] & t[15]; \
    d[ 3] = t[ 4] ^ t[ 0]; \
}
#define SB4(s, d, t) {     \
    t[ 1] = s[ 0] ^ s[ 3]; \
    t[ 2] = s[ 3] & t[ 1]; \
    t[ 3] = s[ 2] ^ t[ 2]; \
    t[ 4] = s[ 1] | t[ 3]; \
    d[ 3] = t[ 1] ^ t[ 4]; \
    t[ 6] =       ~ s[ 1]; \
    t[ 7] = t[ 1] | t[ 6]; \
    d[ 0] = t[ 3] ^ t[ 7]; \
    t[ 9] = s[ 0] & d[ 0]; \
    t[10] = t[ 1] ^ t[ 6]; \
    t[11] = t[ 4] & t[10]; \
    d[ 2] = t[ 9] ^ t[11]; \
    t[13] = s[ 0] ^ t[ 3]; \
    t[14] = t[10] & d[ 2]; \
    d[ 1] = t[13] ^ t[14]; \
}
#define IB4(s, d, t) {     \
    t[ 1] = s[ 2] ^ s[ 3]; \
    t[ 2] = s[ 2] | s[ 3]; \
    t[ 3] = s[ 1] ^ t[ 2]; \
    t[ 4] = s[ 0] & t[ 3]; \
    d[ 1] = t[ 1] ^ t[ 4]; \
    t[ 6] = s[ 0] ^ s[ 3]; \
    t[ 7] = s[ 1] | s[ 3]; \
    t[ 8] = t[ 6] & t[ 7]; \
    d[ 3] = t[ 3] ^ t[ 8]; \
    t[10] =       ~ s[ 0]; \
    t[11] = s[ 2] ^ d[ 3]; \
    t[12] = t[10] | t[11]; \
    d[ 0] = t[ 3] ^ t[12]; \
    t[14] = s[ 2] | t[ 4]; \
    t[15] = t[ 7] ^ t[14]; \
    t[ 0] = d[ 3] | t[10]; \
    d[ 2] = t[15] ^ t[ 0]; \
}
#define SB5(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 0] ^ s[ 1]; \
    t[ 3] = s[ 0] ^ s[ 3]; \
    t[ 4] = s[ 2] ^ t[ 1]; \
    t[ 5] = t[ 2] | t[ 3]; \
    d[ 0] = t[ 4] ^ t[ 5]; \
    t[ 7] = s[ 3] & d[ 0]; \
    t[ 8] = t[ 2] ^ d[ 0]; \
    t[10] = t[ 1] | d[ 0]; \
    d[ 1] = t[ 7] ^ t[ 8]; \
    t[11] = t[ 2] | t[ 7]; \
    t[12] = t[ 3] ^ t[10]; \
    t[14] = s[ 1] ^ t[ 7]; \
    d[ 2] = t[11] ^ t[12]; \
    t[15] = d[ 1] & t[12]; \
    d[ 3] = t[14] ^ t[15]; \
}
#define IB5(s, d, t) {     \
    t[ 1] =       ~ s[ 2]; \
    t[ 2] = s[ 1] & t[ 1]; \
    t[ 3] = s[ 3] ^ t[ 2]; \
    t[ 4] = s[ 0] & t[ 3]; \
    t[ 5] = s[ 1] ^ t[ 1]; \
    d[ 3] = t[ 4] ^ t[ 5]; \
    t[ 7] = s[ 1] | d[ 3]; \
    t[ 8] = s[ 0] & t[ 7]; \
    d[ 1] = t[ 3] ^ t[ 8]; \
    t[10] = s[ 0] | s[ 3]; \
    t[11] = t[ 1] ^ t[ 7]; \
    d[ 0] = t[10] ^ t[11]; \
    t[13] = s[ 0] ^ s[ 2]; \
    t[14] = s[ 1] & t[10]; \
    t[15] = t[ 4] | t[13]; \
    d[ 2] = t[14] ^ t[15]; \
}
#define SB6(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 0] ^ s[ 3]; \
    t[ 3] = s[ 1] ^ t[ 2]; \
    t[ 4] = t[ 1] | t[ 2]; \
    t[ 5] = s[ 2] ^ t[ 4]; \
    d[ 1] = s[ 1] ^ t[ 5]; \
    t[13] =       ~ t[ 5]; \
    t[ 7] = t[ 2] | d[ 1]; \
    t[ 8] = s[ 3] ^ t[ 7]; \
    t[ 9] = t[ 5] & t[ 8]; \
    d[ 2] = t[ 3] ^ t[ 9]; \
    t[11] = t[ 5] ^ t[ 8]; \
    d[ 0] = d[ 2] ^ t[11]; \
    t[14] = t[ 3] & t[11]; \
    d[ 3] = t[13] ^ t[14]; \
}
#define IB6(s, d, t) {     \
    t[ 1] =       ~ s[ 0]; \
    t[ 2] = s[ 0] ^ s[ 1]; \
    t[ 3] = s[ 2] ^ t[ 2]; \
    t[ 4] = s[ 2] | t[ 1]; \
    t[ 5] = s[ 3] ^ t[ 4]; \
    t[13] = s[ 3] & t[ 1]; \
    d[ 1] = t[ 3] ^ t[ 5]; \
    t[ 7] = t[ 3] & t[ 5]; \
    t[ 8] = t[ 2] ^ t[ 7]; \
    t[ 9] = s[ 1] | t[ 8]; \
    d[ 3] = t[ 5] ^ t[ 9]; \
    t[11] = s[ 1] | d[ 3]; \
    d[ 0] = t[ 8] ^ t[11]; \
    t[14] = t[ 3] ^ t[11]; \
    d[ 2] = t[13] ^ t[14]; \
}
#define SB7(s, d, t) {     \
    t[ 1] =       ~ s[ 2]; \
    t[ 2] = s[ 1] ^ s[ 2]; \
    t[ 3] = s[ 1] | t[ 1]; \
    t[ 4] = s[ 3] ^ t[ 3]; \
    t[ 5] = s[ 0] & t[ 4]; \
    t[ 7] = s[ 0] ^ s[ 3]; \
    d[ 3] = t[ 2] ^ t[ 5]; \
    t[ 8] = s[ 1] ^ t[ 5]; \
    t[ 9] = t[ 2] | t[ 8]; \
    t[11] = s[ 3] & t[ 3]; \
    d[ 1] = t[ 7] ^ t[ 9]; \
    t[12] = t[ 5] ^ d[ 1]; \
    t[15] = t[ 1] | t[ 4]; \
    t[13] = d[ 3] & t[12]; \
    d[ 2] = t[11] ^ t[13]; \
    t[ 0] = t[12] ^ d[ 2]; \
    d[ 0] = t[15] ^ t[ 0]; \
}
#define IB7(s, d, t) {     \
    t[ 1] = s[ 0] & s[ 1]; \
    t[ 2] = s[ 0] | s[ 1]; \
    t[ 3] = s[ 2] | t[ 1]; \
    t[ 4] = s[ 3] & t[ 2]; \
    d[ 3] = t[ 3] ^ t[ 4]; \
    t[ 6] =       ~ s[ 3]; \
    t[ 7] = s[ 1] ^ t[ 4]; \
    t[ 8] = d[ 3] ^ t[ 6]; \
    t[11] = s[ 2] ^ t[ 7]; \
    t[ 9] = t[ 7] | t[ 8]; \
    d[ 1] = s[ 0] ^ t[ 9]; \
    t[12] = s[ 3] | d[ 1]; \
    d[ 0] = t[11] ^ t[12]; \
    t[14] = s[ 0] & d[ 3]; \
    t[15] = t[ 3] ^ d[ 1]; \
    t[ 0] = d[ 0] ^ t[14]; \
    d[ 2] = t[15] ^ t[ 0]; \
}
class SerpentBase {};
template <int L>
requires (L == 4 || L == 6 || L == 8)
class SerpentTmpl: public SerpentBase {
    uint32_t rk[140] = {};
public:
    static constexpr size_t BLOCK_SIZE = 16;
    SerpentTmpl(uint8_t const *kin) {
        uint32_t x[4], y[4], t[16];
        READ_LE(rk, kin, L);
        if constexpr (L < 8) {
            rk[L] = 1; // padding with 1 bit and 0 bits to make 256 bits
        }
        for (int i = 0; i < 132; ++i) {
            rk[i + 8] = ROTL(rk[i] ^ rk[i + 3] ^ rk[i + 5] ^ rk[i + 7] ^ 0x9E3779B9 ^ i, 11);
        }
        LK( 0, x); SB3(x, y, t); SK( 0, y);
        LK( 1, x); SB2(x, y, t); SK( 1, y);
        LK( 2, x); SB1(x, y, t); SK( 2, y);
        LK( 3, x); SB0(x, y, t); SK( 3, y);
        LK( 4, x); SB7(x, y, t); SK( 4, y);
        LK( 5, x); SB6(x, y, t); SK( 5, y);
        LK( 6, x); SB5(x, y, t); SK( 6, y);
        LK( 7, x); SB4(x, y, t); SK( 7, y);
        LK( 8, x); SB3(x, y, t); SK( 8, y);
        LK( 9, x); SB2(x, y, t); SK( 9, y);
        LK(10, x); SB1(x, y, t); SK(10, y);
        LK(11, x); SB0(x, y, t); SK(11, y);
        LK(12, x); SB7(x, y, t); SK(12, y);
        LK(13, x); SB6(x, y, t); SK(13, y);
        LK(14, x); SB5(x, y, t); SK(14, y);
        LK(15, x); SB4(x, y, t); SK(15, y);
        LK(16, x); SB3(x, y, t); SK(16, y);
        LK(17, x); SB2(x, y, t); SK(17, y);
        LK(18, x); SB1(x, y, t); SK(18, y);
        LK(19, x); SB0(x, y, t); SK(19, y);
        LK(20, x); SB7(x, y, t); SK(20, y);
        LK(21, x); SB6(x, y, t); SK(21, y);
        LK(22, x); SB5(x, y, t); SK(22, y);
        LK(23, x); SB4(x, y, t); SK(23, y);
        LK(24, x); SB3(x, y, t); SK(24, y);
        LK(25, x); SB2(x, y, t); SK(25, y);
        LK(26, x); SB1(x, y, t); SK(26, y);
        LK(27, x); SB0(x, y, t); SK(27, y);
        LK(28, x); SB7(x, y, t); SK(28, y);
        LK(29, x); SB6(x, y, t); SK(29, y);
        LK(30, x); SB5(x, y, t); SK(30, y);
        LK(31, x); SB4(x, y, t); SK(31, y);
        LK(32, x); SB3(x, y, t); SK(32, y);
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t x[4], y[4], t[16];
        READ_LE(x, src, 4);
        XK( 0, x); SB0(x, y, t); XXROTL(y);
        XK( 1, y); SB1(y, x, t); XXROTL(x);
        XK( 2, x); SB2(x, y, t); XXROTL(y);
        XK( 3, y); SB3(y, x, t); XXROTL(x);
        XK( 4, x); SB4(x, y, t); XXROTL(y);
        XK( 5, y); SB5(y, x, t); XXROTL(x);
        XK( 6, x); SB6(x, y, t); XXROTL(y);
        XK( 7, y); SB7(y, x, t); XXROTL(x);
        XK( 8, x); SB0(x, y, t); XXROTL(y);
        XK( 9, y); SB1(y, x, t); XXROTL(x);
        XK(10, x); SB2(x, y, t); XXROTL(y);
        XK(11, y); SB3(y, x, t); XXROTL(x);
        XK(12, x); SB4(x, y, t); XXROTL(y);
        XK(13, y); SB5(y, x, t); XXROTL(x);
        XK(14, x); SB6(x, y, t); XXROTL(y);
        XK(15, y); SB7(y, x, t); XXROTL(x);
        XK(16, x); SB0(x, y, t); XXROTL(y);
        XK(17, y); SB1(y, x, t); XXROTL(x);
        XK(18, x); SB2(x, y, t); XXROTL(y);
        XK(19, y); SB3(y, x, t); XXROTL(x);
        XK(20, x); SB4(x, y, t); XXROTL(y);
        XK(21, y); SB5(y, x, t); XXROTL(x);
        XK(22, x); SB6(x, y, t); XXROTL(y);
        XK(23, y); SB7(y, x, t); XXROTL(x);
        XK(24, x); SB0(x, y, t); XXROTL(y);
        XK(25, y); SB1(y, x, t); XXROTL(x);
        XK(26, x); SB2(x, y, t); XXROTL(y);
        XK(27, y); SB3(y, x, t); XXROTL(x);
        XK(28, x); SB4(x, y, t); XXROTL(y);
        XK(29, y); SB5(y, x, t); XXROTL(x);
        XK(30, x); SB6(x, y, t); XXROTL(y);
        XK(31, y); SB7(y, x, t); XK(32, x);
        WRITE_LE(dst, x, 4);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t x[4], y[4], t[16];
        READ_LE(x, src, 4);
        XK(32, x); IB7(x, y, t); XK(31, y);
        XXROTR(y); IB6(y, x, t); XK(30, x);
        XXROTR(x); IB5(x, y, t); XK(29, y);
        XXROTR(y); IB4(y, x, t); XK(28, x);
        XXROTR(x); IB3(x, y, t); XK(27, y);
        XXROTR(y); IB2(y, x, t); XK(26, x);
        XXROTR(x); IB1(x, y, t); XK(25, y);
        XXROTR(y); IB0(y, x, t); XK(24, x);
        XXROTR(x); IB7(x, y, t); XK(23, y);
        XXROTR(y); IB6(y, x, t); XK(22, x);
        XXROTR(x); IB5(x, y, t); XK(21, y);
        XXROTR(y); IB4(y, x, t); XK(20, x);
        XXROTR(x); IB3(x, y, t); XK(19, y);
        XXROTR(y); IB2(y, x, t); XK(18, x);
        XXROTR(x); IB1(x, y, t); XK(17, y);
        XXROTR(y); IB0(y, x, t); XK(16, x);
        XXROTR(x); IB7(x, y, t); XK(15, y);
        XXROTR(y); IB6(y, x, t); XK(14, x);
        XXROTR(x); IB5(x, y, t); XK(13, y);
        XXROTR(y); IB4(y, x, t); XK(12, x);
        XXROTR(x); IB3(x, y, t); XK(11, y);
        XXROTR(y); IB2(y, x, t); XK(10, x);
        XXROTR(x); IB1(x, y, t); XK( 9, y);
        XXROTR(y); IB0(y, x, t); XK( 8, x);
        XXROTR(x); IB7(x, y, t); XK( 7, y);
        XXROTR(y); IB6(y, x, t); XK( 6, x);
        XXROTR(x); IB5(x, y, t); XK( 5, y);
        XXROTR(y); IB4(y, x, t); XK( 4, x);
        XXROTR(x); IB3(x, y, t); XK( 3, y);
        XXROTR(y); IB2(y, x, t); XK( 2, x);
        XXROTR(x); IB1(x, y, t); XK( 1, y);
        XXROTR(y); IB0(y, x, t); XK( 0, x);
        WRITE_LE(dst, x, 4);
    }
};
using Serpent128 = SerpentTmpl<4>;
using Serpent192 = SerpentTmpl<6>;
using Serpent256 = SerpentTmpl<8>;
#undef LK
#undef SK
#undef XK
#undef XXROTR
#undef XXROTL
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
