#pragma once
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define BLS sizeof(typename BC::block_t)
template <size_t bls>
class BlockCipher {
public:
	typedef uint8_t block_t[bls];
	virtual void encrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
	virtual void decrypt(uint8_t const *const &src, uint8_t *const &dst) const = 0;
};
class BlockCipherFlow {
public:
	virtual ~BlockCipherFlow() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final = true) = 0;
};
template <class BC> // Block Cipher Algorithm or Enc Mode
class Encrypter : public BlockCipherFlow, BC {
	typename BC::block_t mem;
	size_t use;
public:
	template <class... vals_t>
	Encrypter(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final) {
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
		if (final) {
			memset(mem + use, BLS - use, BLS - use);
			this->encrypt(mem, dst);
			use -= use;
			dst += BLS;
		}
		return dst;
	}
};
template <class BC> // Block Cipher Algorithm or Dec Mode
class Decrypter : public BlockCipherFlow, BC {
	typename BC::block_t mem;
	size_t use;
public:
	template <class... vals_t>
	Decrypter(vals_t const &...vals) : BC(vals...), use(0) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst, bool const &final) {
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
		if (final) {
			assert(use == BLS);
			this->decrypt(mem, dst);
			use -= use;
			dst += BLS - dst[BLS - 1];
		}
		return dst;
	}
};
class StreamCipherFlow {
public:
	virtual ~StreamCipherFlow() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) = 0;
};
template <class BC> // Block Cipher Algorithm only
class CTRCrypter : public StreamCipherFlow, BC {
	typename BC::block_t ctr;
	size_t use;
public:
	template <class... vals_t>
	CTRCrypter(uint8_t const *const &iv, vals_t const &...vals) : BC(vals...), use(0) {
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
