#pragma once
#include "des.hpp"
class TDES {
	DES D, E, S;
public:
	static constexpr size_t BLOCK_SIZE = DES::BLOCK_SIZE;
	TDES(uint8_t const *k, uint8_t const *e, uint8_t const *y):
		D(k), E(e), S(y) {}
	void encrypt(uint8_t const *src, uint8_t *dst) const {
		D.encrypt(src, dst);
		E.decrypt(dst, dst);
		S.encrypt(dst, dst);
	}
	void decrypt(uint8_t const *src, uint8_t *dst) const {
		S.decrypt(src, dst);
		E.encrypt(dst, dst);
		D.decrypt(dst, dst);
	}
};
