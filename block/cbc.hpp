#pragma once
#include "block.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class CBCMode {
    BlockCipher const bc;
    uint8_t rec[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    template <class... vals_t>
    CBCMode(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(rec, civ, BLK);
    }
    void encrypt(uint8_t const *src, uint8_t *dst) {
        for (size_t i = 0; i < BLK; i++) {
            dst[i] = src[i] ^ rec[i];
        }
        bc.encrypt(dst, dst);
        memcpy(rec, dst, BLK);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) {
        bc.decrypt(src, dst);
        for (size_t i = 0; i < BLK; i++) {
            dst[i] ^= rec[i];
        }
        memcpy(rec, src, BLK);
    }
};
template <class BlockCipher>
using CBCModeEncrypter = BlockCipherEncrypter<CBCMode<BlockCipher>>;
template <class BlockCipher>
using CBCModeDecrypter = BlockCipherDecrypter<CBCMode<BlockCipher>>;
#undef BLK
