#pragma once
#include "../utils.hpp"
#define SEC StreamCipher::SECTION_SIZE
#define KEY StreamCipher::KEY_SIZE
#define CIV StreamCipher::CIV_SIZE
template <class StreamCipher>
class PseudoRandomGenerator {
    StreamCipher sc;
    size_t use; // The number of bytes already output in buf. 0 < use <= SEC
    uint8_t buf[SEC];
public:
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CIV;
    template <class... vals_t>
    PseudoRandomGenerator(vals_t &&...vals):
        sc(std::forward<vals_t>(vals)...), use(SEC) {}
    void generate(uint8_t *dst, uint8_t *end) {
        if (SEC + dst < end + use) {
            memcpy(dst, buf + use, SEC - use);
            dst += SEC - use;
            use -= use;
            for (; SEC + dst < end; dst += SEC) {
                sc.generate(dst);
            }
        }
        sc.generate(buf);
        memcpy(dst, buf + use, end - dst);
        use += end - dst;
        dst += end - dst;
    }
};
template <class StreamCipher>
class StreamCipherCrypter {
    StreamCipher sc;
    size_t use; // The number of bytes already used to XOR with the plaintext in buf. 0 < use <= SEC
    uint8_t buf[SEC];
public:
    static constexpr size_t KEY_SIZE = KEY;
    static constexpr size_t CIV_SIZE = CIV;
    template <class... vals_t>
    StreamCipherCrypter(vals_t &&...vals):
        sc(std::forward<vals_t>(vals)...), use(SEC) {}
    // Function returns the pointer to the next byte to be written.
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            for (; use < SEC; ++use, ++src, ++dst) {
                *dst = *src ^ buf[use];
            }
            sc.generate(buf);
            use -= SEC;
        }
        for (; src < end; ++use, ++src, ++dst) {
            *dst = *src ^ buf[use];
        }
        return dst;
    }
};
template <class StreamCipher>
using StreamCipherEncrypter = StreamCipherCrypter<StreamCipher>;
template <class StreamCipher>
using StreamCipherDecrypter = StreamCipherCrypter<StreamCipher>;
#undef SEC
#undef KEY
#undef CIV
