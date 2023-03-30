#pragma once
#include "stream.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class CFBMode {
    BlockCipher bc;
public:
    static constexpr size_t SECTION_SIZE = BLK;
    template <class... vals_t>
    CFBMode(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void generate(uint8_t const *src, uint8_t *dst) {
        bc.encrypt(src, dst);
    }
};
template <class BlockCipher>
using CFBModeEncrypter = StreamCipherEncrypter<CFBMode<BlockCipher>>;
template <class BlockCipher>
using CFBModeDecrypter = StreamCipherDecrypter<CFBMode<BlockCipher>>;
#undef BLK
