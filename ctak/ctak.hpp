#pragma once
#include "../utils.hpp"
#define CFB CTAKCipher::FEEDBACK_SIZE
#define SEC CTAKCipher::SECTION_SIZE
#define KEY CTAKCipher::KEY_SIZE
template <class CTAKCipher>
class CTAKCipherEncrypter {
    CTAKCipher const ac;
    size_t use;
    uint8_t cfb[CFB];
    uint8_t buf[SEC];
public:
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CFB;
    template <class... vals_t>
    CTAKCipherEncrypter(uint8_t const *civ, vals_t &&...vals):
        ac(std::forward<vals_t>(vals)...), use(SEC) {
        memcpy(cfb + SEC, civ, CFB);
    }
    // Function returns the pointer to the next byte to be written.
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            for (; use < SEC; ++use, ++src, ++dst) {
                buf[use] = *dst = *src ^ buf[use];
            }
            for (size_t i = 0; i < CFB; ++i) {
                cfb[i] = cfb[i + SEC];
            }
            ac.generate(cfb, buf);
            use -= SEC;
        }
        for (; src < end; ++use, ++src, ++dst) {
            buf[use] = *dst = *src ^ buf[use];
        }
        return dst;
    }
};
template <class CTAKCipher>
class CTAKCipherDecrypter {
    CTAKCipher const ac;
    size_t use;
    uint8_t cfb[CFB];
    uint8_t buf[SEC];
public:
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CFB;
    template <class... vals_t>
    CTAKCipherDecrypter(uint8_t const *civ, vals_t &&...vals):
        ac(std::forward<vals_t>(vals)...), use(SEC) {
        memcpy(cfb + SEC, civ, CFB);
    }
    // Function returns the pointer to the next byte to be written.
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            for (; use < SEC; ++use, ++src, ++dst) {
                buf[use] ^= *dst = *src ^ buf[use];
            }
            for (size_t i = 0; i < CFB; ++i) {
                cfb[i] = cfb[i + SEC];
            }
            ac.generate(cfb, buf);
            use -= SEC;
        }
        for (; src < end; ++use, ++src, ++dst) {
            buf[use] ^= *dst = *src ^ buf[use];
        }
        return dst;
    }
};
#undef CFB
#undef SEC
#undef KEY
