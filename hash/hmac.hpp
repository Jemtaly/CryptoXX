#pragma once
#include "hash.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
template <class Hash>
requires (DIG < BLK)
class HMAC {
    Hash inner;
    Hash outer;
    uint8_t key[BLK] = {};
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
    HMAC(uint8_t const *kin, size_t len) {
        if (len > BLK) {
            HashWrapper<Hash> tmp;
            tmp.update(kin, kin + len);
            tmp.digest(key);
        } else {
            memcpy(key, kin, len);
        }
        uint8_t bfi[BLK], bfo[BLK];
        for (int j = 0; j < BLK; j++) {
            bfi[j] = key[j] ^ 0x36;
            bfo[j] = key[j] ^ 0x5c;
        }
        inner.push(bfi);
        outer.push(bfo);
    }
    void push(uint8_t const *src) {
        inner.push(src);
    }
    void test(uint8_t const *src, size_t len, uint8_t *dst) const {
        uint8_t buf[DIG];
        inner.test(src, len, buf);
        outer.test(buf, DIG, dst);
    }
};
#undef BLK
#undef DIG
