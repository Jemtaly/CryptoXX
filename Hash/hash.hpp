#pragma once
#include <stdint.h>
#include <string.h>
#define INP sizeof(typename Hash::inp_t)
#define OUT sizeof(typename Hash::out_t)
template <size_t inp_s, size_t out_s>
class HashInterface {
public:
	using inp_t = uint8_t[inp_s];
	using out_t = uint8_t[out_s];
	virtual ~HashInterface() = default;
	virtual void push(uint8_t const *src) = 0;
	virtual void test(uint8_t const *src, size_t len, uint8_t *dst) const = 0;
};
template <class Hash>
class HashWrapper {
	Hash hash;
	size_t use;
	typename Hash::inp_t mem;
public:
	template <class... vals_t>
	HashWrapper(vals_t &&...vals):
		hash(std::forward<vals_t>(vals)...), use(0) {}
	void update(uint8_t const *src, uint8_t const *end) {
		if (INP + src <= use + end) {
			memcpy(mem + use, src, INP - use);
			hash.push(mem);
			src += INP - use;
			use -= use;
			for (; src + INP <= end; src += INP) {
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
#undef INP
#undef OUT
