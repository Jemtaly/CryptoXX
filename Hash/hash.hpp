#pragma once
#include <stdint.h>
#include <string.h>
#define BLK sizeof(typename Hash::blk_t)
#define BUF sizeof(typename Hash::buf_t)
template <size_t blk_s, size_t buf_s>
class HashInterface {
public:
	using blk_t = uint8_t[blk_s];
	using buf_t = uint8_t[buf_s];
	virtual ~HashInterface() = default;
	virtual void push(uint8_t const *src) = 0;
	virtual void test(uint8_t const *src, size_t len, uint8_t *dst) const = 0;
};
template <class Hash>
class HashWrapper {
	Hash hash;
	size_t use;
	typename Hash::blk_t mem;
public:
	template <class... vals_t>
	HashWrapper(vals_t &&...vals):
		hash(std::forward<vals_t>(vals)...), use(0) {}
	void update(uint8_t const *src, uint8_t const *end) {
		if (BLK + src <= use + end) {
			memcpy(mem + use, src, BLK - use);
			hash.push(mem);
			src += BLK - use;
			use -= use;
			for (; src + BLK <= end; src += BLK) {
				hash.push(src);
			}
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
	}
	void digest(uint8_t *dst) const {
		hash.test(mem, use, dst);
	}
};
#undef BLK
#undef BUF
