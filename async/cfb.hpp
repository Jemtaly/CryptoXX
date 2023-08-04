#pragma once
#include "async.hpp"
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class CFBGen {
    BlockCipher const bc;
public:
    static constexpr size_t FEEDBACK_SIZE = BLK;
    static constexpr size_t SECTION_SIZE = BLK;
    template <class... vals_t>
    CFBGen(vals_t &&...vals):
        bc(std::forward<vals_t>(vals)...) {}
    void generate(uint8_t const *cfb, uint8_t *buf) const {
        bc.encrypt(cfb, buf);
    }
};
template <class BlockCipher>
using CFBEncrypter = AsyncCipherEncrypter<CFBGen<BlockCipher>>;
template <class BlockCipher>
using CFBDecrypter = AsyncCipherDecrypter<CFBGen<BlockCipher>>;
#undef BLK
