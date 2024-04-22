#pragma once
#include "CryptoXX/utils.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE
template <class BlockCipher>
class CBCEnc {
    BlockCipher const bc;
    uint8_t rec[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    CBCEnc(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(rec, civ, BLK);
    }
    void crypt(uint8_t const *src, uint8_t *dst) {
        for (size_t i = 0; i < BLK; i++) {
            dst[i] = src[i] ^ rec[i];
        }
        bc.encrypt(dst, dst);
        memcpy(rec, dst, BLK);
    }
};
template <class BlockCipher>
class CBCDec {
    BlockCipher const bc;
    uint8_t rec[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    CBCDec(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(rec, civ, BLK);
    }
    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.decrypt(src, dst);
        for (size_t i = 0; i < BLK; i++) {
            dst[i] = dst[i] ^ rec[i];
        }
        memcpy(rec, src, BLK);
    }
};
#undef BLK
#undef KEY
#include "block.hpp"
template <class BlockCipher>
using CBCEncrypter = BlockCipherEncrypter<CBCEnc<BlockCipher>>;
template <class BlockCipher>
using CBCDecrypter = BlockCipherDecrypter<CBCDec<BlockCipher>>;
