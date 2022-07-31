#pragma once
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define BLS sizeof(typename BC::block_t)
template <size_t bls>
class BlockCipher {
public:
	typedef uint8_t block_t[bls];
	virtual ~BlockCipher() = default;
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
};
template <size_t bls>
class EncryptionMode {
public:
	typedef uint8_t block_t[bls];
	virtual ~EncryptionMode() = default;
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) = 0;
};
template <size_t bls>
class DecryptionMode {
public:
	typedef uint8_t block_t[bls];
	virtual ~DecryptionMode() = default;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) = 0;
};
class BlockCipherFlow {
public:
	virtual ~BlockCipherFlow() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final = true) = 0;
};
template <class BC>
class Encrypter: public BlockCipherFlow {
	size_t use;
	typename BC::block_t mem;
	BC bc;
public:
	template <class... vals_t>
	Encrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final) {
		if (BLS + src <= end + use) {
			memcpy(mem + use, src, BLS - use);
			bc.encrypt(mem, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src <= end; src += BLS, dst += BLS) {
				bc.encrypt(src, dst);
			}
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (final) {
			memset(mem + use, BLS - use, BLS - use);
			bc.encrypt(mem, dst);
			use -= use;
			dst += BLS;
		}
		return dst;
	}
};
template <class BC>
class Decrypter: public BlockCipherFlow {
	size_t use;
	typename BC::block_t mem;
	BC bc;
public:
	template <class... vals_t>
	Decrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final) {
		if (BLS + src < end + use) {
			memcpy(mem + use, src, BLS - use);
			bc.decrypt(mem, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src < end; src += BLS, dst += BLS) {
				bc.decrypt(src, dst);
			}
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
		if (final) {
			assert(use == BLS);
			bc.decrypt(mem, dst);
			use -= use;
			dst += BLS - dst[BLS - 1];
		}
		return dst;
	}
};
#include "stream.hpp"
template <class BC>
class CTRCrypter: public StreamCipherFlow {
	size_t use;
	typename BC::block_t ctr;
	BC const bc;
public:
	template <class... vals_t>
	CTRCrypter(uint8_t const *const &iv, vals_t const &...vals):
		bc(vals...), use(0) {
		memcpy(ctr, iv, BLS);
	}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) {
		typename BC::block_t res;
		if (BLS + src <= end + use) {
			bc.encrypt(ctr, res);
			for (size_t i = use; i < BLS; i++) {
				dst[i - use] = src[i - use] ^ res[i];
			}
			for (size_t i = 0; i < BLS && ++ctr[i] == 0; i++) {}
			src += BLS - use;
			dst += BLS - use;
			use -= use;
			for (; BLS + src <= end; src += BLS, dst += BLS) {
				bc.encrypt(ctr, res);
				for (size_t i = 0; i < BLS; i++) {
					dst[i] = src[i] ^ res[i];
				}
				for (size_t i = 0; i < BLS && ++ctr[i] == 0; i++) {}
			}
		}
		bc.encrypt(ctr, res);
		for (size_t i = use; i + src < end + use; i++) {
			dst[i - use] = src[i - use] ^ res[i];
		}
		use += end - src;
		dst += end - src;
		src += end - src;
		return dst;
	}
};
