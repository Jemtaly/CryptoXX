#pragma once
#include "hash.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define NPD Hash::NO_PADDING
template <class Hash>
    requires (DIG <= BLK)
class HMAC {
    Hash inner;
    Hash outer;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
    static constexpr size_t KEY_SIZE = BLK; // MIN: 0
    static constexpr bool NO_PADDING = NPD;
    HMAC(uint8_t const *key, size_t len = KEY_SIZE) {
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
        inner.push(buf);
        for (size_t i = 0; i < BLK; i++) {
            buf[i] ^= 0x36 ^ 0x5c;
        }
        outer.push(buf);
    }
    void push(uint8_t const *blk) {
        inner.push(blk);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) {
        uint8_t buf[DIG];
        inner.hash(src, len, buf);
        if constexpr (DIG == BLK && not NPD) {
            outer.push(buf);
            outer.hash(NULL,  0, dst);
        } else {
            outer.hash(buf, DIG, dst);
        }
    }
};
template <class Hash>
using HMACWrapper = HashWrapper<HMAC<Hash>>;
#undef BLK
#undef DIG
#undef NPD
