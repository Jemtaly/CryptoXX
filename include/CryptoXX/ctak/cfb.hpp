#pragma once
#include "../utils.hpp"
#define BLK BlockCipher::BLOCK_SIZE
#define KEY BlockCipher::KEY_SIZE
template <class BlockCipher>
class CFBGen {
    BlockCipher const bc;
public:
    static constexpr size_t FEEDBACK_SIZE = BLK;
    static constexpr size_t SECTION_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    template <class... vals_t>
    CFBGen(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void generate(uint8_t const *cfb, uint8_t *buf) const {
        bc.encrypt(cfb, buf);
    }
};
#undef BLK
#undef KEY
#include "ctak.hpp"
template <class BlockCipher>
using CFBEncrypter = CTAKCipherEncrypter<CFBGen<BlockCipher>>;
template <class BlockCipher>
using CFBDecrypter = CTAKCipherDecrypter<CFBGen<BlockCipher>>;
