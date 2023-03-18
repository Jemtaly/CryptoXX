#pragma once
#include <stdint.h>
#include <string.h>
#include <utility> // std::forward
#define BLK BlockCipher::BLOCK_SIZE
template <class BlockCipher>
class BlockCipherEncrypter {
	BlockCipher bc;
	size_t use;
	uint8_t buf[BLK];
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
	uint8_t buf[BLK];
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
