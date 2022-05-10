#pragma once
#include <stdio.h>
#include <string.h>
template <typename data_t, size_t count>
class Crypto {
public:
	virtual void encrypt(data_t const *const &p, data_t *const &c) const {
		memcpy(c, p, sizeof(data_t[count]));
	}
	virtual void decrypt(data_t const *const &c, data_t *const &p) const {
		memcpy(p, c, sizeof(data_t[count]));
	}
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) {
		size_t rec;
		data_t p[count], c[count];
		while ((rec = fread(p, 1, sizeof(data_t[count]), ifp)) == sizeof(data_t[count])) {
			encrypt(p, c);
			fwrite(c, sizeof(data_t[count]), 1, ofp);
		}
		((uint8_t *)p)[sizeof(data_t[count]) - 1] = sizeof(data_t[count]) - rec;
		encrypt(p, c);
		fwrite(c, sizeof(data_t[count]), 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) {
		data_t c[count], p[count];
		fread(c, sizeof(data_t[count]), 1, ifp);
		decrypt(c, p);
		while (fread(c, sizeof(data_t[count]), 1, ifp)) {
			fwrite(p, sizeof(data_t[count]), 1, ofp);
			decrypt(c, p);
		}
		fwrite(p, 1, sizeof(data_t[count]) - ((uint8_t *)p)[sizeof(data_t[count]) - 1], ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, data_t const *const &iv) {
		size_t rec;
		data_t ctr[count], res[count], data[count];
		memcpy(ctr, iv, sizeof(data_t[count]));
		while ((rec = fread(data, 1, sizeof(data_t[count]), ifp)) == sizeof(data_t[count])) {
			encrypt(ctr, res);
			for (size_t i = 0; i < count; i++)
				data[i] ^= res[i];
			fwrite(data, sizeof(data_t[count]), 1, ofp);
			for (size_t i = 0; i < count && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < count; i++)
			data[i] ^= res[i];
		fwrite(data, 1, rec, ofp);
	}
};
