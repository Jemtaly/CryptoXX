#pragma once
#include <assert.h>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define FF0(b, c, d) ((b & c) | (~b & d))
#define FF1(b, c, d) ((b) ^ (c) ^ (d))
#define FF2(b, c, d) ((b & c) | (b & d) | (c & d))
#define FF3(b, c, d) ((b) ^ (c) ^ (d))
#define KK0 0x5A827999
#define KK1 0x6ED9EBA1
#define KK2 0x8F1BBCDC
#define KK3 0xCA62C1D6
#define GGN(N, a, b, c, d, e, w, t, X, Y)                     \
	for (int i = X; i < Y; i++) {                             \
		t = ROL32(a,  5) + FF##N(b, c, d) + e + KK##N + w[i]; \
		e = d;                                                \
		d = c;                                                \
		c = ROL32(b, 30);                                     \
		b = a;                                                \
		a = t;                                                \
	}
struct SHAInner {
	uint32_t h[5] = {
		0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
	};
	void compress(uint8_t const *src) {
		uint32_t a = h[0];
		uint32_t b = h[1];
		uint32_t c = h[2];
		uint32_t d = h[3];
		uint32_t e = h[4];
		uint32_t w[80], t;
		for (int i =  0; i < 16; i++) {
			uint8_t const *ref = &src[i << 2];
			w[i] = ref[0] << 24 | ref[1] << 16 | ref[2] << 8 | ref[3];
		}
		for (int i = 16; i < 80; i++) {
			t = w[i - 16] ^ w[i - 14] ^ w[i -  8] ^ w[i -  3];
			w[i] = ROL32(t, 1);
		}
		GGN(0, a, b, c, d, e, w, t,  0, 20);
		GGN(1, a, b, c, d, e, w, t, 20, 40);
		GGN(2, a, b, c, d, e, w, t, 40, 60);
		GGN(3, a, b, c, d, e, w, t, 60, 80);
		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
		h[4] += e;
	}
};
class SHA {
	SHAInner inner;
	uint64_t counter = 0;
public:
	static constexpr size_t BLOCK_SIZE = 64;
	static constexpr size_t DIGEST_SIZE = 20;
	void push(uint8_t const *src) {
		inner.compress(src);
		counter += 512;
	}
	void test(uint8_t const *src, size_t len, uint8_t *dst) const {
		SHAInner copy = inner;
		uint8_t tmp[64];
		memcpy(tmp, src, len);
		memset(tmp + len, 0, 64 - len);
		tmp[len] = 0x80;
		if (len >= 56) {
			copy.compress(tmp);
			memset(tmp, 0, 56);
		}
		uint64_t ctrxx = counter + 8 * len;
		uint8_t *ctref = (uint8_t *)&ctrxx;
		tmp[63] = ctref[0];
		tmp[62] = ctref[1];
		tmp[61] = ctref[2];
		tmp[60] = ctref[3];
		tmp[59] = ctref[4];
		tmp[58] = ctref[5];
		tmp[57] = ctref[6];
		tmp[56] = ctref[7];
		copy.compress(tmp);
		for (int i = 0; i < 5; i++) {
			uint8_t *ref = &dst[i << 2];
			ref[0] = copy.h[i] >> 24;
			ref[1] = copy.h[i] >> 16;
			ref[2] = copy.h[i] >>  8;
			ref[3] = copy.h[i]      ;
		}
	}
};
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef KK0
#undef KK1
#undef KK2
#undef KK3
#undef GGN
