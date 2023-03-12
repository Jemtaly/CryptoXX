#pragma once
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define FF0(b, c, d) ((b) & (c) | ~(b) & (d))
#define FF1(b, c, d) ((d) & (b) | ~(d) & (c))
#define FF2(b, c, d) ((b) ^ (c) ^ (d))
#define FF3(b, c, d) ((c) ^ ((b) | ~(d)))
#define GG0(i)      (i)
#define GG1(i) (5 * (i) + 1 & 0xf)
#define GG2(i) (3 * (i) + 5 & 0xf)
#define GG3(i) (7 * (i)     & 0xf)
#define HHN(N, a, b, c, d, w, k, r, X, Y)                     \
	for (int i = X; i < Y; i++) {                             \
		uint32_t s = a + FF##N(b, c, d) + k[i] + w[GG##N(i)]; \
		a = d;                                                \
		d = c;                                                \
		c = b;                                                \
		b += ROL32(s, r[i]);                                  \
	}
class MD5Inner {
	static constexpr uint32_t k[64] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
	};
	static constexpr uint8_t r[64] = {
		0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
		0x07, 0x0c, 0x11, 0x16, 0x07, 0x0c, 0x11, 0x16,
		0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
		0x05, 0x09, 0x0e, 0x14, 0x05, 0x09, 0x0e, 0x14,
		0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
		0x04, 0x0b, 0x10, 0x17, 0x04, 0x0b, 0x10, 0x17,
		0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
		0x06, 0x0a, 0x0f, 0x15, 0x06, 0x0a, 0x0f, 0x15,
	};
public:
	uint32_t h[4] = {
		0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476,
	};
	void compress(uint8_t const *const &src) {
		uint32_t a = h[0];
		uint32_t b = h[1];
		uint32_t c = h[2];
		uint32_t d = h[3];
		uint32_t const *w = (uint32_t *)src;
		HHN(0, a, b, c, d, w, k, r,  0, 16);
		HHN(1, a, b, c, d, w, k, r, 16, 32);
		HHN(2, a, b, c, d, w, k, r, 32, 48);
		HHN(3, a, b, c, d, w, k, r, 48, 64);
		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
	}
};
class MD5: public Hash<64, 16> {
	MD5Inner inner;
	uint64_t counter = 0;
public:
	void push(uint8_t const *const &src) {
		inner.compress(src);
		counter += 512;
	}
	void test(uint8_t const *const &src, size_t const &len, uint8_t *const &dst) const {
		MD5Inner copy = inner;
		uint8_t tmp[64];
		memcpy(tmp, src, len);
		memset(tmp + len, 0, 64 - len);
		tmp[len] = 0x80;
		if (len >= 56) {
			copy.compress(tmp);
			memset(tmp, 0, 56);
		}
		((uint64_t *)tmp)[7] = counter + 8 * len;
		copy.compress(tmp);
		((uint32_t *)dst)[0] = copy.h[0];
		((uint32_t *)dst)[1] = copy.h[1];
		((uint32_t *)dst)[2] = copy.h[2];
		((uint32_t *)dst)[3] = copy.h[3];
	}
};
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef GG0
#undef GG1
#undef GG2
#undef GG3
#undef HHN
