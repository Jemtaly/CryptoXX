#pragma once
#include <array>
#include <stdio.h>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
class MD5 : public Hash<64, 16> {
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
	static void compress(uint32_t *const &h, uint8_t const *const &blk) {
		uint32_t a = h[0];
		uint32_t b = h[1];
		uint32_t c = h[2];
		uint32_t d = h[3];
		uint32_t const *w = (uint32_t *)blk;
		for (int i = 0; i < 16; i++) {
			uint32_t f = (b & c) | (~b & d);
			uint32_t temp = d;
			d = c;
			c = b;
			uint32_t sum = a + f + k[i] + w[i];
			b += ROL32(sum, r[i]);
			a = temp;
		}
		for (int i = 16; i < 32; i++) {
			uint32_t f = (d & b) | (~d & c);
			uint32_t temp = d;
			d = c;
			c = b;
			uint32_t sum = a + f + k[i] + w[5 * i + 1 & 0xf];
			b += ROL32(sum, r[i]);
			a = temp;
		}
		for (int i = 32; i < 48; i++) {
			uint32_t f = b ^ c ^ d;
			uint32_t temp = d;
			d = c;
			c = b;
			uint32_t sum = a + f + k[i] + w[3 * i + 5 & 0xf];
			b += ROL32(sum, r[i]);
			a = temp;
		}
		for (int i = 48; i < 64; i++) {
			uint32_t f = c ^ (b | ~d);
			uint32_t temp = d;
			d = c;
			c = b;
			uint32_t sum = a + f + k[i] + w[7 * i & 0xf];
			b += ROL32(sum, r[i]);
			a = temp;
		}
		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
	}
	uint32_t h[4];
	uint64_t countr;
public:
	MD5() : h{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}, countr(0) {}
	void push(uint8_t const *const &blk) {
		compress(h, blk);
		countr += 512;
	}
	void cast(uint8_t const *const &fin, size_t const &len, uint8_t *const &buf) const {
		uint8_t blk[64];
		memcpy(blk, fin, len);
		memset(blk + len, 0, 64 - len);
		blk[len] = 0x80;
		uint32_t *t = (uint32_t *)buf;
		t[0] = h[0];
		t[1] = h[1];
		t[2] = h[2];
		t[3] = h[3];
		if (len >= 56) {
			compress(t, blk);
			memset(blk, 0, 56);
		}
		((uint64_t *)blk)[7] = countr + 8 * len;
		compress(t, blk);
	}
};
