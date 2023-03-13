#pragma once
#include "block.hpp"
#include "stream.hpp"
#define BLK sizeof(typename BlockCipher::blk_t)
template <class BlockCipher>
class CTRMode: public StreamCipherInterface<BLK> {
	BlockCipher bc;
	typename BlockCipher::blk_t ctr;
public:
	template <class... vals_t>
	CTRMode(uint8_t const *civ, vals_t &&...vals):
		bc(std::forward<vals_t>(vals)...) {
		memcpy(ctr, civ, BLK);
	}
	void generate(uint8_t *dst) {
		bc.encrypt(ctr, dst);
		for (size_t i = BLK - 1; i < BLK && ++ctr[i] == 0; i--) {}
	}
};
#undef BLK
