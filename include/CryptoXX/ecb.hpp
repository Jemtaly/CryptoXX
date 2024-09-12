#pragma once

#include "CryptoXX/utils.hpp"

#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE

template<class BlockCipher>
class ECBEnc {
    BlockCipher const bc;

public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = 0;

    template<class... vals_t>
    ECBEnc(uint8_t const *civ, vals_t &&...vals)
        : bc(std::forward<vals_t>(vals)...) {}

    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.encrypt(src, dst);
    }
};

template<class BlockCipher>
class ECBDec {
    BlockCipher const bc;

public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = 0;

    template<class... vals_t>
    ECBDec(uint8_t const *civ, vals_t &&...vals)
        : bc(std::forward<vals_t>(vals)...) {}

    void crypt(uint8_t const *src, uint8_t *dst) {
        bc.decrypt(src, dst);
    }
};

#undef BLK
#undef KEY

#include "CryptoXX/block.hpp"

template<class BlockCipher>
using ECBEncrypter = BlockCipherEncrypter<ECBEnc<BlockCipher>>;
template<class BlockCipher>
using ECBDecrypter = BlockCipherDecrypter<ECBDec<BlockCipher>>;
