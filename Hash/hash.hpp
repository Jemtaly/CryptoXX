#pragma once
#include <stdint.h>
#include <string.h>
#define HIS sizeof(typename HA::blk_t)
#define HOS sizeof(typename HA::buf_t)
template <size_t ibs, size_t obs>
class Hash {
public:
	typedef uint8_t blk_t[ibs];
	typedef uint8_t buf_t[obs];
	virtual ~Hash() = default;
	virtual void push(uint8_t const *const &blk) = 0;
	virtual void cast(uint8_t const *const &fin, size_t const &len, uint8_t *const &buf) const = 0;
};
class HashFlow {
public:
	virtual ~HashFlow() = default;
	virtual void update(uint8_t const *src, uint8_t const *const &end) = 0;
	virtual void obtain(uint8_t *const &buf) const = 0;
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
	void update(uint8_t const *src, uint8_t const *const &end) {
		if (HIS + src <= use + end) {
			memcpy(mem + use, src, HIS - use);
			hash.push(mem);
			src += HIS - use;
			use -= use;
			for (; src + HIS <= end; src += HIS) {
				hash.push(src);
			}
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
	}
	void obtain(uint8_t *const &buf) const {
		hash.cast(mem, use, buf);
	}
};
