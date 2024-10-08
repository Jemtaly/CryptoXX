#pragma once

#include "CryptoXX/utils.hpp"

#define BLK Hash::BLOCK_SIZE
#define DIG Hash::DIGEST_SIZE

template<class Hash, bool = Hash::LAZY>
class HashWrapper;

template<class Hash>
class HashWrapper<Hash, false> {
    Hash hash;
    size_t use;  // The number of bytes of the current block already stored in mem. 0 <= use < BLK
    uint8_t mem[BLK];

public:
    static constexpr size_t DIGEST_SIZE = DIG;

    template<class... vals_t>
    HashWrapper(vals_t &&...vals)
        : hash(std::forward<vals_t>(vals)...), use(0) {}

    void update(uint8_t const *src, uint8_t const *end) {
        if (BLK + src <= use + end) {
            memcpy(mem + use, src, BLK - use);
            hash.input(mem);
            src += BLK - use;
            use -= use;
            for (; src + BLK <= end; src += BLK) {
                hash.input(src);
            }
        }
        memcpy(mem + use, src, end - src);
        use += end - src;
        src += end - src;
    }

    // The object should not be used after calling this function.
    void digest(uint8_t *dst) {
        hash.final(mem, use, dst);
    }
};

template<class Hash>
class HashWrapper<Hash, true> {
    Hash hash;
    size_t use;  // The number of bytes of the current block already stored in mem. 0 < use <= BLK (except at the beginning of the message)
    uint8_t mem[BLK];

public:
    static constexpr size_t DIGEST_SIZE = DIG;

    template<class... vals_t>
    HashWrapper(vals_t &&...vals)
        : hash(std::forward<vals_t>(vals)...), use(0) {}

    void update(uint8_t const *src, uint8_t const *end) {
        if (BLK + src < use + end) {
            memcpy(mem + use, src, BLK - use);
            hash.input(mem);
            src += BLK - use;
            use -= use;
            for (; src + BLK < end; src += BLK) {
                hash.input(src);
            }
        }
        memcpy(mem + use, src, end - src);
        use += end - src;
        src += end - src;
    }

    // The object should not be used after calling this function.
    void digest(uint8_t *dst) {
        hash.final(mem, use, dst);
    }
};

#undef BLK
#undef DIG
