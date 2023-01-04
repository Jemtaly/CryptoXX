#pragma once
#include "stream.hpp"
class RC4: public StreamCipher {
	uint8_t x, y, m[256];
public:
	RC4(uint8_t const &len, uint8_t *const &key):
		x(0), y(0) {
		for (int i = 0; i < 256; i++) {
			m[i] = i;
		}
		uint8_t j = 0, temp;
		for (int i = 0; i < 256; i++) {
			temp = m[i];
			j += temp + key[i % len];
			m[i] = m[j];
			m[j] = temp;
		}
	}
	void crypt(uint8_t const *const &src, uint8_t *const &dst) {
		uint8_t a = m[x += 1];
		uint8_t b = m[y += a];
		m[x] = b;
		m[y] = a;
		*dst = *src ^ m[a + b & 0xff];
	}
};
