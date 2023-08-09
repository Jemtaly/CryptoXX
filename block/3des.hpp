#pragma once
#include "des.hpp"
class TDES {
    DES des[3];
public:
    static constexpr size_t BLOCK_SIZE = DES::BLOCK_SIZE;
    static constexpr size_t KEY_SIZE = DES::KEY_SIZE * 3;
    TDES(uint8_t const *mk):
        des{
            mk + DES::KEY_SIZE * 0,
            mk + DES::KEY_SIZE * 1,
            mk + DES::KEY_SIZE * 2,
        } {}
    void encrypt(uint8_t const *src, uint8_t *dst) const {
        des[0].encrypt(src, dst);
        des[1].decrypt(dst, dst);
        des[2].encrypt(dst, dst);
    }
    void decrypt(uint8_t const *src, uint8_t *dst) const {
        des[2].decrypt(src, dst);
        des[1].encrypt(dst, dst);
        des[0].decrypt(dst, dst);
    }
};
