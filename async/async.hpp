#pragma once
#include "../utils.hpp"
#define CFB AsyncCipher::FEEDBACK_SIZE
#define SEC AsyncCipher::SECTION_SIZE
template <class AsyncCipher>
class AsyncCipherEncrypter {
    AsyncCipher sc;
    size_t use;
    uint8_t cfb[CFB];
    uint8_t buf[SEC];
public:
    template <class... vals_t>
    AsyncCipherEncrypter(uint8_t const *civ, vals_t &&...vals):
        sc(std::forward<vals_t>(vals)...), use(SEC) {
        memcpy(cfb, civ, CFB);
    }
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            for (; use < SEC; ++use, ++src, ++dst) {
                buf[use] = *dst = *src ^ buf[use];
            }
            for (size_t i = 0; i < CFB; ++i) {
                cfb[i] = cfb[i + SEC];
            }
            sc.crypt(cfb, buf);
            use -= SEC;
        }
        for (; src < end; ++use, ++src, ++dst) {
            buf[use] = *dst = *src ^ buf[use];
        }
        return dst;
    }
};
template <class AsyncCipher>
class AsyncCipherDecrypter {
    AsyncCipher sc;
    size_t use;
    uint8_t cfb[CFB];
    uint8_t buf[SEC];
public:
    template <class... vals_t>
    AsyncCipherDecrypter(uint8_t const *civ, vals_t &&...vals):
        sc(std::forward<vals_t>(vals)...), use(SEC) {
        memcpy(cfb, civ, CFB);
    }
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            for (; use < SEC; ++use, ++src, ++dst) {
                buf[use] ^= *dst = *src ^ buf[use];
            }
            for (size_t i = 0; i < CFB; ++i) {
                cfb[i] = cfb[i + SEC];
            }
            sc.crypt(cfb, buf);
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
