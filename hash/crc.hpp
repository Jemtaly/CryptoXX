#pragma once
#include "hash.hpp"
template <size_t sz8>
class CRC : public Hash<1, sz8> {
	uint8_t ini[sz8];
	uint8_t xrv[sz8];
	uint8_t box[256][sz8];
	uint8_t sta[sz8];
public:
	CRC(uint8_t const *const &exp, uint8_t const *const &i, uint8_t const *const &x) : box{} {
		memcpy(ini, i, sz8);
		memcpy(xrv, x, sz8);
		for (int itr = 0; itr < 256; itr++) {
			uint8_t (&sym)[sz8] = box[itr];
			sym[0] = itr;
			for (int i = 0; i < 8; i++) {
				uint8_t per = 0;
				for (size_t j = sz8 - 1; j != -1; j--) {
					uint8_t tmp = sym[j] & 1;
					sym[j] = sym[j] >> 1 | per << 7;
					per = tmp;
				}
				if (per)
					for (size_t j = 0; j < sz8; j++)
						sym[j] ^= exp[j];
			}
		}
	}
	void push(uint8_t const *const &blk) const {
		uint8_t const(&xxx)[sz8] = box[sta[0] ^ blk[0]];
		for (size_t i = 0; i < sz8 - 1; i++)
			sta[i] = sta[i + 1] ^ xxx[i];
		sta[sz8 - 1] = xxx[sz8 - 1];
	}
	void cast(uint8_t const *const &fin, size_t const &len, uint8_t *const &buf) const {
		memcpy(buf, sta, sz8);
		for (size_t n = 0; n < sz8; n++)
			buf[n] ^= xrv[n];
	}
};