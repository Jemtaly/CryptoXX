#pragma once
#include <stdint.h>
#define SCS sizeof(typename SC::sec_t)
template <size_t sec_s>
class StreamCipher {
public:
	using sec_t = uint8_t[sec_s];
	virtual ~StreamCipher() = default;
	virtual void generate(uint8_t *dst) = 0;
};
class StreamCipherFlow {
public:
	virtual ~StreamCipherFlow() = default;
	virtual uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) = 0;
};
template <class SC>
class Crypter: public StreamCipherFlow {
	SC sc;
	size_t use;
	typename SC::sec_t buf;
public:
	template <class... vals_t>
	Crypter(vals_t const &...vals):
		sc(vals...), use(SCS) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		while (SCS + src < end + use) {
			while (use < SCS) {
				*dst++ = *src++ ^ buf[use++];
			}
			sc.generate(buf);
			use -= SCS;
		}
		while (src < end) {
			*dst++ = *src++ ^ buf[use++];
		}
		return dst;
	}
};
#undef SCS
