#pragma once
#include <stdint.h>
#include <string.h>
#include <utility> // std::forward
#define SEC sizeof(typename StreamCipher::sec_t)
template <size_t sec_s>
class StreamCipherInterface {
public:
	using sec_t = uint8_t[sec_s];
	virtual ~StreamCipherInterface() = default;
	virtual void generate(uint8_t *dst) = 0;
};
template <class StreamCipher>
class StreamCipherCrypter {
	StreamCipher sc;
	size_t use;
	typename StreamCipher::sec_t buf;
public:
	template <class... vals_t>
	StreamCipherCrypter(vals_t &&...vals):
		sc(std::forward<vals_t>(vals)...), use(SEC) {}
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
