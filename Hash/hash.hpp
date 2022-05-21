#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define IBS sizeof(typename HF::blk_t)
#define OBS sizeof(typename HF::buf_t)
class HashFunctionRoot {
public:
	virtual ~HashFunctionRoot() = default;
	virtual void hash(FILE *const &ifp, uint8_t *const &buf) const = 0;
};
class HashObjectRoot {
public:
	virtual ~HashObjectRoot() = default;
	virtual void update(uint8_t const *src, uint8_t const *const &end) = 0;
	virtual void obtain(uint8_t *const &buf) const = 0;
};
template <size_t ibs, size_t obs, typename state_t>
class HashFunction : public HashFunctionRoot {
public:
	typedef uint8_t blk_t[ibs];
	typedef uint8_t buf_t[obs];
	typedef state_t sta_t;
	virtual void init(sta_t &sta) const = 0;
	virtual void push(sta_t &sta, uint8_t const *const &blk) const = 0;
	virtual void cast(sta_t const &sta, uint8_t *const &buf, uint8_t const *const &fin, size_t const &len) const = 0;
	void hash(FILE *const &ifp, uint8_t *const &buf) const {
		blk_t blk;
		sta_t sta;
		init(sta);
		size_t use;
		while ((use = fread(blk, 1, ibs, ifp)) == ibs)
			push(sta, blk);
		cast(sta, buf, blk, use);
	}
};
template <class HF>
class HashObject : public HashObjectRoot, HF {
	size_t use;
	typename HF::blk_t mem;
	typename HF::sta_t sta;
public:
	template <class... vals_t>
	HashObject(vals_t const &...vals) : HF(vals...), use(0) {
		this->init(sta);
	}
	void update(uint8_t const *src, uint8_t const *const &end) {
		if (IBS + src <= use + end) {
			memcpy(mem + use, src, IBS - use);
			this->push(sta, mem);
			src += IBS - use;
			use -= use;
			for (; src + IBS <= end; src += IBS)
				this->push(sta, src);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
	}
	void obtain(uint8_t *const &buf) const {
		this->cast(sta, buf, mem, use);
	}
};
