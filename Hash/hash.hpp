#pragma once
#include <stdint.h>
#include <string.h>
#define BLS sizeof(typename HA::blk_t)
#define BFS sizeof(typename HA::buf_t)
template <size_t blk_s, size_t buf_s>
class Hash {
public:
	using blk_t = uint8_t[blk_s];
	using buf_t = uint8_t[buf_s];
	virtual ~Hash() = default;
	virtual void push(uint8_t const *src) = 0;
	virtual void test(uint8_t const *src, size_t len, uint8_t *dst) const = 0;
};
class HashFlow {
public:
	virtual ~HashFlow() = default;
	virtual void update(uint8_t const *src, uint8_t const *end) = 0;
	virtual void digest(uint8_t *dst) const = 0;
};
template <class HA>
class Hasher: public HashFlow {
	HA hash;
	size_t use;
	typename HA::blk_t mem;
public:
	template <class... vals_t>
	Hasher(vals_t const &...vals):
		hash(vals...), use(0) {}
	void update(uint8_t const *src, uint8_t const *end) {
		if (BLS + src <= use + end) {
			memcpy(mem + use, src, BLS - use);
			hash.push(mem);
			src += BLS - use;
			use -= use;
			for (; src + BLS <= end; src += BLS) {
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
#undef BLS
#undef BFS
