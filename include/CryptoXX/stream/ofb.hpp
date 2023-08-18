#pragma once
#include "../utils.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE
template <class BlockCipher>
class OFBGen {
    BlockCipher const bc;
    uint8_t ofb[BLK];
public:
    static constexpr size_t SECTION_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = BLK;
    template <class... vals_t>
    OFBGen(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(ofb, civ, BLK);
    }
    void generate(uint8_t *buf) {
        bc.encrypt(ofb, buf);
        memcpy(ofb, buf, BLK);
    }
};
#undef BLK
#undef KEY
#include "stream.hpp"
template <class BlockCipher>
using OFBCrypter = StreamCipherCrypter<OFBGen<BlockCipher>>;
template <class BlockCipher>
using OFBEncrypter = StreamCipherEncrypter<OFBGen<BlockCipher>>;
template <class BlockCipher>
using OFBDecrypter = StreamCipherDecrypter<OFBGen<BlockCipher>>;
template <class BlockCipher>
using OFBGenerator = PseudoRandomGenerator<OFBGen<BlockCipher>>;
