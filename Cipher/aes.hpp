#pragma once
#include <array>
#include "block.hpp"
constexpr auto M_boxes_init(uint32_t const &n) {
	std::array<uint32_t, 256> coef_mult_n = {};
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 4; i++) {
			uint8_t a = n >> 8 * i, b = j, p = 0;
			for (uint8_t k = 0; k < 8; k++) {
				p = p ^ (b & 1) * a;
				a = a << 1 ^ (a >> 7) * 0x1b;
				b = b >> 1;
			}
			coef_mult_n[j] |= p << 8 * i;
		}
	}
	return coef_mult_n;
}
template <int rn>
class AES: public BlockCipher<16> {
protected:
	static constexpr uint8_t S_box[256] = {
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
	};
	static constexpr uint8_t R_box[256] = {
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
	};
	static constexpr uint8_t Rcon[16] = {
		0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	};
	static constexpr auto E_boxes_0 = M_boxes_init(0x03010102);
	static constexpr auto E_boxes_1 = M_boxes_init(0x01010203);
	static constexpr auto E_boxes_2 = M_boxes_init(0x01020301);
	static constexpr auto E_boxes_3 = M_boxes_init(0x02030101);
	static constexpr auto D_boxes_0 = M_boxes_init(0x0b0d090e);
	static constexpr auto D_boxes_1 = M_boxes_init(0x0d090e0b);
	static constexpr auto D_boxes_2 = M_boxes_init(0x090e0b0d);
	static constexpr auto D_boxes_3 = M_boxes_init(0x0e0b0d09);
	static void mix_columns_enc(uint8_t *const &state) {
		((uint32_t *)state)[0] = E_boxes_0[state[0x0]] ^ E_boxes_1[state[0x1]] ^ E_boxes_2[state[0x2]] ^ E_boxes_3[state[0x3]];
		((uint32_t *)state)[1] = E_boxes_0[state[0x4]] ^ E_boxes_1[state[0x5]] ^ E_boxes_2[state[0x6]] ^ E_boxes_3[state[0x7]];
		((uint32_t *)state)[2] = E_boxes_0[state[0x8]] ^ E_boxes_1[state[0x9]] ^ E_boxes_2[state[0xa]] ^ E_boxes_3[state[0xb]];
		((uint32_t *)state)[3] = E_boxes_0[state[0xc]] ^ E_boxes_1[state[0xd]] ^ E_boxes_2[state[0xe]] ^ E_boxes_3[state[0xf]];
	}
	static void mix_columns_dec(uint8_t *const &state) {
		((uint32_t *)state)[0] = D_boxes_0[state[0x0]] ^ D_boxes_1[state[0x1]] ^ D_boxes_2[state[0x2]] ^ D_boxes_3[state[0x3]];
		((uint32_t *)state)[1] = D_boxes_0[state[0x4]] ^ D_boxes_1[state[0x5]] ^ D_boxes_2[state[0x6]] ^ D_boxes_3[state[0x7]];
		((uint32_t *)state)[2] = D_boxes_0[state[0x8]] ^ D_boxes_1[state[0x9]] ^ D_boxes_2[state[0xa]] ^ D_boxes_3[state[0xb]];
		((uint32_t *)state)[3] = D_boxes_0[state[0xc]] ^ D_boxes_1[state[0xd]] ^ D_boxes_2[state[0xe]] ^ D_boxes_3[state[0xf]];
	}
	static void shift_rows_enc(uint8_t *const &state) {
		uint8_t temp_value;
		temp_value = state[0x1];
		state[0x1] = state[0x5];
		state[0x5] = state[0x9];
		state[0x9] = state[0xd];
		state[0xd] = temp_value;
		temp_value = state[0x2];
		state[0x2] = state[0xa];
		state[0xa] = temp_value;
		temp_value = state[0x6];
		state[0x6] = state[0xe];
		state[0xe] = temp_value;
		temp_value = state[0xf];
		state[0xf] = state[0xb];
		state[0xb] = state[0x7];
		state[0x7] = state[0x3];
		state[0x3] = temp_value;
	}
	static void shift_rows_dec(uint8_t *const &state) {
		uint8_t temp_value;
		temp_value = state[0xd];
		state[0xd] = state[0x9];
		state[0x9] = state[0x5];
		state[0x5] = state[0x1];
		state[0x1] = temp_value;
		temp_value = state[0x2];
		state[0x2] = state[0xa];
		state[0xa] = temp_value;
		temp_value = state[0x6];
		state[0x6] = state[0xe];
		state[0xe] = temp_value;
		temp_value = state[0x3];
		state[0x3] = state[0x7];
		state[0x7] = state[0xb];
		state[0xb] = state[0xf];
		state[0xf] = temp_value;
	}
	static void sub_bytes_enc(uint8_t *const &state) {
		state[0x0] = S_box[state[0x0]];
		state[0x1] = S_box[state[0x1]];
		state[0x2] = S_box[state[0x2]];
		state[0x3] = S_box[state[0x3]];
		state[0x4] = S_box[state[0x4]];
		state[0x5] = S_box[state[0x5]];
		state[0x6] = S_box[state[0x6]];
		state[0x7] = S_box[state[0x7]];
		state[0x8] = S_box[state[0x8]];
		state[0x9] = S_box[state[0x9]];
		state[0xa] = S_box[state[0xa]];
		state[0xb] = S_box[state[0xb]];
		state[0xc] = S_box[state[0xc]];
		state[0xd] = S_box[state[0xd]];
		state[0xe] = S_box[state[0xe]];
		state[0xf] = S_box[state[0xf]];
	}
	static void sub_bytes_dec(uint8_t *const &state) {
		state[0x0] = R_box[state[0x0]];
		state[0x1] = R_box[state[0x1]];
		state[0x2] = R_box[state[0x2]];
		state[0x3] = R_box[state[0x3]];
		state[0x4] = R_box[state[0x4]];
		state[0x5] = R_box[state[0x5]];
		state[0x6] = R_box[state[0x6]];
		state[0x7] = R_box[state[0x7]];
		state[0x8] = R_box[state[0x8]];
		state[0x9] = R_box[state[0x9]];
		state[0xa] = R_box[state[0xa]];
		state[0xb] = R_box[state[0xb]];
		state[0xc] = R_box[state[0xc]];
		state[0xd] = R_box[state[0xd]];
		state[0xe] = R_box[state[0xe]];
		state[0xf] = R_box[state[0xf]];
	}
	static void add_round_key(uint8_t *const &state, uint32_t const (&k)[4]) {
		((uint32_t *)state)[0] ^= k[0];
		((uint32_t *)state)[1] ^= k[1];
		((uint32_t *)state)[2] ^= k[2];
		((uint32_t *)state)[3] ^= k[3];
	}
	uint32_t rk[rn + 1][4];
	AES() = default;
public:
	void encrypt(uint8_t const *const &src, uint8_t *const &dst) const {
		((uint32_t *)dst)[0] = ((uint32_t *)src)[0];
		((uint32_t *)dst)[1] = ((uint32_t *)src)[1];
		((uint32_t *)dst)[2] = ((uint32_t *)src)[2];
		((uint32_t *)dst)[3] = ((uint32_t *)src)[3];
		int round = 0;
		add_round_key(dst, rk[round]);
		sub_bytes_enc(dst);
		shift_rows_enc(dst);
		while (++round < rn) {
			mix_columns_enc(dst);
			add_round_key(dst, rk[round]);
			sub_bytes_enc(dst);
			shift_rows_enc(dst);
		}
		add_round_key(dst, rk[round]);
	}
	void decrypt(uint8_t const *const &src, uint8_t *const &dst) const {
		((uint32_t *)dst)[0] = ((uint32_t *)src)[0];
		((uint32_t *)dst)[1] = ((uint32_t *)src)[1];
		((uint32_t *)dst)[2] = ((uint32_t *)src)[2];
		((uint32_t *)dst)[3] = ((uint32_t *)src)[3];
		int round = rn;
		add_round_key(dst, rk[round]);
		while (--round > 0) {
			shift_rows_dec(dst);
			sub_bytes_dec(dst);
			add_round_key(dst, rk[round]);
			mix_columns_dec(dst);
		}
		shift_rows_dec(dst);
		sub_bytes_dec(dst);
		add_round_key(dst, rk[round]);
	}
};
class AES128: public AES<10> {
public:
	AES128(uint8_t const *const &mk) {
		memcpy(rk, mk, 16);
		for (int i = 4; i < 44; ++i) {
			uint32_t a = (*rk)[i - 1];
			if (i % 4 == 0) {
				a = S_box[a >> 020 & 0xff] << 010 | S_box[a >> 030] << 020 | S_box[a & 0xff] << 030 | S_box[a >> 010 & 0xff] ^ Rcon[i / 4];
			}
			(*rk)[i] = (*rk)[i - 4] ^ a;
		}
	}
};
class AES192: public AES<12> {
public:
	AES192(uint8_t const *const &mk) {
		memcpy(rk, mk, 24);
		for (int i = 6; i < 52; ++i) {
			uint32_t a = (*rk)[i - 1];
			if (i % 6 == 0) {
				a = S_box[a >> 020 & 0xff] << 010 | S_box[a >> 030] << 020 | S_box[a & 0xff] << 030 | S_box[a >> 010 & 0xff] ^ Rcon[i / 6];
			}
			(*rk)[i] = (*rk)[i - 6] ^ a;
		}
	}
};
class AES256: public AES<14> {
public:
	AES256(uint8_t const *const &mk) {
		memcpy(rk, mk, 32);
		for (int i = 8; i < 60; ++i) {
			uint32_t a = (*rk)[i - 1];
			if (i % 8 == 0) {
				a = S_box[a >> 020 & 0xff] << 010 | S_box[a >> 030] << 020 | S_box[a & 0xff] << 030 | S_box[a >> 010 & 0xff] ^ Rcon[i / 8];
			} else if (i % 8 == 4) {
				a = S_box[a & 0xff] | S_box[a >> 010 & 0xff] << 010 | S_box[a >> 020 & 0xff] << 020 | S_box[a >> 030] << 030;
			}
			(*rk)[i] = (*rk)[i - 8] ^ a;
		}
	}
};
