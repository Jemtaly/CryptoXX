#pragma once
#include "async.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class CFBMode {
    BlockCipher const bc;
public:
    static constexpr size_t FEEDBACK_SIZE = BLK;
    static constexpr size_t SECTION_SIZE = BLK;
    template <class... vals_t>
    CFBMode(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void crypt(uint8_t const *cfb, uint8_t *buf) const {
        bc.encrypt(cfb, buf);
    }
};
template <class BlockCipher>
using CFBModeEncrypter = AsyncCipherEncrypter<CFBMode<BlockCipher>>;
template <class BlockCipher>
using CFBModeDecrypter = AsyncCipherDecrypter<CFBMode<BlockCipher>>;
#undef BLK
