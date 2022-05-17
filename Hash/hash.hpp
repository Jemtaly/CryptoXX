#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define BS sizeof(typename H::block_t)
#define SS sizeof(typename H::sizex_t)
#define HS sizeof(typename H::hashx_t)
class HashRoot {
public:
	virtual ~HashRoot() = default;
	virtual void hash(FILE *const &ifp, uint8_t *const &buff) const = 0;
};
class HashObjectRoot {
public:
	virtual ~HashObjectRoot() = default;
	virtual void update(uint8_t const *data, uint8_t const *const &stop) = 0;
	virtual size_t get(uint8_t *const &buff) = 0;
};
template <size_t hs, size_t bs, size_t ss>
class Hash : public HashRoot {
public:
	typedef uint8_t hashx_t[hs];
	typedef uint8_t block_t[bs];
	typedef uint8_t sizex_t[ss];
	virtual void init(uint8_t *const &buff) const = 0;
	virtual void update(uint8_t const *const &data, uint8_t *const &buff) const = 0;
	void hash(FILE *const &ifp, uint8_t *const &buff) const {
		size_t rcrd;
		block_t blck;
		sizex_t size = {};
		init(buff);
		while ((rcrd = fread(blck, 1, bs, ifp)) == bs) {
			if ((size[ss - 1] += bs * 8) < bs * 8)
				for (size_t i = ss - 2; ++size[i] == 0; i--)
					;
			update(blck, buff);
		}
		if ((size[ss - 1] += rcrd * 8) < rcrd * 8)
			for (size_t i = ss - 2; ++size[i] == 0; i--)
				;
		memset(blck + rcrd, 0, bs - rcrd);
		blck[rcrd] = 0x80;
		if (rcrd >= bs - ss) {
			update(blck, buff);
			memset(blck, 0, bs);
		}
		memcpy(blck + bs - ss, size, ss);
		update(blck, buff);
	}
};
template <typename H>
class HashObject : public HashObjectRoot, H {
	size_t rcrd;
	typename H::block_t pers;
	typename H::sizex_t size;
	typename H::hashx_t hash;
public:
	HashObject() : rcrd(0), size{} {
		H::init(hash);
	}
	void update(uint8_t const *data, uint8_t const *const &stop) {
		if (BS + data <= rcrd + stop) {
			memcpy(pers + rcrd, data, BS - rcrd);
			if ((size[SS - 1] += BS * 8) < BS * 8)
				for (size_t i = SS - 2; ++size[i] == 0; i--)
					;
			H::update(pers, hash);
			data += BS - rcrd;
			rcrd -= rcrd;
			for (; data + BS <= stop; data += BS) {
				if ((size[SS - 1] += BS * 8) < BS * 8)
					for (size_t i = SS - 2; ++size[i] == 0; i--)
						;
				H::update(data, hash);
			}
		}
		memcpy(pers + rcrd, data, stop - data);
		rcrd += stop - data;
		data += stop - data;
	}
	size_t get(uint8_t *const &buff) {
		typename H::block_t temp;
		memcpy(temp, pers, rcrd);
		memcpy(buff, hash, HS);
		memset(temp + rcrd, 0, BS - rcrd);
		temp[rcrd] = 0x80;
		if ((size[SS - 1] += rcrd * 8) < rcrd * 8)
			for (size_t i = SS - 2; ++size[i] == 0; i--)
				;
		if (rcrd >= BS - SS) {
			H::update(temp, buff);
			memset(temp, 0, BS);
		}
		memcpy(temp + BS - SS, size, SS);
		H::update(temp, buff);
		return HS;
	}
};
