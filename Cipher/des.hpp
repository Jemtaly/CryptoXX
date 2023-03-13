#pragma once
#include "block.hpp"
class DES: public BlockCipherInterface<8> {
	static constexpr uint8_t E[48] = {
		0x00, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1c, 0x1b,
		0x1a, 0x19, 0x18, 0x17, 0x18, 0x17, 0x16, 0x15,
		0x14, 0x13, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f,
		0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0c, 0x0b,
		0x0a, 0x09, 0x08, 0x07, 0x08, 0x07, 0x06, 0x05,
		0x04, 0x03, 0x04, 0x03, 0x02, 0x01, 0x00, 0x1f,
	};
	static constexpr uint8_t IP[64] = {
		0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e,
		0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x34, 0x3c,
		0x02, 0x0a, 0x12, 0x1a, 0x22, 0x2a, 0x32, 0x3a,
		0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
		0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f,
		0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x35, 0x3d,
		0x03, 0x0b, 0x13, 0x1b, 0x23, 0x2b, 0x33, 0x3b,
		0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x39,
	};
	static constexpr uint8_t FP[64] = {
		0x18, 0x38, 0x10, 0x30, 0x08, 0x28, 0x00, 0x20,
		0x19, 0x39, 0x11, 0x31, 0x09, 0x29, 0x01, 0x21,
		0x1a, 0x3a, 0x12, 0x32, 0x0a, 0x2a, 0x02, 0x22,
		0x1b, 0x3b, 0x13, 0x33, 0x0b, 0x2b, 0x03, 0x23,
		0x1c, 0x3c, 0x14, 0x34, 0x0c, 0x2c, 0x04, 0x24,
		0x1d, 0x3d, 0x15, 0x35, 0x0d, 0x2d, 0x05, 0x25,
		0x1e, 0x3e, 0x16, 0x36, 0x0e, 0x2e, 0x06, 0x26,
		0x1f, 0x3f, 0x17, 0x37, 0x0f, 0x2f, 0x07, 0x27,
	};
	static constexpr uint8_t PC_1[56] = {
		0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f,
		0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e,
		0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x35, 0x3d,
		0x04, 0x0c, 0x14, 0x1c, 0x01, 0x09, 0x11, 0x19,
		0x21, 0x29, 0x31, 0x39, 0x02, 0x0a, 0x12, 0x1a,
		0x22, 0x2a, 0x32, 0x3a, 0x03, 0x0b, 0x13, 0x1b,
		0x23, 0x2b, 0x33, 0x3b, 0x24, 0x2c, 0x34, 0x3c,
	};
	static constexpr uint8_t PC_2[48] = {
		0x2a, 0x27, 0x2d, 0x20, 0x37, 0x33, 0x35, 0x1c,
		0x29, 0x32, 0x23, 0x2e, 0x21, 0x25, 0x2c, 0x34,
		0x1e, 0x30, 0x28, 0x31, 0x1d, 0x24, 0x2b, 0x36,
		0x0f, 0x04, 0x19, 0x13, 0x09, 0x01, 0x1a, 0x10,
		0x05, 0x0b, 0x17, 0x08, 0x0c, 0x07, 0x11, 0x00,
		0x16, 0x03, 0x0a, 0x0e, 0x06, 0x14, 0x1b, 0x18,
	};
	static constexpr uint8_t P[32] = {
		0x10, 0x19, 0x0c, 0x0b, 0x03, 0x14, 0x04, 0x0f,
		0x1f, 0x11, 0x09, 0x06, 0x1b, 0x0e, 0x01, 0x16,
		0x1e, 0x18, 0x08, 0x12, 0x00, 0x05, 0x1d, 0x17,
		0x0d, 0x13, 0x02, 0x1a, 0x0a, 0x15, 0x1c, 0x07,
	};
	static constexpr uint8_t S_boxes_0[64] = {
		0xe, 0x0, 0x4, 0xf, 0xd, 0x7, 0x1, 0x4, 0x2, 0xe, 0xf, 0x2, 0xb, 0xd, 0x8, 0x1,
		0x3, 0xa, 0xa, 0x6, 0x6, 0xc, 0xc, 0xb, 0x5, 0x9, 0x9, 0x5, 0x0, 0x3, 0x7, 0x8,
		0x4, 0xf, 0x1, 0xc, 0xe, 0x8, 0x8, 0x2, 0xd, 0x4, 0x6, 0x9, 0x2, 0x1, 0xb, 0x7,
		0xf, 0x5, 0xc, 0xb, 0x9, 0x3, 0x7, 0xe, 0x3, 0xa, 0xa, 0x0, 0x5, 0x6, 0x0, 0xd,
	};
	static constexpr uint8_t S_boxes_1[64] = {
		0xf, 0x3, 0x1, 0xd, 0x8, 0x4, 0xe, 0x7, 0x6, 0xf, 0xb, 0x2, 0x3, 0x8, 0x4, 0xe,
		0x9, 0xc, 0x7, 0x0, 0x2, 0x1, 0xd, 0xa, 0xc, 0x6, 0x0, 0x9, 0x5, 0xb, 0xa, 0x5,
		0x0, 0xd, 0xe, 0x8, 0x7, 0xa, 0xb, 0x1, 0xa, 0x3, 0x4, 0xf, 0xd, 0x4, 0x1, 0x2,
		0x5, 0xb, 0x8, 0x6, 0xc, 0x7, 0x6, 0xc, 0x9, 0x0, 0x3, 0x5, 0x2, 0xe, 0xf, 0x9,
	};
	static constexpr uint8_t S_boxes_2[64] = {
		0xa, 0xd, 0x0, 0x7, 0x9, 0x0, 0xe, 0x9, 0x6, 0x3, 0x3, 0x4, 0xf, 0x6, 0x5, 0xa,
		0x1, 0x2, 0xd, 0x8, 0xc, 0x5, 0x7, 0xe, 0xb, 0xc, 0x4, 0xb, 0x2, 0xf, 0x8, 0x1,
		0xd, 0x1, 0x6, 0xa, 0x4, 0xd, 0x9, 0x0, 0x8, 0x6, 0xf, 0x9, 0x3, 0x8, 0x0, 0x7,
		0xb, 0x4, 0x1, 0xf, 0x2, 0xe, 0xc, 0x3, 0x5, 0xb, 0xa, 0x5, 0xe, 0x2, 0x7, 0xc,
	};
	static constexpr uint8_t S_boxes_3[64] = {
		0x7, 0xd, 0xd, 0x8, 0xe, 0xb, 0x3, 0x5, 0x0, 0x6, 0x6, 0xf, 0x9, 0x0, 0xa, 0x3,
		0x1, 0x4, 0x2, 0x7, 0x8, 0x2, 0x5, 0xc, 0xb, 0x1, 0xc, 0xa, 0x4, 0xe, 0xf, 0x9,
		0xa, 0x3, 0x6, 0xf, 0x9, 0x0, 0x0, 0x6, 0xc, 0xa, 0xb, 0x1, 0x7, 0xd, 0xd, 0x8,
		0xf, 0x9, 0x1, 0x4, 0x3, 0x5, 0xe, 0xb, 0x5, 0xc, 0x2, 0x7, 0x8, 0x2, 0x4, 0xe,
	};
	static constexpr uint8_t S_boxes_4[64] = {
		0x2, 0xe, 0xc, 0xb, 0x4, 0x2, 0x1, 0xc, 0x7, 0x4, 0xa, 0x7, 0xb, 0xd, 0x6, 0x1,
		0x8, 0x5, 0x5, 0x0, 0x3, 0xf, 0xf, 0xa, 0xd, 0x3, 0x0, 0x9, 0xe, 0x8, 0x9, 0x6,
		0x4, 0xb, 0x2, 0x8, 0x1, 0xc, 0xb, 0x7, 0xa, 0x1, 0xd, 0xe, 0x7, 0x2, 0x8, 0xd,
		0xf, 0x6, 0x9, 0xf, 0xc, 0x0, 0x5, 0x9, 0x6, 0xa, 0x3, 0x4, 0x0, 0x5, 0xe, 0x3,
	};
	static constexpr uint8_t S_boxes_5[64] = {
		0xc, 0xa, 0x1, 0xf, 0xa, 0x4, 0xf, 0x2, 0x9, 0x7, 0x2, 0xc, 0x6, 0x9, 0x8, 0x5,
		0x0, 0x6, 0xd, 0x1, 0x3, 0xd, 0x4, 0xe, 0xe, 0x0, 0x7, 0xb, 0x5, 0x3, 0xb, 0x8,
		0x9, 0x4, 0xe, 0x3, 0xf, 0x2, 0x5, 0xc, 0x2, 0x9, 0x8, 0x5, 0xc, 0xf, 0x3, 0xa,
		0x7, 0xb, 0x0, 0xe, 0x4, 0x1, 0xa, 0x7, 0x1, 0x6, 0xd, 0x0, 0xb, 0x8, 0x6, 0xd,
	};
	static constexpr uint8_t S_boxes_6[64] = {
		0x4, 0xd, 0xb, 0x0, 0x2, 0xb, 0xe, 0x7, 0xf, 0x4, 0x0, 0x9, 0x8, 0x1, 0xd, 0xa,
		0x3, 0xe, 0xc, 0x3, 0x9, 0x5, 0x7, 0xc, 0x5, 0x2, 0xa, 0xf, 0x6, 0x8, 0x1, 0x6,
		0x1, 0x6, 0x4, 0xb, 0xb, 0xd, 0xd, 0x8, 0xc, 0x1, 0x3, 0x4, 0x7, 0xa, 0xe, 0x7,
		0xa, 0x9, 0xf, 0x5, 0x6, 0x0, 0x8, 0xf, 0x0, 0xe, 0x5, 0x2, 0x9, 0x3, 0x2, 0xc,
	};
	static constexpr uint8_t S_boxes_7[64] = {
		0xd, 0x1, 0x2, 0xf, 0x8, 0xd, 0x4, 0x8, 0x6, 0xa, 0xf, 0x3, 0xb, 0x7, 0x1, 0x4,
		0xa, 0xc, 0x9, 0x5, 0x3, 0x6, 0xe, 0xb, 0x5, 0x0, 0x0, 0xe, 0xc, 0x9, 0x7, 0x2,
		0x7, 0x2, 0xb, 0x1, 0x4, 0xe, 0x1, 0x7, 0x9, 0x4, 0xc, 0xa, 0xe, 0x8, 0x2, 0xd,
		0x0, 0xf, 0x6, 0xc, 0xa, 0x9, 0xd, 0x0, 0xf, 0x3, 0x3, 0x5, 0x5, 0x6, 0x8, 0xb,
	};
	static constexpr uint8_t shift[16] = {
		1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1,
	};
	template <int dout>
	static uint64_t permutation(uint64_t vin, uint8_t const (&A)[dout]) {
		uint64_t vout = 0;
		for (int i = 0; i < dout; i += 8) {
			vout = vout << 8 |
				(vin >> A[i    ] & 1) << 7 |
				(vin >> A[i | 1] & 1) << 6 |
				(vin >> A[i | 2] & 1) << 5 |
				(vin >> A[i | 3] & 1) << 4 |
				(vin >> A[i | 4] & 1) << 3 |
				(vin >> A[i | 5] & 1) << 2 |
				(vin >> A[i | 6] & 1) << 1 |
				(vin >> A[i | 7] & 1)     ;
		}
		return vout;
	}
	static uint64_t F(uint64_t r, uint64_t k) {
		uint64_t x = permutation(r, E) ^ k;
		uint64_t f =
			S_boxes_0[x >> 42       ] << 28 |
			S_boxes_1[x >> 36 & 0x3f] << 24 |
			S_boxes_2[x >> 30 & 0x3f] << 20 |
			S_boxes_3[x >> 24 & 0x3f] << 16 |
			S_boxes_4[x >> 18 & 0x3f] << 12 |
			S_boxes_5[x >> 12 & 0x3f] <<  8 |
			S_boxes_6[x >>  6 & 0x3f] <<  4 |
			S_boxes_7[x       & 0x3f]      ;
		return permutation(f, P);
	}
	uint64_t rk[16];
public:
	DES(uint8_t const *mk) {
		uint64_t t = permutation(*(uint64_t *)mk, PC_1);
		uint64_t l = t >> 28, r = t & 0xfffffff;
		for (int i = 0; i < 16; i++) {
			l = l << shift[i] & 0xfffffff | l >> (28 - shift[i]);
			r = r << shift[i] & 0xfffffff | r >> (28 - shift[i]);
			rk[i] = permutation(l << 28 | r, PC_2);
		}
	}
	void encrypt(uint8_t const *src, uint8_t *dst) const {
		uint64_t t = permutation(*(uint64_t *)src, IP);
		uint64_t l = t >> 32, r = t & 0xffffffff;
		for (int i = 0; i < 16; i++) {
			t = l;
			l = r;
			r = F(l, rk[i]) ^ t;
		}
		*(uint64_t *)dst = permutation(r << 32 | l, FP);
	}
	void decrypt(uint8_t const *src, uint8_t *dst) const {
		uint64_t t = permutation(*(uint64_t *)src, IP);
		uint64_t l = t >> 32, r = t & 0xffffffff;
		for (int i = 0; i < 16; i++) {
			t = l;
			l = r;
			r = F(l, rk[15 - i]) ^ t;
		}
		*(uint64_t *)dst = permutation(r << 32 | l, FP);
	}
};
