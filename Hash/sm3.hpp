#include <stdint.h>
#include <string.h>
#include <array>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define P0(x) ((x) ^ ROL32(x, 9) ^ ROL32(x, 17))
#define P1(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FFf(x, y, z) ((x) & (y) | (x) & (z) | (y) & (z))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GGf(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
struct SM3State {
	uint32_t rec[8];
	uint64_t countr;
};
constexpr std::array<uint32_t, 64> K_init(uint32_t const &TT0, uint32_t const &TTf) noexcept {
	std::array<uint32_t, 64> K = {};
	for (int j = 0; j < 16; j++)
		K[j] = TT0 << j % 32 | TT0 >> (32 - j) % 32;
	for (int j = 16; j < 64; j++)
		K[j] = TTf << j % 32 | TTf >> (96 - j) % 32;
	return K;
}
class SM3 : public HashFunction<64, 32, SM3State> {
	static constexpr auto K = K_init(0x79cc4519, 0x7a879d8a);
	static void compress(uint32_t *const &rec, uint8_t const *const &blk) {
		uint32_t W[68];
		for (int j = 0; j < 16; j++)
			W[j] = blk[j << 2] << 030 | blk[j << 2 | 1] << 020 | blk[j << 2 | 2] << 010 | blk[j << 2 | 3];
		for (int j = 16; j < 68; j++) {
			uint32_t TT0 = W[j - 16] ^ W[j - 9] ^ ROL32(W[j - 3], 15);
			W[j] = P1(TT0) ^ W[j - 6] ^ ROL32(W[j - 13], 7);
		}
		uint32_t A = rec[0];
		uint32_t B = rec[1];
		uint32_t C = rec[2];
		uint32_t D = rec[3];
		uint32_t E = rec[4];
		uint32_t F = rec[5];
		uint32_t G = rec[6];
		uint32_t H = rec[7];
		for (int j = 0; j < 16; j++) {
			uint32_t SS0 = ROL32(A, 12) + E + K[j];
			uint32_t SS1 = ROL32(SS0, 7);
			uint32_t SS2 = SS1 ^ ROL32(A, 12);
			uint32_t TT1 = FF0(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
			uint32_t TT2 = GG0(E, F, G) + H + SS1 + W[j];
			D = C;
			C = ROL32(B, 9);
			B = A;
			A = TT1;
			H = G;
			G = ROL32(F, 19);
			F = E;
			E = P0(TT2);
		}
		for (int j = 16; j < 64; j++) {
			uint32_t SS0 = ROL32(A, 12) + E + K[j];
			uint32_t SS1 = ROL32(SS0, 7);
			uint32_t SS2 = SS1 ^ ROL32(A, 12);
			uint32_t TT1 = FFf(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
			uint32_t TT2 = GGf(E, F, G) + H + SS1 + W[j];
			D = C;
			C = ROL32(B, 9);
			B = A;
			A = TT1;
			H = G;
			G = ROL32(F, 19);
			F = E;
			E = P0(TT2);
		}
		rec[0] ^= A;
		rec[1] ^= B;
		rec[2] ^= C;
		rec[3] ^= D;
		rec[4] ^= E;
		rec[5] ^= F;
		rec[6] ^= G;
		rec[7] ^= H;
	}
public:
	void init(sta_t &sta) const {
		sta.rec[0] = 0x7380166F;
		sta.rec[1] = 0x4914B2B9;
		sta.rec[2] = 0x172442D7;
		sta.rec[3] = 0xDA8A0600;
		sta.rec[4] = 0xA96F30BC;
		sta.rec[5] = 0x163138AA;
		sta.rec[6] = 0xE38DEE4D;
		sta.rec[7] = 0xB0FB0E4E;
		sta.countr = 0;
	}
	void push(sta_t &sta, uint8_t const *const &blk) const {
		compress(sta.rec, blk);
		sta.countr += 64;
	}
	void cast(sta_t const &sta, uint8_t *const &buf, uint8_t const *const &fin, size_t const &len) const {
		uint8_t blk[64];
		memcpy(blk, fin, len);
		memset(blk + len, 0, 64 - len);
		blk[len] = 0x80;
		uint32_t rectmp[8];
		memcpy(rectmp, sta.rec, 32);
		uint64_t ctrtmp = sta.countr + 8 * len;
		uint8_t *u8ctmp = (uint8_t *)&ctrtmp;
		if (len >= 56) {
			compress(rectmp, blk);
			memset(blk, 0, 56);
			blk[63] = u8ctmp[0];
			blk[62] = u8ctmp[1];
			blk[61] = u8ctmp[2];
			blk[60] = u8ctmp[3];
			blk[59] = u8ctmp[4];
			blk[58] = u8ctmp[5];
			blk[57] = u8ctmp[6];
			blk[56] = u8ctmp[7];
			compress(rectmp, blk);
		} else {
			blk[63] = u8ctmp[0];
			blk[62] = u8ctmp[1];
			blk[61] = u8ctmp[2];
			blk[60] = u8ctmp[3];
			blk[59] = u8ctmp[4];
			blk[58] = u8ctmp[5];
			blk[57] = u8ctmp[6];
			blk[56] = u8ctmp[7];
			compress(rectmp, blk);
		}
		for (int j = 0; j < 8; j++) {
			buf[j << 2] = rectmp[j] >> 030;
			buf[j << 2 | 1] = rectmp[j] >> 020;
			buf[j << 2 | 2] = rectmp[j] >> 010;
			buf[j << 2 | 3] = rectmp[j];
		}
	}
};