#pragma once
#include "block.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class ECBEnc {
    BlockCipher const bc;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    template <class... vals_t>
    ECBEnc(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.encrypt(src, dst);
    }
};
template <class BlockCipher>
class ECBDec {
    BlockCipher const bc;
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    template <class... vals_t>
    ECBDec(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.decrypt(src, dst);
    }
};
template <class BlockCipher>
using ECBEncrypter = BlockCipherEncrypter<ECBEnc<BlockCipher>>;
template <class BlockCipher>
using ECBDecrypter = BlockCipherDecrypter<ECBDec<BlockCipher>>;
#undef BLK
