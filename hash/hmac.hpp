#pragma once
#include "hash.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define NAP Hash::NOT_ALWAYS_PADDING
template <class Hash>
    requires (DIG <= BLK)
class HMAC {
    Hash inner;
    Hash outer;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
    static constexpr bool NOT_ALWAYS_PADDING = NAP;
    HMAC(uint8_t const *key, size_t len) {
        uint8_t buf[BLK] = {};
        if (len > BLK) {
            HashWrapper<Hash> tmp;
            tmp.update(key, key + len);
            tmp.digest(buf);
        } else {
            memcpy(buf, key, len);
        }
        for (size_t i = 0; i < BLK; i++) {
            buf[i] ^= 0x36;
        }
        inner.input(buf);
        for (size_t i = 0; i < BLK; i++) {
            buf[i] ^= 0x36 ^ 0x5c;
        }
        outer.input(buf);
    }
    void input(uint8_t const *blk) {
        inner.input(blk);
    }
    void final(uint8_t const *src, size_t len, uint8_t *dst) {
        uint8_t buf[DIG];
        inner.final(src, len, buf);
        if constexpr (DIG == BLK && not NAP) {
            outer.input(buf);
            outer.final(NULL,  0, dst);
        } else {
            outer.final(buf, DIG, dst);
        }
    }
};
template <class Hash>
using HMACWrapper = HashWrapper<HMAC<Hash>>;
#undef BLK
#undef DIG
#undef NAP
