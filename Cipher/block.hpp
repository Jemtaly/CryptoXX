#pragma once
#include <stdint.h>
#include <string.h>
#include <utility> // std::forward
#define BLK sizeof(typename BlockCipher::blk_t)
template <size_t blk_s>
class BlockCipherInterface {
public:
	using blk_t = uint8_t[blk_s];
	virtual ~BlockCipherInterface() = default;
	virtual void encrypt(uint8_t const *src, uint8_t *dst) const = 0;
	virtual void decrypt(uint8_t const *src, uint8_t *dst) const = 0;
};
template <class BlockCipher>
class BlockCipherEncrypter {
	BlockCipher bc;
	size_t use;
	typename BlockCipher::blk_t buf;
public:
	template <class... vals_t>
	BlockCipherEncrypter(vals_t &&...vals):
		bc(std::forward<vals_t>(vals)...), use(0) {}
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
class BlockCipherDecrypter {
	BlockCipher bc;
	size_t use;
	typename BlockCipher::blk_t buf;
public:
	template <class... vals_t>
	BlockCipherDecrypter(vals_t &&...vals):
		bc(std::forward<vals_t>(vals)...), use(0) {}
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
#undef BLK
