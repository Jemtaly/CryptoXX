#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
template <size_t bs>
class Crypto {
public:
	virtual void encrypt(uint8_t const *const &p, uint8_t *const &c) const = 0;
	virtual void decrypt(uint8_t const *const &c, uint8_t *const &p) const = 0;
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) {
		size_t rec;
		uint8_t p[bs], c[bs];
		while ((rec = fread(p, 1, bs, ifp)) == bs) {
			encrypt(p, c);
			fwrite(c, bs, 1, ofp);
		}
		p[bs - 1] = bs - rec;
		encrypt(p, c);
		fwrite(c, bs, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) {
		uint8_t c[bs], p[bs];
		fread(c, bs, 1, ifp);
		decrypt(c, p);
		while (fread(c, bs, 1, ifp)) {
			fwrite(p, bs, 1, ofp);
			decrypt(c, p);
		}
		fwrite(p, 1, bs - p[bs - 1], ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) {
		size_t rec;
		uint8_t ctr[bs], res[bs], data[bs];
		memcpy(ctr, iv, bs);
		while ((rec = fread(data, 1, bs, ifp)) == bs) {
			encrypt(ctr, res);
			for (size_t i = 0; i < bs; i++)
				data[i] ^= res[i];
			fwrite(data, bs, 1, ofp);
			for (size_t i = 0; i < bs && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < bs; i++)
			data[i] ^= res[i];
		fwrite(data, 1, rec, ofp);
	}
};
