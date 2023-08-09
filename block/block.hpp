#pragma once
#include "../utils.hpp"
#define BLK BlockCipherMode::BLOCK_SIZE
#define KEY BlockCipherMode::KEY_SIZE
#define CIV BlockCipherMode::CIV_SIZE
template <class BlockCipherMode>
class BlockCipherEncrypter {
    BlockCipherMode bce;
    size_t use;
    uint8_t buf[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CIV;
    template <class... vals_t>
    BlockCipherEncrypter(vals_t &&...vals):
        bce(std::forward<vals_t>(vals)...), use(0) {}
    // Function returns the pointer to the next byte to be written.
    // Output buffer must be at least (end - src) + BLOCK_SIZE bytes.
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        if (BLK + src <= end + use) {
            memcpy(buf + use, src, BLK - use);
            bce.crypt(buf, dst);
            src += BLK - use;
            dst += BLK;
            use -= use;
            for (; BLK + src <= end; src += BLK, dst += BLK) {
                bce.crypt(src, dst);
            }
        }
        memcpy(buf + use, src, end - src);
        use += end - src;
        src += end - src;
        return dst;
    }
    uint8_t *fflush(uint8_t *dst) {
        memset(buf + use, BLK - use, BLK - use);
        bce.crypt(buf, dst);
        use -= use;
        dst += BLK;
        return dst;
    }
};
template <class BlockCipherMode>
class BlockCipherDecrypter {
    BlockCipherMode bcd;
    size_t use;
    uint8_t buf[BLK];
public:
    static constexpr size_t BLOCK_SIZE = BLK;
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CIV;
    template <class... vals_t>
    BlockCipherDecrypter(vals_t &&...vals):
        bcd(std::forward<vals_t>(vals)...), use(0) {}
    // Function returns the pointer to the next byte to be written.
    // Output buffer must be at least (end - src) + BLOCK_SIZE bytes.
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        if (BLK + src <  end + use) {
            memcpy(buf + use, src, BLK - use);
            bcd.crypt(buf, dst);
            src += BLK - use;
            dst += BLK;
            use -= use;
            for (; BLK + src <  end; src += BLK, dst += BLK) {
                bcd.crypt(src, dst);
            }
        }
        memcpy(buf + use, src, end - src);
        use += end - src;
        src += end - src;
        return dst;
    }
    uint8_t *fflush(uint8_t *dst) {
        if (use != BLK) { return nullptr; }
        bcd.crypt(buf, dst);
        use -= use;
        dst += BLK - dst[BLK - 1];
        return dst;
    }
};
#undef BLK
#undef KEY
#undef CIV
