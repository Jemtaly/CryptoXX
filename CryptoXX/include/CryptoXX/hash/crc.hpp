#pragma once

#include "CryptoXX/utils.hpp"

#define DIG sizeof(digest_t)

template<std::unsigned_integral digest_t, digest_t EXP, digest_t CIV, digest_t CXV>
class CRC {
    static constexpr auto LUT = []() {
        std::array<digest_t, 256> LUT = {};
        for (int i = 0; i < 256; i++) {
            digest_t tmp = i;
            for (int j = 0; j < 8; j++) {
                tmp = tmp >> 1 ^ (tmp & 0x1 ? EXP : 0x0);
            }
            LUT[i] = tmp;
        }
        return LUT;
    }();

    digest_t sta = CIV;

public:
    static constexpr size_t BLOCK_SIZE = 1;
    static constexpr size_t DIGEST_SIZE = DIG;
    static constexpr bool LAZY = false;

    void input(uint8_t const *blk) {
        sta = sta >> 8 ^ LUT[sta & 0xff ^ *blk];
    }

    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        PUT_BE(dst, sta ^ CXV);
    }
};

using CRC32 = CRC<uint32_t, 0xedb88320, 0xffffffff, 0xffffffff>;
using CRC64 = CRC<uint64_t, 0xc96c5795d7870f42, 0xffffffffffffffff, 0xffffffffffffffff>;

#undef DIG
