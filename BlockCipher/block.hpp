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
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) = 0;
};
template <size_t bls>
class BlockCipher : public BlockCipherRoot {
public:
	typedef uint8_t block_t[bls];
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	void ECB_encrypt(FILE *const &ifp, FILE *const &ofp) const {
		size_t use;
		block_t src, dst;
		while ((use = fread(src, 1, bls, ifp)) == bls) {
			encrypt(src, dst);
			fwrite(dst, bls, 1, ofp);
		}
		memset(src + use, bls - use, bls - use);
		encrypt(src, dst);
		fwrite(dst, bls, 1, ofp);
	}
	void ECB_decrypt(FILE *const &ifp, FILE *const &ofp) const {
		block_t src, dst;
		fread(src, bls, 1, ifp);
		decrypt(src, dst);
		while (fread(src, bls, 1, ifp)) {
			fwrite(dst, bls, 1, ofp);
			decrypt(src, dst);
		}
		fwrite(dst, 1, bls - dst[bls - 1], ofp);
	}
	void CTR_xxcrypt(FILE *const &ifp, FILE *const &ofp, uint8_t const *const &iv0) const {
		size_t use;
		block_t ctr, res, tmp;
		memcpy(ctr, iv0, bls);
		while ((use = fread(tmp, 1, bls, ifp)) == bls) {
			encrypt(ctr, res);
			for (size_t i = 0; i < bls; i++)
				tmp[i] ^= res[i];
			fwrite(tmp, bls, 1, ofp);
			for (size_t i = 0; i < bls && ++ctr[i] == 0; i++)
				;
		}
		encrypt(ctr, res);
		for (size_t i = 0; i < use; i++)
			tmp[i] ^= res[i];
		fwrite(tmp, 1, use, ofp);
	}
};
template <class BC>
class ECBEncrypt : public ECBEncryptRoot, BC {
	size_t use;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBEncrypt(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (BLS + src <= end + use) {
			memcpy(mem + use, src, BLS - use);
			this->encrypt(mem, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src <= end; src += BLS, dst += BLS)
				this->encrypt(src, dst);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (padding) {
			memset(mem + use, BLS - use, BLS - use);
			this->encrypt(mem, dst);
			use -= use;
			dst += BLS;
		}
		return dst;
	}
};
template <class BC>
class ECBDecrypt : public ECBDecryptRoot, BC {
	size_t use;
	typename BC::block_t mem;
public:
	template <class... vals_t>
	ECBDecrypt(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &padding) {
		if (BLS + src < end + use) {
			memcpy(mem + use, src, BLS - use);
			this->decrypt(mem, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src < end; src += BLS, dst += BLS)
				this->decrypt(src, dst);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (padding) {
			this->decrypt(mem, dst);
			use -= use;
			dst += BLS - dst[BLS - 1];
		}
		return dst;
	}
};
template <class BC>
class CTRXxcrypt : public CTRXxcryptRoot, BC {
	size_t use;
	typename BC::block_t ctr;
public:
	template <class... vals_t>
	CTRXxcrypt(uint8_t const *const &iv, vals_t const &...vals) : BC(vals...), use(0) {
		memcpy(ctr, iv, BLS);
	}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) {
		typename BC::block_t res;
		if (BLS + src <= end + use) {
			this->encrypt(ctr, res);
			for (size_t i = use; i < BLS; i++)
				dst[i - use] = src[i - use] ^ res[i];
			for (size_t i = 0; i < BLS && ++ctr[i] == 0; i++)
				;
			src += BLS - use;
			dst += BLS - use;
			use -= use;
			for (; BLS + src <= end; src += BLS, dst += BLS) {
				this->encrypt(ctr, res);
				for (size_t i = 0; i < BLS; i++)
					dst[i] = src[i] ^ res[i];
				for (size_t i = 0; i < BLS && ++ctr[i] == 0; i++)
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
