#pragma once
#include <stdint.h>
class StreamCipher {
public:
	virtual ~StreamCipher() = default;
	virtual void crypt(uint8_t const *const &src, uint8_t *const &dst) = 0;
};
class StreamCipherFlow {
public:
	virtual ~StreamCipherFlow() = default;
	virtual uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) = 0;
};
template <class SC>
class Crypter: public StreamCipherFlow {
	SC sc;
public:
	using sc_t = SC;
	template <class... vals_t>
	Crypter(vals_t const &...vals):
		sc(vals...) {}
	uint8_t *update(uint8_t const *src, uint8_t const *const &end, uint8_t *dst) {
		while (src < end) {
			sc.crypt(src++, dst++);
		}
		return dst;
	}
};
