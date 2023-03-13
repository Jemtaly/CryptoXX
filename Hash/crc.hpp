#pragma once
#include "hash.hpp"
template <size_t sz8>
class CRC: public HashInterface<1, sz8> {
	uint8_t box[256][sz8];
	uint8_t sta[sz8];
	uint8_t xrv[sz8];
public:
	CRC(uint8_t const *exp, uint8_t const *iv, uint8_t const *xv):
		box{} {
		memcpy(xrv, xv, sz8);
		memcpy(sta, iv, sz8);
		for (int itr = 0; itr < 256; itr++) {
			uint8_t *ref = &box[itr];
			ref[0] = itr;
			for (int i = 0; i < 8; i++) {
				uint8_t per = 0;
				for (size_t j = sz8 - 1; j < sz8; j--) {
					uint8_t tmp = ref[j] & 1;
					ref[j] = ref[j] >> 1 | per << 7;
					per = tmp;
				}
				if (per) {
					for (size_t j = 0; j < sz8; j++) {
						ref[j] ^= exp[j];
					}
				}
			}
		}
	}
	void push(uint8_t const *src) {
		uint8_t const *ref = &box[sta[0] ^ src[0]];
		uint8_t const *st1 = &sta[1];
		for (size_t i = 0; i < sz8 - 1; i++) {
			sta[i] = ref[i] ^ st1[i];
		}
		sta[sz8 - 1] = ref[sz8 - 1];
	}
	void test(uint8_t const *src, size_t len, uint8_t *dst) const {
		for (size_t i = 0; i < sz8; i++) {
			dst[i] = xrv[i] ^ sta[i];
		}
	}
};
