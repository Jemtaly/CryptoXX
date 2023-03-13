#pragma once
#include <stdint.h>
#define SEC sizeof(typename StreamCipher::sec_t)
template <size_t sec_s>
class StreamCipherBase {
public:
	using sec_t = uint8_t[sec_s];
	virtual ~StreamCipherBase() = default;
	virtual void generate(uint8_t *dst) = 0;
};
class StreamCipherCrypterBase {
public:
	virtual ~StreamCipherCrypterBase() = default;
	virtual uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) = 0;
};
template <class StreamCipher>
class StreamCipherCrypter: public StreamCipherCrypterBase {
	StreamCipher sc;
	size_t use;
	typename StreamCipher::sec_t buf;
public:
	template <class... vals_t>
	StreamCipherCrypter(vals_t const &...vals):
		sc(vals...), use(SEC) {}
	uint8_t *update(uint8_t *dst, uint8_t const *src, uint8_t const *end) {
		while (SEC + src < end + use) {
			while (use < SEC) {
				*dst++ = *src++ ^ buf[use++];
			}
			sc.generate(buf);
			use -= SEC;
		}
		while (src < end) {
			*dst++ = *src++ ^ buf[use++];
		}
		return dst;
	}
};
#undef SEC
