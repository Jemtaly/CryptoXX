#pragma once
#include "stream.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class OFBMode {
    BlockCipher const bc;
    uint8_t ofb[BLK];
public:
    static constexpr size_t SECTION_SIZE = BLK;
    template <class... vals_t>
    OFBMode(uint8_t const *civ, vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {
        memcpy(ofb, civ, BLK);
    }
    void generate(uint8_t *buf) {
        bc.encrypt(ofb, buf);
        memcpy(ofb, buf, BLK);
    }
};
template <class BlockCipher>
using OFBModeCrypter = StreamCipherCrypter<OFBMode<BlockCipher>>;
#undef BLK
