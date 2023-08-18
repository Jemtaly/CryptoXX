#pragma once
#include "../utils.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE
template <class BlockCipher>
class PCBCEnc {
    BlockCipher const bc;
    uint8_t rec[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    PCBCEnc(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(rec, civ, BLK);
    }
    void crypt(uint8_t const *src, uint8_t *dst) {
        for (size_t i = 0; i < BLK; i++) {
            dst[i] = src[i] ^ rec[i];
        }
        bc.encrypt(dst, dst);
        for (size_t i = 0; i < BLK; i++) {
            rec[i] = dst[i] ^ src[i];
        }
    }
};
template <class BlockCipher>
class PCBCDec {
    BlockCipher const bc;
    uint8_t rec[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    PCBCDec(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(rec, civ, BLK);
    }
    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.decrypt(src, dst);
        for (size_t i = 0; i < BLK; i++) {
            dst[i] = dst[i] ^ rec[i];
        }
        for (size_t i = 0; i < BLK; i++) {
            rec[i] = dst[i] ^ src[i];
        }
    }
};
#undef BLK
#undef KEY
#include "block.hpp"
template <class BlockCipher>
using PCBCEncrypter = BlockCipherEncrypter<PCBCEnc<BlockCipher>>;
template <class BlockCipher>
using PCBCDecrypter = BlockCipherDecrypter<PCBCDec<BlockCipher>>;
