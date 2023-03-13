#pragma once
#include <stdint.h>
#include <string.h>
#include "stream.hpp"
#define BLK sizeof(typename BlockCipher::blk_t)
template <size_t blk_s>
class BlockCipherBase {
public:
	using blk_t = uint8_t[blk_s];
	virtual ~BlockCipherBase() = default;
	virtual void encrypt(uint8_t const *src, uint8_t *dst) const = 0;
	virtual void decrypt(uint8_t const *src, uint8_t *dst) const = 0;
};
class BlockCipherCrypterBase {
public:
	virtual ~BlockCipherCrypterBase() = default;
	virtual uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) = 0;
	virtual uint8_t *fflush(uint8_t *dst) = 0;
};
template <class BlockCipher>
class BlockCipherEncrypter: public BlockCipherCrypterBase {
	BlockCipher bc;
	size_t use;
	typename BlockCipher::blk_t buf;
public:
	template <class... vals_t>
	BlockCipherEncrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		if (BLK + src <= end + use) {
			memcpy(buf + use, src, BLK - use);
			bc.encrypt(buf, dst);
			src += BLK - use;
			dst += BLK;
			use -= use;
			for (; BLK + src <= end; src += BLK, dst += BLK) {
				bc.encrypt(src, dst);
			}
		}
		memcpy(buf + use, src, end - src);
		use += end - src;
		src += end - src;
		return dst;
	}
	uint8_t *fflush(uint8_t *dst) {
		memset(buf + use, BLK - use, BLK - use);
		bc.encrypt(buf, dst);
		use -= use;
		dst += BLK;
		return dst;
	}
};
template <class BlockCipher>
class BlockCipherDecrypter: public BlockCipherCrypterBase {
	BlockCipher bc;
	size_t use;
	typename BlockCipher::blk_t buf;
public:
	template <class... vals_t>
	BlockCipherDecrypter(vals_t const &...vals):
		bc(vals...), use(0) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		if (BLK + src < end + use) {
			memcpy(buf + use, src, BLK - use);
			bc.decrypt(buf, dst);
			src += BLK - use;
			dst += BLK;
			use -= use;
			for (; BLK + src < end; src += BLK, dst += BLK) {
				bc.decrypt(src, dst);
			}
		}
		memcpy(buf + use, src, end - src);
		use += end - src;
		src += end - src;
		return dst;
	}
	uint8_t *fflush(uint8_t *dst) {
		if (use != BLK) { // invalid padding
			return nullptr;
		}
		bc.decrypt(buf, dst);
		use -= use;
		dst += BLK - dst[BLK - 1];
		return dst;
	}
};
template <class BlockCipher>
class CTRMode: public StreamCipherBase<BLK> {
	BlockCipher bc;
	typename BlockCipher::blk_t ctr;
public:
	template <class... vals_t>
	CTRMode(uint8_t const *civ, vals_t const &...vals):
		bc(vals...) {
		memcpy(ctr, civ, BLK);
	}
	void generate(uint8_t *dst) {
		bc.encrypt(ctr, dst);
		for (size_t i = BLK - 1; i < BLK && ++ctr[i] == 0; i--) {}
	}
};
#undef BLK
