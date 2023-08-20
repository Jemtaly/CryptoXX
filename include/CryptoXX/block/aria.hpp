#pragma once
#include "../utils.hpp"
struct ARIAWord {
    uint64_t hi = 0;
    uint64_t lo = 0;
    constexpr uint8_t operator[](int n) const {
        return n < 8 ? hi >> (56 - (n - 0) * 8) : lo >> (56 - (n - 8) * 8);
    }
    constexpr ARIAWord rotl(bits_t n) const { // 0 < n < 64
        return {hi << (n & 63) | lo >> (-n & 63), lo << (n & 63) | hi >> (-n & 63)};
    }
    constexpr ARIAWord rotr(bits_t n) const { // 0 < n < 64
        return {hi >> (n & 63) | lo << (-n & 63), lo >> (n & 63) | hi << (-n & 63)};
    }
    friend constexpr ARIAWord operator^(const ARIAWord& a, const ARIAWord& b) {
        return {a.hi ^ b.hi, a.lo ^ b.lo};
    }
};
class ARIABase {
protected:
    static constexpr ARIAWord C[3] = {
        0x517cc1b727220a94, 0xfe13abe8fa9a6ee0,
        0x6db14acc9e21c820, 0xff28b1d5ef5de2b0,
        0xdb92371d2126e970, 0x0324977504e8c90e,
    };
    static constexpr uint8_t S_BOX[4][256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
        0xe2, 0x4e, 0x54, 0xfc, 0x94, 0xc2, 0x4a, 0xcc, 0x62, 0x0d, 0x6a, 0x46, 0x3c, 0x4d, 0x8b, 0xd1,
        0x5e, 0xfa, 0x64, 0xcb, 0xb4, 0x97, 0xbe, 0x2b, 0xbc, 0x77, 0x2e, 0x03, 0xd3, 0x19, 0x59, 0xc1,
        0x1d, 0x06, 0x41, 0x6b, 0x55, 0xf0, 0x99, 0x69, 0xea, 0x9c, 0x18, 0xae, 0x63, 0xdf, 0xe7, 0xbb,
        0x00, 0x73, 0x66, 0xfb, 0x96, 0x4c, 0x85, 0xe4, 0x3a, 0x09, 0x45, 0xaa, 0x0f, 0xee, 0x10, 0xeb,
        0x2d, 0x7f, 0xf4, 0x29, 0xac, 0xcf, 0xad, 0x91, 0x8d, 0x78, 0xc8, 0x95, 0xf9, 0x2f, 0xce, 0xcd,
        0x08, 0x7a, 0x88, 0x38, 0x5c, 0x83, 0x2a, 0x28, 0x47, 0xdb, 0xb8, 0xc7, 0x93, 0xa4, 0x12, 0x53,
        0xff, 0x87, 0x0e, 0x31, 0x36, 0x21, 0x58, 0x48, 0x01, 0x8e, 0x37, 0x74, 0x32, 0xca, 0xe9, 0xb1,
        0xb7, 0xab, 0x0c, 0xd7, 0xc4, 0x56, 0x42, 0x26, 0x07, 0x98, 0x60, 0xd9, 0xb6, 0xb9, 0x11, 0x40,
        0xec, 0x20, 0x8c, 0xbd, 0xa0, 0xc9, 0x84, 0x04, 0x49, 0x23, 0xf1, 0x4f, 0x50, 0x1f, 0x13, 0xdc,
        0xd8, 0xc0, 0x9e, 0x57, 0xe3, 0xc3, 0x7b, 0x65, 0x3b, 0x02, 0x8f, 0x3e, 0xe8, 0x25, 0x92, 0xe5,
        0x15, 0xdd, 0xfd, 0x17, 0xa9, 0xbf, 0xd4, 0x9a, 0x7e, 0xc5, 0x39, 0x67, 0xfe, 0x76, 0x9d, 0x43,
        0xa7, 0xe1, 0xd0, 0xf5, 0x68, 0xf2, 0x1b, 0x34, 0x70, 0x05, 0xa3, 0x8a, 0xd5, 0x79, 0x86, 0xa8,
        0x30, 0xc6, 0x51, 0x4b, 0x1e, 0xa6, 0x27, 0xf6, 0x35, 0xd2, 0x6e, 0x24, 0x16, 0x82, 0x5f, 0xda,
        0xe6, 0x75, 0xa2, 0xef, 0x2c, 0xb2, 0x1c, 0x9f, 0x5d, 0x6f, 0x80, 0x0a, 0x72, 0x44, 0x9b, 0x6c,
        0x90, 0x0b, 0x5b, 0x33, 0x7d, 0x5a, 0x52, 0xf3, 0x61, 0xa1, 0xf7, 0xb0, 0xd6, 0x3f, 0x7c, 0x6d,
        0xed, 0x14, 0xe0, 0xa5, 0x3d, 0x22, 0xb3, 0xf8, 0x89, 0xde, 0x71, 0x1a, 0xaf, 0xba, 0xb5, 0x81,
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
        0x30, 0x68, 0x99, 0x1b, 0x87, 0xb9, 0x21, 0x78, 0x50, 0x39, 0xdb, 0xe1, 0x72, 0x09, 0x62, 0x3c,
        0x3e, 0x7e, 0x5e, 0x8e, 0xf1, 0xa0, 0xcc, 0xa3, 0x2a, 0x1d, 0xfb, 0xb6, 0xd6, 0x20, 0xc4, 0x8d,
        0x81, 0x65, 0xf5, 0x89, 0xcb, 0x9d, 0x77, 0xc6, 0x57, 0x43, 0x56, 0x17, 0xd4, 0x40, 0x1a, 0x4d,
        0xc0, 0x63, 0x6c, 0xe3, 0xb7, 0xc8, 0x64, 0x6a, 0x53, 0xaa, 0x38, 0x98, 0x0c, 0xf4, 0x9b, 0xed,
        0x7f, 0x22, 0x76, 0xaf, 0xdd, 0x3a, 0x0b, 0x58, 0x67, 0x88, 0x06, 0xc3, 0x35, 0x0d, 0x01, 0x8b,
        0x8c, 0xc2, 0xe6, 0x5f, 0x02, 0x24, 0x75, 0x93, 0x66, 0x1e, 0xe5, 0xe2, 0x54, 0xd8, 0x10, 0xce,
        0x7a, 0xe8, 0x08, 0x2c, 0x12, 0x97, 0x32, 0xab, 0xb4, 0x27, 0x0a, 0x23, 0xdf, 0xef, 0xca, 0xd9,
        0xb8, 0xfa, 0xdc, 0x31, 0x6b, 0xd1, 0xad, 0x19, 0x49, 0xbd, 0x51, 0x96, 0xee, 0xe4, 0xa8, 0x41,
        0xda, 0xff, 0xcd, 0x55, 0x86, 0x36, 0xbe, 0x61, 0x52, 0xf8, 0xbb, 0x0e, 0x82, 0x48, 0x69, 0x9a,
        0xe0, 0x47, 0x9e, 0x5c, 0x04, 0x4b, 0x34, 0x15, 0x79, 0x26, 0xa7, 0xde, 0x29, 0xae, 0x92, 0xd7,
        0x84, 0xe9, 0xd2, 0xba, 0x5d, 0xf3, 0xc5, 0xb0, 0xbf, 0xa4, 0x3b, 0x71, 0x44, 0x46, 0x2b, 0xfc,
        0xeb, 0x6f, 0xd5, 0xf6, 0x14, 0xfe, 0x7c, 0x70, 0x5a, 0x7d, 0xfd, 0x2f, 0x18, 0x83, 0x16, 0xa5,
        0x91, 0x1f, 0x05, 0x95, 0x74, 0xa9, 0xc1, 0x5b, 0x4a, 0x85, 0x6d, 0x13, 0x07, 0x4f, 0x4e, 0x45,
        0xb2, 0x0f, 0xc9, 0x1c, 0xa6, 0xbc, 0xec, 0x73, 0x90, 0x7b, 0xcf, 0x59, 0x8f, 0xa1, 0xf9, 0x2d,
        0xf2, 0xb1, 0x00, 0x94, 0x37, 0x9f, 0xd0, 0x2e, 0x9c, 0x6e, 0x28, 0x3f, 0x80, 0xf0, 0x3d, 0xd3,
        0x25, 0x8a, 0xb5, 0xe7, 0x42, 0xb3, 0xc7, 0xea, 0xf7, 0x4c, 0x11, 0x33, 0x03, 0xa2, 0xac, 0x60,
    };
    static ARIAWord sl1(ARIAWord const &x) {
        return {
            (uint64_t)S_BOX[0][x[ 0]] << 56 | (uint64_t)S_BOX[1][x[ 1]] << 48 |
            (uint64_t)S_BOX[2][x[ 2]] << 40 | (uint64_t)S_BOX[3][x[ 3]] << 32 |
            (uint64_t)S_BOX[0][x[ 4]] << 24 | (uint64_t)S_BOX[1][x[ 5]] << 16 |
            (uint64_t)S_BOX[2][x[ 6]] <<  8 | (uint64_t)S_BOX[3][x[ 7]]      ,
            (uint64_t)S_BOX[0][x[ 8]] << 56 | (uint64_t)S_BOX[1][x[ 9]] << 48 |
            (uint64_t)S_BOX[2][x[10]] << 40 | (uint64_t)S_BOX[3][x[11]] << 32 |
            (uint64_t)S_BOX[0][x[12]] << 24 | (uint64_t)S_BOX[1][x[13]] << 16 |
            (uint64_t)S_BOX[2][x[14]] <<  8 | (uint64_t)S_BOX[3][x[15]]      ,
        };
    }
    static ARIAWord sl2(ARIAWord const &x) {
        return {
            (uint64_t)S_BOX[2][x[ 0]] << 56 | (uint64_t)S_BOX[3][x[ 1]] << 48 |
            (uint64_t)S_BOX[0][x[ 2]] << 40 | (uint64_t)S_BOX[1][x[ 3]] << 32 |
            (uint64_t)S_BOX[2][x[ 4]] << 24 | (uint64_t)S_BOX[3][x[ 5]] << 16 |
            (uint64_t)S_BOX[0][x[ 6]] <<  8 | (uint64_t)S_BOX[1][x[ 7]]      ,
            (uint64_t)S_BOX[2][x[ 8]] << 56 | (uint64_t)S_BOX[3][x[ 9]] << 48 |
            (uint64_t)S_BOX[0][x[10]] << 40 | (uint64_t)S_BOX[1][x[11]] << 32 |
            (uint64_t)S_BOX[2][x[12]] << 24 | (uint64_t)S_BOX[3][x[13]] << 16 |
            (uint64_t)S_BOX[0][x[14]] <<  8 | (uint64_t)S_BOX[1][x[15]]      ,
        };
    }
    // static ARIAWord a(ARIAWord const &t) {
    //     uint8_t y[16];
    //     y[ 0] = t[ 3] ^ t[ 4] ^ t[ 6] ^ t[ 8] ^ t[ 9] ^ t[13] ^ t[14];
    //     y[ 1] = t[ 2] ^ t[ 5] ^ t[ 7] ^ t[ 8] ^ t[ 9] ^ t[12] ^ t[15];
    //     y[ 2] = t[ 1] ^ t[ 4] ^ t[ 6] ^ t[10] ^ t[11] ^ t[12] ^ t[15];
    //     y[ 3] = t[ 0] ^ t[ 5] ^ t[ 7] ^ t[10] ^ t[11] ^ t[13] ^ t[14];
    //     y[ 4] = t[ 0] ^ t[ 2] ^ t[ 5] ^ t[ 8] ^ t[11] ^ t[14] ^ t[15];
    //     y[ 5] = t[ 1] ^ t[ 3] ^ t[ 4] ^ t[ 9] ^ t[10] ^ t[14] ^ t[15];
    //     y[ 6] = t[ 0] ^ t[ 2] ^ t[ 7] ^ t[ 9] ^ t[10] ^ t[12] ^ t[13];
    //     y[ 7] = t[ 1] ^ t[ 3] ^ t[ 6] ^ t[ 8] ^ t[11] ^ t[12] ^ t[13];
    //     y[ 8] = t[ 0] ^ t[ 1] ^ t[ 4] ^ t[ 7] ^ t[10] ^ t[13] ^ t[15];
    //     y[ 9] = t[ 0] ^ t[ 1] ^ t[ 5] ^ t[ 6] ^ t[11] ^ t[12] ^ t[14];
    //     y[10] = t[ 2] ^ t[ 3] ^ t[ 5] ^ t[ 6] ^ t[ 8] ^ t[13] ^ t[15];
    //     y[11] = t[ 2] ^ t[ 3] ^ t[ 4] ^ t[ 7] ^ t[ 9] ^ t[12] ^ t[14];
    //     y[12] = t[ 1] ^ t[ 2] ^ t[ 6] ^ t[ 7] ^ t[ 9] ^ t[11] ^ t[12];
    //     y[13] = t[ 0] ^ t[ 3] ^ t[ 6] ^ t[ 7] ^ t[ 8] ^ t[10] ^ t[13];
    //     y[14] = t[ 0] ^ t[ 3] ^ t[ 4] ^ t[ 5] ^ t[ 9] ^ t[11] ^ t[14];
    //     y[15] = t[ 1] ^ t[ 2] ^ t[ 4] ^ t[ 5] ^ t[ 8] ^ t[10] ^ t[15];
    //     return {GET_BE<uint64_t>(y), GET_BE<uint64_t>(y + 8)};
    // }
    static constexpr auto A_LUT = []() {
        std::array<std::array<ARIAWord, 256>, 16> A_LUT = {};
        for (int j = 0; j < 256; j++) {
            A_LUT[ 0][j] = {0x0000000101000100U * j, 0x0101000000010100U * j};
            A_LUT[ 1][j] = {0x0000010000010001U * j, 0x0101000001000001U * j};
            A_LUT[ 2][j] = {0x0001000001000100U * j, 0x0000010101000001U * j};
            A_LUT[ 3][j] = {0x0100000000010001U * j, 0x0000010100010100U * j};
            A_LUT[ 4][j] = {0x0100010000010000U * j, 0x0100000100000101U * j};
            A_LUT[ 5][j] = {0x0001000101000000U * j, 0x0001010000000101U * j};
            A_LUT[ 6][j] = {0x0100010000000001U * j, 0x0001010001010000U * j};
            A_LUT[ 7][j] = {0x0001000100000100U * j, 0x0100000101010000U * j};
            A_LUT[ 8][j] = {0x0101000001000001U * j, 0x0000010000010001U * j};
            A_LUT[ 9][j] = {0x0101000000010100U * j, 0x0000000101000100U * j};
            A_LUT[10][j] = {0x0000010100010100U * j, 0x0100000000010001U * j};
            A_LUT[11][j] = {0x0000010101000001U * j, 0x0001000001000100U * j};
            A_LUT[12][j] = {0x0001010000000101U * j, 0x0001000101000000U * j};
            A_LUT[13][j] = {0x0100000100000101U * j, 0x0100010000010000U * j};
            A_LUT[14][j] = {0x0100000101010000U * j, 0x0001000100000100U * j};
            A_LUT[15][j] = {0x0001010001010000U * j, 0x0100010000000001U * j};
        }
        return A_LUT;
    }();
    static constexpr auto O_LUT = []() {
        std::array<std::array<ARIAWord, 256>, 16> O_LUT = {};
        for (int j = 0; j < 256; j++) {
            uint8_t a = S_BOX[0][j];
            uint8_t b = S_BOX[1][j];
            uint8_t c = S_BOX[2][j];
            uint8_t d = S_BOX[3][j];
            O_LUT[ 0][j] = A_LUT[ 0][a]; O_LUT[ 1][j] = A_LUT[ 1][b]; O_LUT[ 2][j] = A_LUT[ 2][c]; O_LUT[ 3][j] = A_LUT[ 3][d];
            O_LUT[ 4][j] = A_LUT[ 4][a]; O_LUT[ 5][j] = A_LUT[ 5][b]; O_LUT[ 6][j] = A_LUT[ 6][c]; O_LUT[ 7][j] = A_LUT[ 7][d];
            O_LUT[ 8][j] = A_LUT[ 8][a]; O_LUT[ 9][j] = A_LUT[ 9][b]; O_LUT[10][j] = A_LUT[10][c]; O_LUT[11][j] = A_LUT[11][d];
            O_LUT[12][j] = A_LUT[12][a]; O_LUT[13][j] = A_LUT[13][b]; O_LUT[14][j] = A_LUT[14][c]; O_LUT[15][j] = A_LUT[15][d];
        }
        return O_LUT;
    }();
    static constexpr auto E_LUT = []() {
        std::array<std::array<ARIAWord, 256>, 16> E_LUT = {};
        for (int j = 0; j < 256; j++) {
            uint8_t a = S_BOX[2][j];
            uint8_t b = S_BOX[3][j];
            uint8_t c = S_BOX[0][j];
            uint8_t d = S_BOX[1][j];
            E_LUT[ 0][j] = A_LUT[ 0][a]; E_LUT[ 1][j] = A_LUT[ 1][b]; E_LUT[ 2][j] = A_LUT[ 2][c]; E_LUT[ 3][j] = A_LUT[ 3][d];
            E_LUT[ 4][j] = A_LUT[ 4][a]; E_LUT[ 5][j] = A_LUT[ 5][b]; E_LUT[ 6][j] = A_LUT[ 6][c]; E_LUT[ 7][j] = A_LUT[ 7][d];
            E_LUT[ 8][j] = A_LUT[ 8][a]; E_LUT[ 9][j] = A_LUT[ 9][b]; E_LUT[10][j] = A_LUT[10][c]; E_LUT[11][j] = A_LUT[11][d];
            E_LUT[12][j] = A_LUT[12][a]; E_LUT[13][j] = A_LUT[13][b]; E_LUT[14][j] = A_LUT[14][c]; E_LUT[15][j] = A_LUT[15][d];
        }
        return E_LUT;
    }();
    static ARIAWord a(ARIAWord const &x) {
        return
            A_LUT[ 0][x[ 0]] ^ A_LUT[ 1][x[ 1]] ^ A_LUT[ 2][x[ 2]] ^ A_LUT[ 3][x[ 3]] ^
            A_LUT[ 4][x[ 4]] ^ A_LUT[ 5][x[ 5]] ^ A_LUT[ 6][x[ 6]] ^ A_LUT[ 7][x[ 7]] ^
            A_LUT[ 8][x[ 8]] ^ A_LUT[ 9][x[ 9]] ^ A_LUT[10][x[10]] ^ A_LUT[11][x[11]] ^
            A_LUT[12][x[12]] ^ A_LUT[13][x[13]] ^ A_LUT[14][x[14]] ^ A_LUT[15][x[15]];
    }
    static ARIAWord fo(ARIAWord const &x) {
        return
            O_LUT[ 0][x[ 0]] ^ O_LUT[ 1][x[ 1]] ^ O_LUT[ 2][x[ 2]] ^ O_LUT[ 3][x[ 3]] ^
            O_LUT[ 4][x[ 4]] ^ O_LUT[ 5][x[ 5]] ^ O_LUT[ 6][x[ 6]] ^ O_LUT[ 7][x[ 7]] ^
            O_LUT[ 8][x[ 8]] ^ O_LUT[ 9][x[ 9]] ^ O_LUT[10][x[10]] ^ O_LUT[11][x[11]] ^
            O_LUT[12][x[12]] ^ O_LUT[13][x[13]] ^ O_LUT[14][x[14]] ^ O_LUT[15][x[15]];
    }
    static ARIAWord fe(ARIAWord const &x) {
        return
            E_LUT[ 0][x[ 0]] ^ E_LUT[ 1][x[ 1]] ^ E_LUT[ 2][x[ 2]] ^ E_LUT[ 3][x[ 3]] ^
            E_LUT[ 4][x[ 4]] ^ E_LUT[ 5][x[ 5]] ^ E_LUT[ 6][x[ 6]] ^ E_LUT[ 7][x[ 7]] ^
            E_LUT[ 8][x[ 8]] ^ E_LUT[ 9][x[ 9]] ^ E_LUT[10][x[10]] ^ E_LUT[11][x[11]] ^
            E_LUT[12][x[12]] ^ E_LUT[13][x[13]] ^ E_LUT[14][x[14]] ^ E_LUT[15][x[15]];
    }
};
template <int L, int R = 8 + 2 * L>
    requires (L == 2 || L == 3 || L == 4)
class ARIATmpl: public ARIABase {
    ARIAWord ek[R + 1];
    ARIAWord dk[R + 1];
public:
    static constexpr size_t BLOCK_SIZE = 16;
    static constexpr size_t KEY_SIZE = 8 * L;
    ARIATmpl(uint8_t const *mk) {
        ARIAWord kl = {L > 0 ? GET_BE<uint64_t>(mk +  0) : 0, L > 1 ? GET_BE<uint64_t>(mk +  8) : 0};
        ARIAWord kr = {L > 2 ? GET_BE<uint64_t>(mk + 16) : 0, L > 3 ? GET_BE<uint64_t>(mk + 24) : 0};
        ARIAWord w0 =                           kl;
        ARIAWord w1 = fo(w0 ^ C[(L + 1) % 3]) ^ kr;
        ARIAWord w2 = fe(w1 ^ C[(L + 2) % 3]) ^ w0;
        ARIAWord w3 = fo(w2 ^ C[(L + 0) % 3]) ^ w1;
        for (;;) {
            ek[ 0] = w0 ^ w1.rotr(19);
            ek[ 1] = w1 ^ w2.rotr(19);
            ek[ 2] = w2 ^ w3.rotr(19);
            ek[ 3] = w3 ^ w0.rotr(19);
            ek[ 4] = w0 ^ w1.rotr(31);
            ek[ 5] = w1 ^ w2.rotr(31);
            ek[ 6] = w2 ^ w3.rotr(31);
            ek[ 7] = w3 ^ w0.rotr(31);
            ek[ 8] = w0 ^ w1.rotl(61);
            ek[ 9] = w1 ^ w2.rotl(61);
            ek[10] = w2 ^ w3.rotl(61);
            ek[11] = w3 ^ w0.rotl(61);
            ek[12] = w0 ^ w1.rotl(31);
            if constexpr (L == 2) { break; }
            ek[13] = w1 ^ w2.rotl(31);
            ek[14] = w2 ^ w3.rotl(31);
            if constexpr (L == 3) { break; }
            ek[15] = w3 ^ w0.rotl(31);
            ek[16] = w0 ^ w1.rotl(19);
            if constexpr (L == 4) { break; }
        }
        dk[0] = ek[R];
        dk[R] = ek[0];
        for (int i = 1; i < R; i++) {
            dk[i] = a(ek[R - i]);
        }
    }
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        ARIAWord x = ARIAWord{GET_BE<uint64_t>(src), GET_BE<uint64_t>(src + 8)} ^ ek[0];
        for (int r = 1;;) {
            x = fo(x) ^ ek[r++];
            if (r == R) { break; }
            x = fe(x) ^ ek[r++];
        }
        x = sl2(x) ^ ek[R];
        PUT_BE<uint64_t>(dst    , x.hi);
        PUT_BE<uint64_t>(dst + 8, x.lo);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        ARIAWord x = ARIAWord{GET_BE<uint64_t>(src), GET_BE<uint64_t>(src + 8)} ^ dk[0];
        for (int r = 1;;) {
            x = fo(x) ^ dk[r++];
            if (r == R) { break; }
            x = fe(x) ^ dk[r++];
        }
        x = sl2(x) ^ dk[R];
        PUT_BE<uint64_t>(dst    , x.hi);
        PUT_BE<uint64_t>(dst + 8, x.lo);
    }
};
using ARIA128 = ARIATmpl<2>;
using ARIA192 = ARIATmpl<3>;
using ARIA256 = ARIATmpl<4>;