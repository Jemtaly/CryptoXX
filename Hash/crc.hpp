#pragma once
#include "hash.hpp"
template <size_t sz8>
class CRC : public HashFunction<1, sz8, uint8_t[sz8]> {
	uint8_t exp[sz8];
	uint8_t ini[sz8];
	uint8_t xrv[sz8];
public:
	CRC(uint8_t const *const &e, uint8_t const *const &i, uint8_t const *const &x) {
		memcpy(exp, e, sz8);
		memcpy(ini, i, sz8);
		memcpy(xrv, x, sz8);
	}
	void init(sta_t &sta) const {
		memcpy(sta, ini, sz8);
	}
	void push(sta_t &sta, uint8_t const *const &blk) const {
		for (size_t i = 7; i != -1; i--) {
			bool per = blk[0] >> i & 1;
			for (size_t j = sz8 - 1; j != -1; j--) {
				bool tmp = sta[j] >> 7;
				sta[j] = sta[j] << 1 | per;
				per = tmp;
			}
			if (per)
				for (size_t j = 0; j < sz8; j++)
					sta[j] ^= exp[j];
		}
	}
	void cast(sta_t const &sta, uint8_t *const &buf, uint8_t const *const &fin, size_t const &len) const {
		memcpy(buf, sta, sz8);
		for (size_t n = 0; n < sz8; n++)
			for (size_t i = 7; i != -1; i--) {
				bool per = xrv[n] >> i & 1;
				for (size_t j = sz8 - 1; j != -1; j--) {
					bool tmp = buf[j] >> 7;
					buf[j] = buf[j] << 1 | per;
					per = tmp;
				}
				if (per)
					for (size_t j = 0; j < sz8; j++)
						buf[j] ^= exp[j];
			}
	}
};
