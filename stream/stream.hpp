#pragma once
#include <stdint.h>
#include <string.h>
#include <utility> // std::forward
#define SEC StreamCipher::SECTION_SIZE
template <class StreamCipher>
class StreamCipherCrypter {
    StreamCipher sc;
    size_t use;
    uint8_t buf[SEC];
public:
    template <class... vals_t>
    StreamCipherCrypter(vals_t &&...vals):
        sc(std::forward<vals_t>(vals)...), use(SEC) {}
    uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
        while (SEC + src < end + use) {
            while (use < SEC) {
                *dst++ = *src++ ^ buf[use++];
            }
            sc.generate(buf);
            use -= SEC;
        }
        while (src < end) {
            *dst++ = *src++ ^ buf[use++];
        }
        return dst;
    }
};
#undef SEC
