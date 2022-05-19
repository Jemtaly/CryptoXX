#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define BLS sizeof(typename BC::block_t)
class BlockCipherRoot {
public:
	virtual ~BlockCipherRoot() = default;
	virtual void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const = 0;
	virtual void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv) const = 0;
};
class ECBEncrypterRoot {
public:
	virtual ~ECBEncrypterRoot() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) = 0;
};
class ECBDecrypterRoot {
public:
	virtual ~ECBDecrypterRoot() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) = 0;
};
class CTRXxcrypterRoot {
public:
	virtual ~CTRXxcrypterRoot() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) = 0;
};
template <size_t ibs>
class BlockCipher : public BlockCipherRoot {
public:
	typedef uint8_t block_t[ibs];
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const {
		size_t use;
		block_t src, dst;
		while ((use = fread(src, 1, ibs, ifp)) == ibs) {
			encrypt(src, dst);
			fwrite(dst, ibs, 1, ofp);
		}
		memset(src + use, ibs - use, ibs - use);
		encrypt(src, dst);
		fwrite(dst, ibs, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const {
		block_t src, dst;
		fread(src, ibs, 1, ifp);
		decrypt(src, dst);
		while (fread(src, ibs, 1, ifp)) {
			fwrite(dst, ibs, 1, ofp);
			decrypt(src, dst);
		}
		fwrite(dst, 1, ibs - dst[ibs - 1], ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv0) const {
		size_t use;
		block_t ctr, res, tmp;
		memcpy(ctr, iv0, ibs);
		while ((use = fread(tmp, 1, ibs, ifp)) == ibs) {
			encrypt(ctr, res);
			for (size_t i = 0; i < ibs; i++)
				tmp[i] ^= res[i];
			fwrite(tmp, ibs, 1, ofp);
			for (size_t i = 0; i < ibs && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < use; i++)
			tmp[i] ^= res[i];
		fwrite(tmp, 1, use, ofp);
	}
};
template <class BC>
class ECBEncrypter : public ECBEncrypterRoot, BC {
	size_t use;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBEncrypter(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (IBS + src <= end + use) {
			memcpy(mem + use, src, IBS - use);
			this->encrypt(mem, dst);
			src += IBS - use;
			dst += IBS;
			use -= use;
			for (; IBS + src <= end; src += IBS, dst += IBS)
				this->encrypt(src, dst);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (padding) {
			memset(mem + use, IBS - use, IBS - use);
			this->encrypt(mem, dst);
			use -= use;
			dst += IBS;
		}
		return dst;
	}
};
template <class BC>
class ECBDecrypter : public ECBDecrypterRoot, BC {
	size_t use;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBDecrypter(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (IBS + src < end + use) {
			memcpy(mem + use, src, IBS - use);
			this->decrypt(mem, dst);
			src += IBS - use;
			dst += IBS;
			use -= use;
			for (; IBS + src < end; src += IBS, dst += IBS)
				this->decrypt(src, dst);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (padding) {
			this->decrypt(mem, dst);
			use -= use;
			dst += IBS - dst[IBS - 1];
		}
		return dst;
	}
};
template <class BC>
class CTRXxcrypter : public CTRXxcrypterRoot, BC {
	size_t use;
	typename BC::block_t ctr;
public:
	template <class... vals_t>
	CTRXxcrypter(uint8_t const *const &iv, vals_t const &...vals) : BC(vals...), use(0) {
		memcpy(ctr, iv, IBS);
	}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) {
		typename BC::block_t res;
		if (IBS + src <= end + use) {
			this->encrypt(ctr, res);
			for (size_t i = use; i < IBS; i++)
				dst[i - use] = src[i - use] ^ res[i];
			for (size_t i = 0; i < IBS && ++ctr[i] == 0; i++)
				;
			src += IBS - use;
			dst += IBS - use;
			use -= use;
			for (; IBS + src <= end; src += IBS, dst += IBS) {
				this->encrypt(ctr, res);
				for (size_t i = 0; i < IBS; i++)
					dst[i] = src[i] ^ res[i];
				for (size_t i = 0; i < IBS && ++ctr[i] == 0; i++)
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
