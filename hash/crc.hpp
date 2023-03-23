#pragma once
#include "hash.hpp"
template <size_t N>
class CRC {
    uint8_t box[256][N];
    uint8_t sta[N];
    uint8_t xrv[N];
public:
    static constexpr size_t BLOCK_SIZE = 1;
    static constexpr size_t NEST_SIZE = N;
    CRC(uint8_t const *exp, uint8_t const *iv, uint8_t const *xv):
        box{} {
        memcpy(xrv, xv, N);
        memcpy(sta, iv, N);
        for (int itr = 0; itr < 256; itr++) {
            uint8_t *ref = &box[itr];
            ref[0] = itr;
            for (int i = 0; i < 8; i++) {
                uint8_t per = 0;
                for (size_t j = N - 1; j < N; j--) {
                    uint8_t tmp = ref[j] & 1;
                    ref[j] = ref[j] >> 1 | per << 7;
                    per = tmp;
                }
                if (per) {
                    for (size_t j = 0; j < N; j++) {
                        ref[j] ^= exp[j];
                    }
                }
            }
        }
    }
    void push(uint8_t const *src) {
        uint8_t const *ref = &box[sta[0] ^ src[0]];
        uint8_t const *st1 = &sta[1];
        for (size_t i = 0; i < N - 1; i++) {
            sta[i] = ref[i] ^ st1[i];
        }
        sta[N - 1] = ref[N - 1];
    }
    void test(uint8_t const *src, size_t len, uint8_t *dst) const {
        for (size_t i = 0; i < N; i++) {
            dst[i] = xrv[i] ^ sta[i];
        }
    }
};
