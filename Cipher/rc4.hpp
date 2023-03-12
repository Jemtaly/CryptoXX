#pragma once
#include "stream.hpp"
class RC4: public StreamCipher<1> {
	uint8_t x, y, m[256];
public:
	RC4(uint8_t len, uint8_t const *key):
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
	void generate(uint8_t *dst) {
		uint8_t a = m[x += 1];
		uint8_t b = m[y += a];
		m[x] = b;
		m[y] = a;
		*dst = m[a + b & 255];
	}
};
