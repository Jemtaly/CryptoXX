#pragma once
#include <stdint.h>
#include <string.h>
#define IBS sizeof(typename HF::blk_t)
#define OBS sizeof(typename HF::buf_t)
template <size_t ibs, size_t obs>
class Hash {
public:
	typedef uint8_t blk_t[ibs];
	typedef uint8_t buf_t[obs];
	virtual void push(uint8_t const *const &blk) const = 0;
	virtual void cast(uint8_t const *const &fin, size_t const &len, uint8_t *const &buf) const = 0;
};
class HashFlow {
public:
	virtual ~HashFlow() = default;
	virtual void update(uint8_t const *src, uint8_t const *const &end) = 0;
	virtual void obtain(uint8_t *const &buf) const = 0;
};
template <class HF>
class Hasher : public HashFlow, HF {
	size_t use;
	typename HF::blk_t mem;
	typename HF::sta_t sta;
public:
	template <class... vals_t>
	Hasher(vals_t const &...vals) : HF(vals...), use(0) {}
	void update(uint8_t const *src, uint8_t const *const &end) {
		if (IBS + src <= use + end) {
			memcpy(mem + use, src, IBS - use);
			this->push(mem);
			src += IBS - use;
			use -= use;
			for (; src + IBS <= end; src += IBS)
				this->push(src);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
	}
	void obtain(uint8_t *const &buf) const {
		this->cast(mem, use, buf);
	}
};
