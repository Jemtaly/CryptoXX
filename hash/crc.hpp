#pragma once
#include <array>
#include "hash.hpp"
template <std::unsigned_integral digest_t, digest_t EXP, digest_t CIV, digest_t CXV>
class CRC {
    static constexpr auto box = []() {
        std::array<digest_t, 256> box;
        for (int i = 0; i < 256; i++) {
            digest_t tmp = i;
            for (int j = 0; j < 8; j++) {
                tmp = tmp >> 1 ^ (tmp & 1 ? EXP : 0);
            }
            box[i] = tmp;
        }
        return box;
    }();
    digest_t sta = CIV;
public:
    static constexpr size_t BLOCK_SIZE = 1;
    static constexpr size_t DIGEST_SIZE = sizeof(digest_t);
    void push(uint8_t const *blk) {
        sta = sta >> 8 ^ box[sta & 0xff ^ blk[0]];
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) const {
        digest_t dig = sta;
        for (int i = 0; i < len; i++) {
            dig = dig >> 8 ^ box[dig & 0xff ^ src[i]];
        }
        dig ^= CXV;
        for (int i = 0; i < sizeof(digest_t); i++) {
            dst[i] = dig >> (sizeof(digest_t) - i - 1) * 8;
        }
    }
};
using CRC32 = CRC<uint32_t, 0xedb88320, 0xffffffff, 0xffffffff>;
using CRC64 = CRC<uint64_t, 0xc96c5795d7870f42, 0xffffffffffffffff, 0xffffffffffffffff>;
