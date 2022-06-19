#pragma once
#include "stream.hpp"
class RC4: public StreamCipher {
	uint8_t x, y, m[256];
public:
	RC4(uint8_t *const &key, size_t const &len):
		x(0), y(0) {
		for (int i = 0; i < 256; i++) {
			m[i] = i;
		}
		uint8_t j = 0;
		for (int i = 0; i < 256; i++) {
			uint8_t t = m[i];
			j += t + key[i % len];
			m[i] = m[j];
			m[j] = t;
		}
	}
	void crypt(uint8_t const *const &src, uint8_t *const &dst) {
		uint8_t a = m[++x];
		uint8_t b = m[x] = m[y += a];
		m[y] = a;
		*dst = *src ^ m[a + b & 0xff];
	}
};
