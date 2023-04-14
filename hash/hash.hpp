#pragma once
#include "../bits.hpp"
#include <utility> // std::forward
#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE
#define NPD Hash::NO_PADDING
template <class Hash, bool = NPD>
class HashWrapper;
template <class Hash>
class HashWrapper<Hash, false> {
    Hash hash;
    size_t use;
    uint8_t mem[BLK];
public:
    template <class... vals_t>
    HashWrapper(vals_t &&...vals):
        hash(std::forward<vals_t>(vals)...), use(0) {}
    void update(uint8_t const *src, uint8_t const *end) {
        if (BLK + src <= use + end) {
            memcpy(mem + use, src, BLK - use);
            hash.push(mem);
            src += BLK - use;
            use -= use;
            for (; src + BLK <= end; src += BLK) {
                hash.push(src);
            }
        }
        memcpy(mem + use, src, end - src);
        use += end - src;
        src += end - src;
    }
    void digest(uint8_t *dst) const {
        Hash copy = hash;
        copy.hash(mem, use, dst);
    }
};
template <class Hash>
class HashWrapper<Hash, true> {
    Hash hash;
    size_t use;
    uint8_t mem[BLK];
public:
    template <class... vals_t>
    HashWrapper(vals_t &&...vals):
        hash(std::forward<vals_t>(vals)...), use(0) {}
    void update(uint8_t const *src, uint8_t const *end) {
        if (BLK + src <  use + end) {
            memcpy(mem + use, src, BLK - use);
            hash.push(mem);
            src += BLK - use;
            use -= use;
            for (; src + BLK <  end; src += BLK) {
                hash.push(src);
            }
        }
        memcpy(mem + use, src, end - src);
        use += end - src;
        src += end - src;
    }
    void digest(uint8_t *dst) const {
        Hash copy = hash;
        copy.hash(mem, use, dst);
    }
};
#undef BLK
#undef DIG
#undef NPD
