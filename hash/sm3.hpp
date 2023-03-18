#pragma once
#include <array>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define PP0(x) ((x) ^ ROL32(x,  9) ^ ROL32(x, 17))
#define PP1(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) ((x) & (y) | (x) & (z) | (y) & (z))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define HHN(N, a, b, c, d, e, f, g, h, w, k, X, Y)                           \
	for (int j = X; j < Y; j++) {                                            \
		uint32_t a12 = ROL32(a  , 12);                                       \
		uint32_t aek = a12 + e + k[j];                                       \
		uint32_t ss1 = ROL32(aek,  7);                                       \
		uint32_t tt1 = FF##N(a, b, c) + d + (ss1 ^ a12) + (w[j] ^ w[j + 4]); \
		uint32_t tt2 = GG##N(e, f, g) + h +  ss1        +  w[j]            ; \
		d = c;                                                               \
		c = ROL32(b,  9);                                                    \
		b = a;                                                               \
		a =    (tt1);                                                        \
		h = g;                                                               \
		g = ROL32(f, 19);                                                    \
		f = e;                                                               \
		e = PP0(tt2);                                                        \
	}
class SM3Inner {
	static constexpr auto K = [](uint32_t TT00, uint32_t TT10) {
		std::array<uint32_t, 64> K = {};
		for (int j =  0; j < 16; j++) {
			K[j] = TT00 << j % 32 | TT00 >> (32 - j) % 32;
		}
		for (int j = 16; j < 64; j++) {
			K[j] = TT10 << j % 32 | TT10 >> (96 - j) % 32;
		}
		return K;
	}(0x79cc4519, 0x7a879d8a);
public:
	uint32_t rec[8] = {
		0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
		0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E,
	};
	void compress(uint8_t const *src) {
		uint32_t a = rec[0];
		uint32_t b = rec[1];
		uint32_t c = rec[2];
		uint32_t d = rec[3];
		uint32_t e = rec[4];
		uint32_t f = rec[5];
		uint32_t g = rec[6];
		uint32_t h = rec[7];
		uint32_t w[68], tmp;
		for (int j =  0; j < 16; j++) {
			uint8_t const *ref = &src[j << 2];
			w[j] = ref[0] << 24 | ref[1] << 16 | ref[2] << 8 | ref[3];
		}
		for (int j = 16; j < 68; j++) {
			tmp = w[j - 16] ^ w[j - 9] ^ ROL32(w[j -  3], 15);
			w[j] = PP1(tmp) ^ w[j - 6] ^ ROL32(w[j - 13],  7);
		}
		HHN(0, a, b, c, d, e, f, g, h, w, K,  0, 16);
		HHN(1, a, b, c, d, e, f, g, h, w, K, 16, 64);
		rec[0] ^= a;
		rec[1] ^= b;
		rec[2] ^= c;
		rec[3] ^= d;
		rec[4] ^= e;
		rec[5] ^= f;
		rec[6] ^= g;
		rec[7] ^= h;
	}
};
class SM3 {
	SM3Inner inner;
	uint64_t counter = 0;
public:
	static constexpr size_t BLOCK_SIZE = 64;
	static constexpr size_t DIGEST_SIZE = 32;
	void push(uint8_t const *src) {
		inner.compress(src);
		counter += 512;
	}
	void test(uint8_t const *src, size_t slen, uint8_t *dst) const {
		SM3Inner icopy = inner;
		uint8_t tmp[64];
		memcpy(tmp, src, slen);
		memset(tmp + slen, 0, 64 - slen);
		tmp[slen] = 0x80;
		if (slen >= 56) {
			icopy.compress(tmp);
			memset(tmp, 0, 56);
		}
		uint64_t totaln = counter + 8 * slen;
		uint8_t *totals = (uint8_t *)&totaln;
		tmp[63] = totals[0];
		tmp[62] = totals[1];
		tmp[61] = totals[2];
		tmp[60] = totals[3];
		tmp[59] = totals[4];
		tmp[58] = totals[5];
		tmp[57] = totals[6];
		tmp[56] = totals[7];
		icopy.compress(tmp);
		for (int j = 0; j < 8; j++) {
			uint8_t *ref = &dst[j << 2];
			ref[0] = icopy.rec[j] >> 24;
			ref[1] = icopy.rec[j] >> 16;
			ref[2] = icopy.rec[j] >>  8;
			ref[3] = icopy.rec[j]      ;
		}
	}
};
#undef PP0
#undef PP1
#undef FF0
#undef FF1
#undef GG0
#undef GG1
#undef HHN
