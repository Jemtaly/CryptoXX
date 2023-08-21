#pragma once
#include <array>
#include <bit>
#include <utility> // std::forward
#include <stdint.h>
#include <string.h>
#include <concepts>
#include <stdexcept>
// bits_t is used to represent the number of bits to rotate
using bits_t = uint8_t;
template <std::unsigned_integral T>
constexpr T ROTL(T const x, bits_t const n) {
    return (x << (n & (sizeof(T) * 8 - 1)) | (x) >> (-n & (sizeof(T) * 8 - 1)));
}
template <std::unsigned_integral T>
constexpr T ROTR(T const x, bits_t const n) {
    return (x >> (n & (sizeof(T) * 8 - 1)) | (x) << (-n & (sizeof(T) * 8 - 1)));
}
template <std::endian E>
struct EndianUtils {
    template <std::unsigned_integral T>
    static constexpr T GET(uint8_t const *arr);
    template <std::unsigned_integral T>
    static constexpr void PUT(uint8_t *arr, T w);
    template <std::unsigned_integral T>
    static constexpr void READ(T *a, uint8_t const *arr, int n);
    template <std::unsigned_integral T>
    static constexpr void WRITE(uint8_t *arr, T const *a, int n);
    template <std::unsigned_integral T>
    static constexpr uint8_t &BYTE(T *a, int i);
    template <std::unsigned_integral T>
    static constexpr uint8_t const &BYTE(T const *a, int i);
    template <std::unsigned_integral T>
    static constexpr void READB(T *a, uint8_t const *arr, int n);
    template <std::unsigned_integral T>
    static constexpr void WRITEB(uint8_t *arr, T const *a, int n);
    template <std::unsigned_integral T>
    static constexpr void XORB(T *a, uint8_t const *arr, int n);
    template <std::unsigned_integral T>
    static constexpr void XORB(uint8_t *arr, T const *a, int n);
};
template <> template <>
constexpr uint16_t EndianUtils<std::endian::little>::GET(uint8_t const *arr) {
    return
        (uint16_t)(arr)[0]       | (uint16_t)(arr)[1] <<  8;
}
template <> template <>
constexpr uint32_t EndianUtils<std::endian::little>::GET(uint8_t const *arr) {
    return
        (uint32_t)(arr)[0]       | (uint32_t)(arr)[1] <<  8 |
        (uint32_t)(arr)[2] << 16 | (uint32_t)(arr)[3] << 24;
}
template <> template <>
constexpr uint64_t EndianUtils<std::endian::little>::GET(uint8_t const *arr) {
    return
        (uint64_t)(arr)[0]       | (uint64_t)(arr)[1] <<  8 |
        (uint64_t)(arr)[2] << 16 | (uint64_t)(arr)[3] << 24 |
        (uint64_t)(arr)[4] << 32 | (uint64_t)(arr)[5] << 40 |
        (uint64_t)(arr)[6] << 48 | (uint64_t)(arr)[7] << 56;
}
template <> template <>
constexpr uint16_t EndianUtils<std::endian::big>::GET(uint8_t const *arr) {
    return
        (uint16_t)(arr)[0] <<  8 | (uint16_t)(arr)[1]      ;
}
template <> template <>
constexpr uint32_t EndianUtils<std::endian::big>::GET(uint8_t const *arr) {
    return
        (uint32_t)(arr)[0] << 24 | (uint32_t)(arr)[1] << 16 |
        (uint32_t)(arr)[2] <<  8 | (uint32_t)(arr)[3]      ;
}
template <> template <>
constexpr uint64_t EndianUtils<std::endian::big>::GET(uint8_t const *arr) {
    return
        (uint64_t)(arr)[0] << 56 | (uint64_t)(arr)[1] << 48 |
        (uint64_t)(arr)[2] << 40 | (uint64_t)(arr)[3] << 32 |
        (uint64_t)(arr)[4] << 24 | (uint64_t)(arr)[5] << 16 |
        (uint64_t)(arr)[6] <<  8 | (uint64_t)(arr)[7]      ;
}
template <> template <>
constexpr void EndianUtils<std::endian::little>::PUT(uint8_t *arr, uint16_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8       ;
}
template <> template <>
constexpr void EndianUtils<std::endian::little>::PUT(uint8_t *arr, uint32_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8 & 0xff;
    arr[2] = w >> 16 & 0xff;
    arr[3] = w >> 24       ;
}
template <> template <>
constexpr void EndianUtils<std::endian::little>::PUT(uint8_t *arr, uint64_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8 & 0xff;
    arr[2] = w >> 16 & 0xff;
    arr[3] = w >> 24 & 0xff;
    arr[4] = w >> 32 & 0xff;
    arr[5] = w >> 40 & 0xff;
    arr[6] = w >> 48 & 0xff;
    arr[7] = w >> 56       ;
}
template <> template <>
constexpr void EndianUtils<std::endian::big>::PUT(uint8_t *arr, uint16_t w) {
    arr[0] = w >>  8       ;
    arr[1] = w       & 0xff;
}
template <> template <>
constexpr void EndianUtils<std::endian::big>::PUT(uint8_t *arr, uint32_t w) {
    arr[0] = w >> 24       ;
    arr[1] = w >> 16 & 0xff;
    arr[2] = w >>  8 & 0xff;
    arr[3] = w       & 0xff;
}
template <> template <>
constexpr void EndianUtils<std::endian::big>::PUT(uint8_t *arr, uint64_t w) {
    arr[0] = w >> 56       ;
    arr[1] = w >> 48 & 0xff;
    arr[2] = w >> 40 & 0xff;
    arr[3] = w >> 32 & 0xff;
    arr[4] = w >> 24 & 0xff;
    arr[5] = w >> 16 & 0xff;
    arr[6] = w >>  8 & 0xff;
    arr[7] = w       & 0xff;
}
template <> template <std::unsigned_integral T>
constexpr void EndianUtils<std::endian::native>::READ(T *a, uint8_t const *arr, int n) {
    memcpy(a, arr, sizeof(T) * n);
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::READ(T *a, uint8_t const *arr, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = EndianUtils<E>::GET<T>(arr + sizeof(T) * i);
    }
}
template <> template <std::unsigned_integral T>
constexpr void EndianUtils<std::endian::native>::WRITE(uint8_t *arr, T const *a, int n) {
    memcpy(arr, a, sizeof(T) * n);
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::WRITE(uint8_t *arr, T const *a, int n) {
    for (int i = 0; i < n; i++) {
        EndianUtils<E>::PUT(arr + sizeof(T) * i, a[i]);
    }
}
template <> template <std::unsigned_integral T>
constexpr uint8_t &EndianUtils<std::endian::native>::BYTE(T *a, int i) {
    return ((uint8_t *)a)[i];
}
template <std::endian E> template <std::unsigned_integral T>
constexpr uint8_t &EndianUtils<E>::BYTE(T *a, int i) {
    return ((uint8_t (*)[sizeof(T)])a)[i / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)];
}
template <> template <std::unsigned_integral T>
constexpr uint8_t const &EndianUtils<std::endian::native>::BYTE(T const *a, int i) {
    return ((uint8_t *)a)[i];
}
template <std::endian E> template <std::unsigned_integral T>
constexpr uint8_t const &EndianUtils<E>::BYTE(T const *a, int i) {
    return ((uint8_t (*)[sizeof(T)])a)[i / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)];
}
template <> template <std::unsigned_integral T>
constexpr void EndianUtils<std::endian::native>::READB(T *a, uint8_t const *arr, int n) {
    memcpy(a, arr, n);
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::READB(T *a, uint8_t const *arr, int n) {
    EndianUtils<E>::READ(a, arr, n / sizeof(T));
    for (int i = n / sizeof(T) * sizeof(T); i < n; i++) {
        ((uint8_t (*)[sizeof(T)])a)[n / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)] = arr[i];
    }
}
template <> template <std::unsigned_integral T>
constexpr void EndianUtils<std::endian::native>::WRITEB(uint8_t *arr, T const *a, int n) {
    memcpy(arr, a, n);
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::WRITEB(uint8_t *arr, T const *a, int n) {
    EndianUtils<E>::WRITE(arr, a, n / sizeof(T));
    for (int i = n / sizeof(T) * sizeof(T); i < n; i++) {
        arr[i] = ((uint8_t (*)[sizeof(T)])a)[n / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)];
    }
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::XORB(T *a, uint8_t const *arr, int n) {
    for (int i = 0; i < n; i++) {
        EndianUtils<E>::BYTE(a, i) ^= arr[i];
    }
}
template <std::endian E> template <std::unsigned_integral T>
constexpr void EndianUtils<E>::XORB(uint8_t *arr, T const *a, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] ^= EndianUtils<E>::BYTE(a, i);
    }
}
#define GET_BE EndianUtils<std::endian::big>::GET
#define GET_LE EndianUtils<std::endian::little>::GET
#define PUT_BE EndianUtils<std::endian::big>::PUT
#define PUT_LE EndianUtils<std::endian::little>::PUT
#define READ_BE EndianUtils<std::endian::big>::READ
#define READ_LE EndianUtils<std::endian::little>::READ
#define WRITE_BE EndianUtils<std::endian::big>::WRITE
#define WRITE_LE EndianUtils<std::endian::little>::WRITE
#define BYTE_BE EndianUtils<std::endian::big>::BYTE
#define BYTE_LE EndianUtils<std::endian::little>::BYTE
#define READB_BE EndianUtils<std::endian::big>::READB
#define READB_LE EndianUtils<std::endian::little>::READB
#define WRITEB_BE EndianUtils<std::endian::big>::WRITEB
#define WRITEB_LE EndianUtils<std::endian::little>::WRITEB
#define XORB_BE EndianUtils<std::endian::big>::XORB
#define XORB_LE EndianUtils<std::endian::little>::XORB
// Loop unrolling
#define FOR(i, Init, Next, Cond, ...) do {                                              \
    static constexpr auto Arr = []() {                                                  \
        auto i = Init;                                                                  \
        bool $ = Cond;                                                                  \
        std::array<std::pair<decltype(i), bool>, 64> Arr;                               \
        for (int _ = 0; _ < 64; _++) {                                                  \
            Arr[_] = {i, $};                                                            \
            i = Next;                                                                   \
            $ = $ && (Cond);                                                            \
        }                                                                               \
        if ($) {                                                                        \
            throw std::logic_error("Too many iterations");                              \
        }                                                                               \
        return Arr;                                                                     \
    }();                                                                                \
    if constexpr (static constexpr auto &i = Arr[ 0].first; Arr[ 0].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 1].first; Arr[ 1].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 2].first; Arr[ 2].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 3].first; Arr[ 3].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 4].first; Arr[ 4].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 5].first; Arr[ 5].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 6].first; Arr[ 6].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 7].first; Arr[ 7].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 8].first; Arr[ 8].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[ 9].first; Arr[ 9].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[10].first; Arr[10].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[11].first; Arr[11].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[12].first; Arr[12].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[13].first; Arr[13].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[14].first; Arr[14].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[15].first; Arr[15].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[16].first; Arr[16].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[17].first; Arr[17].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[18].first; Arr[18].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[19].first; Arr[19].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[20].first; Arr[20].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[21].first; Arr[21].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[22].first; Arr[22].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[23].first; Arr[23].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[24].first; Arr[24].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[25].first; Arr[25].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[26].first; Arr[26].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[27].first; Arr[27].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[28].first; Arr[28].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[29].first; Arr[29].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[30].first; Arr[30].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[31].first; Arr[31].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[32].first; Arr[32].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[33].first; Arr[33].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[34].first; Arr[34].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[35].first; Arr[35].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[36].first; Arr[36].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[37].first; Arr[37].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[38].first; Arr[38].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[39].first; Arr[39].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[40].first; Arr[40].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[41].first; Arr[41].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[42].first; Arr[42].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[43].first; Arr[43].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[44].first; Arr[44].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[45].first; Arr[45].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[46].first; Arr[46].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[47].first; Arr[47].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[48].first; Arr[48].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[49].first; Arr[49].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[50].first; Arr[50].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[51].first; Arr[51].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[52].first; Arr[52].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[53].first; Arr[53].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[54].first; Arr[54].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[55].first; Arr[55].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[56].first; Arr[56].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[57].first; Arr[57].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[58].first; Arr[58].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[59].first; Arr[59].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[60].first; Arr[60].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[61].first; Arr[61].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[62].first; Arr[62].second) __VA_ARGS__ \
    if constexpr (static constexpr auto &i = Arr[63].first; Arr[63].second) __VA_ARGS__ \
} while (false)
