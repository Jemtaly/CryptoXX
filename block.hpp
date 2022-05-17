#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define bls sizeof(typename BC::block_t)
template <size_t blocksize>
class BlockCipher {
public:
	typedef uint8_t block_t[blocksize];
	virtual ~BlockCipher() = default;
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
};
class FileXxcryptRoot {
public:
	virtual ~FileXxcryptRoot() = default;
	virtual void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) const = 0;
};
class ECBEncryptRoot {
public:
	virtual ~ECBEncryptRoot() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) = 0;
};
class ECBDecryptRoot {
public:
	virtual ~ECBDecryptRoot() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) = 0;
};
class CTRXxcryptRoot {
public:
	virtual ~CTRXxcryptRoot() = default;
	virtual void reset(uint8_t const *const &iv) = 0;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) = 0;
};
template <class BC>
class FileXxcrypt : public FileXxcryptRoot, protected BC {
public:
	template <class... vals_t>
	FileXxcrypt(vals_t const &...vals) : BC(vals...) {}
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const {
		size_t num;
		typename BC::block_t src, dst;
		while ((num = fread(src, 1, bls, ifp)) == bls) {
			this->encrypt(src, dst);
			fwrite(dst, bls, 1, ofp);
		}
		src[bls - 1] = bls - num;
		this->encrypt(src, dst);
		fwrite(dst, bls, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const {
		typename BC::block_t src, dst;
		fread(src, bls, 1, ifp);
		this->decrypt(src, dst);
		while (fread(src, bls, 1, ifp)) {
			fwrite(dst, bls, 1, ofp);
			this->decrypt(src, dst);
		}
		fwrite(dst, 1, bls - dst[bls - 1], ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) const {
		size_t num;
		typename BC::block_t ctr, res, tmp;
		memcpy(ctr, iv, bls);
		while ((num = fread(tmp, 1, bls, ifp)) == bls) {
			this->encrypt(ctr, res);
			for (size_t i = 0; i < bls; i++)
				tmp[i] ^= res[i];
			fwrite(tmp, bls, 1, ofp);
			for (size_t i = 0; i < bls && ++ctr[i] == 0; i++)
				;
		}
		this->encrypt(ctr, res);
		for (size_t i = 0; i < num; i++)
			tmp[i] ^= res[i];
		fwrite(tmp, 1, num, ofp);
	}
};
template <class BC>
class ECBEncrypt : public ECBEncryptRoot, protected BC {
	size_t rec;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBEncrypt(vals_t const &...vals) : BC(vals...), rec(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (bls + src <= end + rec) {
			memcpy(mem + rec, src, bls - rec);
			this->encrypt(mem, dst);
			src += bls - rec;
			dst += bls;
			rec -= rec;
			for (; bls + src <= end; src += bls, dst += bls)
				this->encrypt(src, dst);
		}
		memcpy(mem + rec, src, end - src);
		rec += end - src;
		src += end - src;
		if (padding) {
			mem[bls - 1] = bls - rec;
			this->encrypt(mem, dst);
			rec -= rec;
			dst += bls;
		}
		return dst;
	}
};
template <class BC>
class ECBDecrypt : public ECBDecryptRoot, protected BC {
	size_t rec;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBDecrypt(vals_t const &...vals) : BC(vals...), rec(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (bls + src < end + rec) {
			memcpy(mem + rec, src, bls - rec);
			this->decrypt(mem, dst);
			src += bls - rec;
			dst += bls;
			rec -= rec;
			for (; bls + src < end; src += bls, dst += bls)
				this->decrypt(src, dst);
		}
		memcpy(mem + rec, src, end - src);
		rec += end - src;
		src += end - src;
		if (padding) {
			this->decrypt(mem, dst);
			rec -= rec;
			dst += bls - dst[bls - 1];
		}
		return dst;
	}
};
template <class BC>
class CTRXxcrypt : public CTRXxcryptRoot, protected BC {
	size_t use;
	typename BC::block_t ctr;
public:
	template <class... vals_t>
	CTRXxcrypt(vals_t const &...vals) : BC(vals...) {}
	void reset(uint8_t const *const &iv) {
		use = 0;
		memcpy(ctr, iv, bls);
	}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) {
		typename BC::block_t res;
		if (bls + src <= end + use) {
			this->encrypt(ctr, res);
			for (size_t i = use; i < bls; i++)
				dst[i - use] = src[i - use] ^ res[i];
			for (size_t i = 0; i < bls && ++ctr[i] == 0; i++)
				;
			src += bls - use;
			dst += bls - use;
			use -= use;
			for (; bls + src <= end; src += bls, dst += bls) {
				this->encrypt(ctr, res);
				for (size_t i = 0; i < bls; i++)
					dst[i] = src[i] ^ res[i];
				for (size_t i = 0; i < bls && ++ctr[i] == 0; i++)
					;
			}
		}
		this->encrypt(ctr, res);
		for (size_t i = use; i + src < end + use; i++)
			dst[i - use] = src[i - use] ^ res[i];
		use += end - src;
		dst += end - src;
		src += end - src;
		return dst;
	}
};
