#pragma once
#include "CryptoXX/utils.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE
template <class BlockCipher>
class CTRGen {
    BlockCipher const bc;
    uint8_t ctr[BLK];
public:
    static constexpr size_t SECTION_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    CTRGen(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(ctr, civ, BLK);
    }
    void generate(uint8_t *buf) {
        bc.encrypt(ctr, buf);
        for (size_t i = BLK - 1; i < BLK && ++ctr[i] == 0; i--) {}
    }
};
#undef BLK
#undef KEY
#include "CryptoXX/stream.hpp"
template <class BlockCipher>
using CTRCrypter = StreamCipherCrypter<CTRGen<BlockCipher>>;
template <class BlockCipher>
using CTREncrypter = StreamCipherEncrypter<CTRGen<BlockCipher>>;
template <class BlockCipher>
using CTRDecrypter = StreamCipherDecrypter<CTRGen<BlockCipher>>;
template <class BlockCipher>
using CTRGenerator = PseudoRandomGenerator<CTRGen<BlockCipher>>;
