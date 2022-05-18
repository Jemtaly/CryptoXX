#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define BLS sizeof(typename H::blk_t)
class HashRoot {
public:
	virtual ~HashRoot() = default;
	virtual void hash(FILE *const &ifp, uint8_t *const &buf) const = 0;
};
class HashObjectRoot {
public:
	virtual ~HashObjectRoot() = default;
	virtual void update(uint8_t const *src, uint8_t const *const &end) = 0;
	virtual void obtain(uint8_t *const &buf) const = 0;
};
template <size_t bls, size_t rcs, size_t bfs>
class Hash : public HashRoot {
public:
	typedef uint8_t blk_t[bls];
	typedef uint8_t rec_t[rcs];
	typedef uint8_t buf_t[bfs];
	virtual void init(uint8_t *const &rec) const = 0;
	virtual void load(uint8_t const *const &rci, uint8_t *const &rco, uint8_t const *const &blk) const = 0;
	virtual void save(uint8_t const *const &rec, uint8_t *const &buf, uint8_t const *const &src, size_t const &len) const = 0;
	void hash(FILE *const &ifp, uint8_t *const &buf) const {
		size_t use;
		blk_t blk;
		rec_t rec;
		init(rec);
		while ((use = fread(blk, 1, bls, ifp)) == bls)
			load(rec, rec, blk);
		save(rec, buf, blk, use);
	}
};
template <class H>
class HashObject : public HashObjectRoot, H {
	size_t use;
	typename H::blk_t mem;
	typename H::rec_t rec;
public:
	template <class... vals_t>
	HashObject(vals_t const &...vals) : H(vals...), use(0) {
		this->init(rec);
	}
	void update(uint8_t const *src, uint8_t const *const &end) {
		if (BLS + src <= use + end) {
			memcpy(mem + use, src, BLS - use);
			this->load(rec, rec, mem);
			src += BLS - use;
			use -= use;
			for (; src + BLS <= end; src += BLS)
				this->load(rec, rec, src);
		}
		memcpy(mem + use, src, end - src);
		use += end - src;
		src += end - src;
	}
	void obtain(uint8_t *const &buf) const {
		this->save(rec, buf, mem, use);
	}
};
