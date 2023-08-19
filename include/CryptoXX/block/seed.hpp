#pragma once
#include "../utils.hpp"
class SEED {
    static constexpr uint32_t KC[16] = {
        0x9e3779b9, 0x3c6ef373, 0x78dde6e6, 0xf1bbcdcc,
        0xe3779b99, 0xc6ef3733, 0x8dde6e67, 0x1bbcdccf,
        0x3779b99e, 0x6ef3733c, 0xdde6e678, 0xbbcdccf1,
        0x779b99e3, 0xef3733c6, 0xde6e678d, 0xbcdccf1b,
    };
    static constexpr uint8_t S_BOX[2][256] = {
        0xA9, 0x85, 0xD6, 0xD3, 0x54, 0x1D, 0xAC, 0x25, 0x5D, 0x43, 0x18, 0x1E, 0x51, 0xFC, 0xCA, 0x63, 0x28,
        0x44, 0x20, 0x9D, 0xE0, 0xE2, 0xC8, 0x17, 0xA5, 0x8F, 0x03, 0x7B, 0xBB, 0x13, 0xD2, 0xEE, 0x70, 0x8C,
        0x3F, 0xA8, 0x32, 0xDD, 0xF6, 0x74, 0xEC, 0x95, 0x0B, 0x57, 0x5C, 0x5B, 0xBD, 0x01, 0x24, 0x1C, 0x73,
        0x98, 0x10, 0xCC, 0xF2, 0xD9, 0x2C, 0xE7, 0x72, 0x83, 0x9B, 0xD1, 0x86, 0xC9, 0x60, 0x50, 0xA3, 0xEB,
        0x0D, 0xB6, 0x9E, 0x4F, 0xB7, 0x5A, 0xC6, 0x78, 0xA6, 0x12, 0xAF, 0xD5, 0x61, 0xC3, 0xB4, 0x41, 0x52,
        0x7D, 0x8D, 0x08, 0x1F, 0x99, 0x00, 0x19, 0x04, 0x53, 0xF7, 0xE1, 0xFD, 0x76, 0x2F, 0x27, 0xB0, 0x8B,
        0x0E, 0xAB, 0xA2, 0x6E, 0x93, 0x4D, 0x69, 0x7C, 0x09, 0x0A, 0xBF, 0xEF, 0xF3, 0xC5, 0x87, 0x14, 0xFE,
        0x64, 0xDE, 0x2E, 0x4B, 0x1A, 0x06, 0x21, 0x6B, 0x66, 0x02, 0xF5, 0x92, 0x8A, 0x0C, 0xB3, 0x7E, 0xD0,
        0x7A, 0x47, 0x96, 0xE5, 0x26, 0x80, 0xAD, 0xDF, 0xA1, 0x30, 0x37, 0xAE, 0x36, 0x15, 0x22, 0x38, 0xF4,
        0xA7, 0x45, 0x4C, 0x81, 0xE9, 0x84, 0x97, 0x35, 0xCB, 0xCE, 0x3C, 0x71, 0x11, 0xC7, 0x89, 0x75, 0xFB,
        0xDA, 0xF8, 0x94, 0x59, 0x82, 0xC4, 0xFF, 0x49, 0x39, 0x67, 0xC0, 0xCF, 0xD7, 0xB8, 0x0F, 0x8E, 0x42,
        0x23, 0x91, 0x6C, 0xDB, 0xA4, 0x34, 0xF1, 0x48, 0xC2, 0x6F, 0x3D, 0x2D, 0x40, 0xBE, 0x3E, 0xBC, 0xC1,
        0xAA, 0xBA, 0x4E, 0x55, 0x3B, 0xDC, 0x68, 0x7F, 0x9C, 0xD8, 0x4A, 0x56, 0x77, 0xA0, 0xED, 0x46, 0xB5,
        0x2B, 0x65, 0xFA, 0xE3, 0xB9, 0xB1, 0x9F, 0x5E, 0xF9, 0xE6, 0xB2, 0x31, 0xEA, 0x6D, 0x5F, 0xE4, 0xF0,
        0xCD, 0x88, 0x16, 0x3A, 0x58, 0xD4, 0x62, 0x29, 0x07, 0x33, 0xE8, 0x1B, 0x05, 0x79, 0x90, 0x6A, 0x2A,
        0x9A,
        0x38, 0xE8, 0x2D, 0xA6, 0xCF, 0xDE, 0xB3, 0xB8, 0xAF, 0x60, 0x55, 0xC7, 0x44, 0x6F, 0x6B, 0x5B, 0xC3,
        0x62, 0x33, 0xB5, 0x29, 0xA0, 0xE2, 0xA7, 0xD3, 0x91, 0x11, 0x06, 0x1C, 0xBC, 0x36, 0x4B, 0xEF, 0x88,
        0x6C, 0xA8, 0x17, 0xC4, 0x16, 0xF4, 0xC2, 0x45, 0xE1, 0xD6, 0x3F, 0x3D, 0x8E, 0x98, 0x28, 0x4E, 0xF6,
        0x3E, 0xA5, 0xF9, 0x0D, 0xDF, 0xD8, 0x2B, 0x66, 0x7A, 0x27, 0x2F, 0xF1, 0x72, 0x42, 0xD4, 0x41, 0xC0,
        0x73, 0x67, 0xAC, 0x8B, 0xF7, 0xAD, 0x80, 0x1F, 0xCA, 0x2C, 0xAA, 0x34, 0xD2, 0x0B, 0xEE, 0xE9, 0x5D,
        0x94, 0x18, 0xF8, 0x57, 0xAE, 0x08, 0xC5, 0x13, 0xCD, 0x86, 0xB9, 0xFF, 0x7D, 0xC1, 0x31, 0xF5, 0x8A,
        0x6A, 0xB1, 0xD1, 0x20, 0xD7, 0x02, 0x22, 0x04, 0x68, 0x71, 0x07, 0xDB, 0x9D, 0x99, 0x61, 0xBE, 0xE6,
        0x59, 0xDD, 0x51, 0x90, 0xDC, 0x9A, 0xA3, 0xAB, 0xD0, 0x81, 0x0F, 0x47, 0x1A, 0xE3, 0xEC, 0x8D, 0xBF,
        0x96, 0x7B, 0x5C, 0xA2, 0xA1, 0x63, 0x23, 0x4D, 0xC8, 0x9E, 0x9C, 0x3A, 0x0C, 0x2E, 0xBA, 0x6E, 0x9F,
        0x5A, 0xF2, 0x92, 0xF3, 0x49, 0x78, 0xCC, 0x15, 0xFB, 0x70, 0x75, 0x7F, 0x35, 0x10, 0x03, 0x64, 0x6D,
        0xC6, 0x74, 0xD5, 0xB4, 0xEA, 0x09, 0x76, 0x19, 0xFE, 0x40, 0x12, 0xE0, 0xBD, 0x05, 0xFA, 0x01, 0xF0,
        0x2A, 0x5E, 0xA9, 0x56, 0x43, 0x85, 0x14, 0x89, 0x9B, 0xB0, 0xE5, 0x48, 0x79, 0x97, 0xFC, 0x1E, 0x82,
        0x21, 0x8C, 0x1B, 0x5F, 0x77, 0x54, 0xB2, 0x1D, 0x25, 0x4F, 0x00, 0x46, 0xED, 0x58, 0x52, 0xEB, 0x7E,
        0xDA, 0xC9, 0xFD, 0x30, 0x95, 0x65, 0x3C, 0xB6, 0xE4, 0xBB, 0x7C, 0x0E, 0x50, 0x39, 0x26, 0x32, 0x84,
        0x69, 0x93, 0x37, 0xE7, 0x24, 0xA4, 0xCB, 0x53, 0x0A, 0x87, 0xD9, 0x4C, 0x83, 0x8F, 0xCE, 0x3B, 0x4A,
        0xB7,
    };
    static constexpr uint8_t M[4] = {
        0xFC, 0xF3, 0xCF, 0x3F,
    };
    static constexpr auto SSBOX = []() {
        std::array<std::array<uint32_t, 256>, 4> SSBOX;
        for (size_t i = 0; i < 256; ++i) {
            SSBOX[0][i] = (S_BOX[0][i] & M[3]) << 24 | (S_BOX[0][i] & M[2]) << 16 | (S_BOX[0][i] & M[1]) << 8 | S_BOX[0][i] & M[0];
            SSBOX[1][i] = (S_BOX[1][i] & M[0]) << 24 | (S_BOX[1][i] & M[3]) << 16 | (S_BOX[1][i] & M[2]) << 8 | S_BOX[1][i] & M[1];
            SSBOX[2][i] = (S_BOX[0][i] & M[1]) << 24 | (S_BOX[0][i] & M[0]) << 16 | (S_BOX[0][i] & M[3]) << 8 | S_BOX[0][i] & M[2];
            SSBOX[3][i] = (S_BOX[1][i] & M[2]) << 24 | (S_BOX[1][i] & M[1]) << 16 | (S_BOX[1][i] & M[0]) << 8 | S_BOX[1][i] & M[3];
        }
        return SSBOX;
    }();
    static uint32_t g(uint32_t x) {
        return SSBOX[0][x & 0xff] ^ SSBOX[1][x >> 8 & 0xff] ^ SSBOX[2][x >> 16 & 0xff] ^ SSBOX[3][x >> 24];
    }
    uint32_t k[32];
public:
    static constexpr size_t BLOCK_SIZE = 16;
    static constexpr size_t KEY_SIZE = 16;
    SEED(uint8_t const *mk) {
        uint32_t t0, t1;
        uint32_t k0 = GET_BE<uint32_t>(mk +  0);
        uint32_t k1 = GET_BE<uint32_t>(mk +  4);
        uint32_t q0 = GET_BE<uint32_t>(mk +  8);
        uint32_t q1 = GET_BE<uint32_t>(mk + 12);
        for (int i = 0; i < 32; i += 4) {
            k[i +  0] = g(k0 + q0 - KC[i / 2 + 0]);
            k[i +  1] = g(k1 - q1 + KC[i / 2 + 0]);
            t0 = k0 >> 8 | k1 << 24;
            t1 = k1 >> 8 | k0 << 24;
            k0 = t0;
            k1 = t1;
            k[i +  2] = g(k0 + q0 - KC[i / 2 + 1]);
            k[i +  3] = g(k1 - q1 + KC[i / 2 + 1]);
            t0 = q0 << 8 | q1 >> 24;
            t1 = q1 << 8 | q0 >> 24;
            q0 = t0;
            q1 = t1;
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t t0, t1;
        uint32_t l0 = GET_BE<uint32_t>(src +  0);
        uint32_t l1 = GET_BE<uint32_t>(src +  4);
        uint32_t r0 = GET_BE<uint32_t>(src +  8);
        uint32_t r1 = GET_BE<uint32_t>(src + 12);
        for (int i = 0; i < 32; i += 4) {
            t1 = r1 ^ k[i +  1];
            t0 = r0 ^ k[i +  0];
            t1 = g(t0 ^ t1);
            t0 = g(t1 + t0);
            t1 = g(t0 + t1);
            t0 += t1;
            l1 ^= t1;
            l0 ^= t0;
            t1 = l1 ^ k[i +  3];
            t0 = l0 ^ k[i +  2];
            t1 = g(t0 ^ t1);
            t0 = g(t1 + t0);
            t1 = g(t0 + t1);
            t0 += t1;
            r1 ^= t1;
            r0 ^= t0;
        }
        PUT_BE(dst +  0, r0);
        PUT_BE(dst +  4, r1);
        PUT_BE(dst +  8, l0);
        PUT_BE(dst + 12, l1);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        uint32_t t0, t1;
        uint32_t l0 = GET_BE<uint32_t>(src +  0);
        uint32_t l1 = GET_BE<uint32_t>(src +  4);
        uint32_t r0 = GET_BE<uint32_t>(src +  8);
        uint32_t r1 = GET_BE<uint32_t>(src + 12);
        for (int i = 0; i < 32; i += 4) {
            t1 = r1 ^ k[31 - i];
            t0 = r0 ^ k[30 - i];
            t1 = g(t0 ^ t1);
            t0 = g(t1 + t0);
            t1 = g(t0 + t1);
            t0 += t1;
            l1 ^= t1;
            l0 ^= t0;
            t1 = l1 ^ k[29 - i];
            t0 = l0 ^ k[28 - i];
            t1 = g(t0 ^ t1);
            t0 = g(t1 + t0);
            t1 = g(t0 + t1);
            t0 += t1;
            r1 ^= t1;
            r0 ^= t0;
        }
        PUT_BE(dst +  0, r0);
        PUT_BE(dst +  4, r1);
        PUT_BE(dst +  8, l0);
        PUT_BE(dst + 12, l1);
    }
};
