#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
template <size_t bs>
class Crypto {
public:
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) {
		size_t record;
		uint8_t src[bs], dst[bs];
		while ((record = fread(src, 1, bs, ifp)) == bs) {
			encrypt(src, dst);
			fwrite(dst, bs, 1, ofp);
		}
		src[bs - 1] = bs - record;
		encrypt(src, dst);
		fwrite(dst, bs, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) {
		uint8_t src[bs], dst[bs];
		fread(src, bs, 1, ifp);
		decrypt(src, dst);
		while (fread(src, bs, 1, ifp)) {
			fwrite(dst, bs, 1, ofp);
			decrypt(src, dst);
		}
		size_t tail = bs - dst[bs - 1];
		fwrite(dst, 1, tail, ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) {
		size_t record;
		uint8_t ctr[bs], res[bs], tmp[bs];
		memcpy(ctr, iv, bs);
		while ((record = fread(tmp, 1, bs, ifp)) == bs) {
			encrypt(ctr, res);
			for (size_t i = 0; i < bs; i++)
				tmp[i] ^= res[i];
			fwrite(tmp, bs, 1, ofp);
			for (size_t i = 0; i < bs && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < record; i++)
			tmp[i] ^= res[i];
		fwrite(tmp, 1, record, ofp);
	}
	size_t ECB_encrypt(uint8_t const *const &src, uint8_t *const &dst, size_t const &size) {
		size_t tail = size % bs, front = size - tail;
		for (size_t n = 0; n < front; n += bs)
			encrypt(src + n, dst + n);
		uint8_t tmp[bs];
		memcpy(src + front, tmp, tail);
		tmp[bs - 1] = bs - tail;
		encrypt(tmp, dst + front);
		return front + bs;
	}
	size_t ECB_decrypt(uint8_t const *const &src, uint8_t *const &dst, size_t const &size) {
		size_t front = size - bs;
		for (size_t n = 0; n < front; n += bs)
			decrypt(src + n, dst + n);
		uint8_t tmp[bs];
		decrypt(src + front, tmp);
		size_t tail = bs - tmp[bs - 1];
		memcpy(tmp, dst + front, tail);
		return front + tail;
	}
	void CTR_xxcrypt(uint8_t const *const &src, uint8_t *const &dst, uint8_t const *const &iv, size_t const &size) {
		size_t tail = size % bs, front = size - tail;
		uint8_t ctr[bs], res[bs];
		memcpy(ctr, iv, bs);
		for (size_t n = 0; n < front; n += bs) {
			encrypt(ctr, res);
			for (size_t i = 0; i < bs; i++)
				dst[n + i] = src[n + i] ^ res[i];
			for (size_t i = 0; i < bs && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < tail; i++)
			dst[front + i] = src[front + i] ^ res[i];
	}
};
