#pragma once
#include "des.hpp"
class TDES: public BlockCipher<8> {
	DES D, E, S;
public:
	TDES(uint8_t const *const &k, uint8_t const *const &e, uint8_t const *const &y):
		D(k), E(e), S(y) {}
	void encrypt(uint8_t const *const &src, uint8_t *const &dst) const {
		D.encrypt(src, dst);
		E.decrypt(dst, dst);
		S.encrypt(dst, dst);
	}
	void decrypt(uint8_t const *const &src, uint8_t *const &dst) const {
		S.decrypt(src, dst);
		E.encrypt(dst, dst);
		D.decrypt(dst, dst);
	}
};
