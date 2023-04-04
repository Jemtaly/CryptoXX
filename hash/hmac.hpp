#pragma once
#include "hash.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define NPD Hash::NO_PADDING
template <class Hash>
requires (DIG < BLK || NPD && DIG == BLK)
class HMAC {
    Hash inner;
    Hash outer;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
    static constexpr bool NO_PADDING = NPD;
    HMAC(uint8_t const *key, size_t len) {
        uint8_t buf[BLK] = {};
        if (len > BLK) {
            HashWrapper<Hash> tmp;
            tmp.update(key, key + len);
            tmp.digest(buf);
        } else {
            memcpy(buf, key, len);
        }
        uint8_t ibf[BLK], obf[BLK];
        for (int j = 0; j < BLK; j++) {
            ibf[j] = buf[j] ^ 0x36;
            obf[j] = buf[j] ^ 0x5c;
        }
        inner.push(ibf);
        outer.push(obf);
    }
    void push(uint8_t const *blk) {
        inner.push(blk);
    }
    void hash(uint8_t const *src, size_t len, uint8_t *dst) {
        uint8_t buf[DIG];
        inner.hash(src, len, buf);
        outer.hash(buf, DIG, dst);
    }
};
template <class Hash>
using HMACWrapper = HashWrapper<HMAC<Hash>>;
#undef BLK
#undef DIG
#undef NPD
