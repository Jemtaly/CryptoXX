#pragma once
#include <stdint.h>
#include <string.h>
#define BLS sizeof(typename BC::blk_t)
template <size_t blk_s>
class BlockCipher {
public:
	using blk_t = uint8_t[blk_s];
	virtual ~BlockCipher() = default;
	virtual void encrypt(uint8_t const *src, uint8_t *dst) const = 0;
	virtual void decrypt(uint8_t const *src, uint8_t *dst) const = 0;
};
class BlockCipherFlow {
public:
	virtual ~BlockCipherFlow() = default;
	virtual uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) = 0;
	virtual uint8_t *fflush(uint8_t *dst) = 0;
};
template <class BC>
class Encrypter: public BlockCipherFlow {
	BC bc;
	size_t use;
	typename BC::blk_t buf;
public:
	template <class... vals_t>
	Encrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		if (BLS + src <= end + use) {
			memcpy(buf + use, src, BLS - use);
			bc.encrypt(buf, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src <= end; src += BLS, dst += BLS) {
				bc.encrypt(src, dst);
			}
		}
		memcpy(buf + use, src, end - src);
		use += end - src;
		src += end - src;
		return dst;
	}
	uint8_t *fflush(uint8_t *dst) {
		memset(buf + use, BLS - use, BLS - use);
		bc.encrypt(buf, dst);
		use -= use;
		dst += BLS;
		return dst;
	}
};
template <class BC>
class Decrypter: public BlockCipherFlow {
	BC bc;
	size_t use;
	typename BC::blk_t buf;
public:
	template <class... vals_t>
	Decrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		if (BLS + src < end + use) {
			memcpy(buf + use, src, BLS - use);
			bc.decrypt(buf, dst);
			src += BLS - use;
			dst += BLS;
			use -= use;
			for (; BLS + src < end; src += BLS, dst += BLS) {
				bc.decrypt(src, dst);
			}
		}
		memcpy(buf + use, src, end - src);
		use += end - src;
		src += end - src;
		return dst;
	}
	uint8_t *fflush(uint8_t *dst) {
		if (use != BLS) { // invalid padding
			return nullptr;
		}
		bc.decrypt(buf, dst);
		use -= use;
		dst += BLS - dst[BLS - 1];
		return dst;
	}
};
#include "stream.hpp"
template <class BC>
class CTRMode: public StreamCipher<BLS> {
	BC bc;
	typename BC::blk_t ctr;
public:
	template <class... vals_t>
	CTRMode(uint8_t const *civ, vals_t const &...vals):
		bc(vals...) {
		memcpy(ctr, civ, BLS);
	}
	void generate(uint8_t *dst) {
		bc.encrypt(ctr, dst);
		for (size_t i = BLS - 1; i < BLS && ++ctr[i] == 0; i--) {}
	}
};
#undef BLS
