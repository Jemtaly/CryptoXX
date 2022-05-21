#include <stdint.h>
#include <string.h>
#include <array>
#include "hash.hpp"
#define ROL32(x, n) ((x) << (n) | (x) >> (32 - (n)))
#define P0(x) ((x) ^ ROL32(x, 9) ^ ROL32(x, 17))
#define P1(x) ((x) ^ ROL32(x, 15) ^ ROL32(x, 23))
#define FF00(x, y, z) ((x) ^ (y) ^ (z))
#define FF10(x, y, z) ((x) & (y) | (x) & (z) | (y) & (z))
#define GG00(x, y, z) ((x) ^ (y) ^ (z))
#define GG10(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
struct SM3State {
	uint32_t rec[8];
	uint64_t countr;
};
class SM3 : public HashFunction<64, 32, SM3State> {
	static constexpr auto K = [](uint32_t const &TT00, uint32_t const &TT10) {
		std::array<uint32_t, 64> K = {};
		for (int j = 0; j < 16; j++)
			K[j] = TT00 << j % 32 | TT00 >> (32 - j) % 32;
		for (int j = 16; j < 64; j++)
			K[j] = TT10 << j % 32 | TT10 >> (96 - j) % 32;
		return K;
	}(0x79cc4519, 0x7a879d8a);
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
			uint32_t TT1 = FF00(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
			uint32_t TT2 = GG00(E, F, G) + H + SS1 + W[j];
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
			uint32_t TT1 = FF10(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
			uint32_t TT2 = GG10(E, F, G) + H + SS1 + W[j];
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
	void init(SM3State &sta) const {
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
	void push(SM3State &sta, uint8_t const *const &blk) const {
		compress(sta.rec, blk);
		sta.countr += 64;
	}
	void cast(SM3State const &sta, uint8_t *const &buf, uint8_t const *const &fin, size_t const &len) const {
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
