#pragma once
#include "stream.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class OFBMode {
    BlockCipher bc;
    uint8_t ofb[BLK];
public:
    static constexpr size_t SECTION_SIZE = BLK;
    template <class... vals_t>
    OFBMode(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(ofb, civ, BLK);
    }
    void generate(uint8_t *dst) {
        bc.encrypt(ofb, dst);
        memcpy(ofb, dst, BLK);
    }
};
template <class BlockCipher>
using OFBModeCrypter = StreamCipherCrypter<OFBMode<BlockCipher>>;
#undef BLK
