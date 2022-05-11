#pragma once
#include <stdint.h>
#include "des.hpp"
class TDES : public Crypto<8> {
	DES des_a, des_b, des_c;
public:
	TDES(uint8_t const *const &k) : des_a(k), des_b(k + 010), des_c(k + 020) {}
	void encrypt(uint8_t const *const &p, uint8_t *const &c) const {
		des_a.encrypt(p, c);
		des_b.decrypt(c, c);
		des_c.encrypt(c, c);
	}
	void decrypt(uint8_t const *const &c, uint8_t *const &p) const {
		des_c.decrypt(c, p);
		des_b.encrypt(p, p);
		des_a.decrypt(p, p);
	}
};
