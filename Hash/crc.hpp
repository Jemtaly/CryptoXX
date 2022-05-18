#pragma once
#include "hash.hpp"
template <size_t size>
class CRC : public Hash<1, size, size> {
	uint8_t exp[size];
	uint8_t ini[size];
	uint8_t xrv[size];
public:
	CRC(uint8_t const *const &e, uint8_t const *const &i, uint8_t const *const &x) {
		memcpy(exp, e, size);
		memcpy(ini, i, size);
		memcpy(xrv, x, size);
	}
	void init(uint8_t *const &rec) const {
		memcpy(rec, ini, size);
	}
	void load(uint8_t const *const &rci, uint8_t *const &rco, uint8_t const *const &blk) const {
		memcpy(rco, rci, size);
		for (size_t i = 7; i != -1; i--) {
			bool per = blk[0] >> i & 1;
			for (size_t j = size - 1; j != -1; j--) {
				bool tmp = rco[j] >> 7;
				rco[j] = rco[j] << 1 | per;
				per = tmp;
			}
			if (per)
				for (size_t j = 0; j < size; j++)
					rco[j] ^= exp[j];
		}
	}
	void save(uint8_t const *const &rec, uint8_t *const &buf, uint8_t const *const &src, size_t const &len) const {
		memcpy(buf, rec, size);
		for (size_t n = 0; n < size; n++)
			for (size_t i = 7; i != -1; i--) {
				bool per = xrv[n] >> i & 1;
				for (size_t j = size - 1; j != -1; j--) {
					bool tmp = buf[j] >> 7;
					buf[j] = buf[j] << 1 | per;
					per = tmp;
				}
				if (per)
					for (size_t j = 0; j < size; j++)
						buf[j] ^= exp[j];
			}
	}
};
