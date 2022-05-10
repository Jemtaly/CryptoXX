#pragma once
#include <stdint.h>
#include "des.hpp"
class TDES : public Crypto<uint64_t, 1> {
	DES des1, des2, des3;
public:
	TDES(uint64_t const &k1, uint64_t const &k2, uint64_t const &k3) : des1(k1), des2(k2), des3(k3) {}
	void encrypt(uint64_t const *const &p, uint64_t *const &c) const {
		des1.encrypt(p, c);
		des2.decrypt(c, c);
		des3.encrypt(c, c);
	}
	void decrypt(uint64_t const *const &c, uint64_t *const &p) const {
		des3.decrypt(c, p);
		des2.encrypt(p, p);
		des1.decrypt(p, p);
	}
};
