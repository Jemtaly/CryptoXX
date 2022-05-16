#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define bs sizeof(typename BC::block_t)
class BaseBlockCipher {
public:
	virtual ~BaseBlockCipher() = default;
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
};
class BaseFileXxcrypt {
public:
	virtual ~BaseFileXxcrypt() = default;
	virtual void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) const = 0;
};
template <size_t blocksize>
class BlockCipher : public BaseBlockCipher {
public:
	typedef uint8_t block_t[blocksize];
};
template <class BC>
class FileXxcrypt : public BaseFileXxcrypt {
	BC bc;
public:
	template <class... vals_t>
	FileXxcrypt(vals_t const &...vals) : bc(vals...) {}
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const {
		size_t record;
		typename BC::block_t src, dst;
		while ((record = fread(src, 1, bs, ifp)) == bs) {
			bc.encrypt(src, dst);
			fwrite(dst, bs, 1, ofp);
		}
		src[bs - 1] = bs - record;
		bc.encrypt(src, dst);
		fwrite(dst, bs, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const {
		typename BC::block_t src, dst;
		fread(src, bs, 1, ifp);
		bc.decrypt(src, dst);
		while (fread(src, bs, 1, ifp)) {
			fwrite(dst, bs, 1, ofp);
			bc.decrypt(src, dst);
		}
		size_t record = bs - dst[bs - 1];
		fwrite(dst, 1, record, ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) const {
		size_t record;
		typename BC::block_t ctr, res, tmp;
		memcpy(ctr, iv, bs);
		while ((record = fread(tmp, 1, bs, ifp)) == bs) {
			bc.encrypt(ctr, res);
			for (size_t i = 0; i < bs; i++)
				tmp[i] ^= res[i];
			fwrite(tmp, bs, 1, ofp);
			for (size_t i = 0; i < bs && ++ctr[i] == 0; i++)
				;
		}
		bc.encrypt(ctr, res);
		for (size_t i = 0; i < record; i++)
			tmp[i] ^= res[i];
		fwrite(tmp, 1, record, ofp);
	}
};
