#pragma once
#include "hash.hpp"
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
template <class Hash>
requires (DIG < BLK)
class HMAC {
    Hash inner;
    Hash outer;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t DIGEST_SIZE = DIG;
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
