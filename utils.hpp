#pragma once
#include <utility> // std::forward
#include <stdint.h>
#include <string.h>
#include <concepts>
template <std::unsigned_integral T>
constexpr inline T ROTL(T const x, uint8_t const n) {
    return (x << (n & (sizeof(T) * 8 - 1)) | (x) >> (-n & (sizeof(T) * 8 - 1)));
}
template <std::unsigned_integral T>
constexpr inline T ROTR(T const x, uint8_t const n) {
    return (x >> (n & (sizeof(T) * 8 - 1)) | (x) << (-n & (sizeof(T) * 8 - 1)));
}
template <std::unsigned_integral T>
constexpr inline T GET_FW(uint8_t const *arr);
template <>
constexpr inline uint16_t GET_FW(uint8_t const *arr) {
    return
        (uint16_t)(arr)[0]       | (uint16_t)(arr)[1] <<  8;
}
template <>
constexpr inline uint32_t GET_FW(uint8_t const *arr) {
    return
        (uint32_t)(arr)[0]       | (uint32_t)(arr)[1] <<  8 |
        (uint32_t)(arr)[2] << 16 | (uint32_t)(arr)[3] << 24;
}
template <>
constexpr inline uint64_t GET_FW(uint8_t const *arr) {
    return
        (uint64_t)(arr)[0]       | (uint64_t)(arr)[1] <<  8 |
        (uint64_t)(arr)[2] << 16 | (uint64_t)(arr)[3] << 24 |
        (uint64_t)(arr)[4] << 32 | (uint64_t)(arr)[5] << 40 |
        (uint64_t)(arr)[6] << 48 | (uint64_t)(arr)[7] << 56;
}
template <std::unsigned_integral T>
constexpr inline T GET_BW(uint8_t const *arr);
template <>
constexpr inline uint16_t GET_BW(uint8_t const *arr) {
    return
        (uint16_t)(arr)[0] <<  8 | (uint16_t)(arr)[1]      ;
}
template <>
constexpr inline uint32_t GET_BW(uint8_t const *arr) {
    return
        (uint32_t)(arr)[0] << 24 | (uint32_t)(arr)[1] << 16 |
        (uint32_t)(arr)[2] <<  8 | (uint32_t)(arr)[3]      ;
}
template <>
constexpr inline uint64_t GET_BW(uint8_t const *arr) {
    return
        (uint64_t)(arr)[0] << 56 | (uint64_t)(arr)[1] << 48 |
        (uint64_t)(arr)[2] << 40 | (uint64_t)(arr)[3] << 32 |
        (uint64_t)(arr)[4] << 24 | (uint64_t)(arr)[5] << 16 |
        (uint64_t)(arr)[6] <<  8 | (uint64_t)(arr)[7]      ;
}
template <std::unsigned_integral T>
constexpr inline void PUT_FW(uint8_t *arr, T w);
template <>
constexpr inline void PUT_FW(uint8_t *arr, uint16_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8       ;
}
template <>
constexpr inline void PUT_FW(uint8_t *arr, uint32_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8 & 0xff;
    arr[2] = w >> 16 & 0xff;
    arr[3] = w >> 24       ;
}
template <>
constexpr inline void PUT_FW(uint8_t *arr, uint64_t w) {
    arr[0] = w       & 0xff;
    arr[1] = w >>  8 & 0xff;
    arr[2] = w >> 16 & 0xff;
    arr[3] = w >> 24 & 0xff;
    arr[4] = w >> 32 & 0xff;
    arr[5] = w >> 40 & 0xff;
    arr[6] = w >> 48 & 0xff;
    arr[7] = w >> 56       ;
}
template <std::unsigned_integral T>
constexpr inline void PUT_BW(uint8_t *arr, T w);
template <>
constexpr inline void PUT_BW(uint8_t *arr, uint16_t w) {
    arr[0] = w >>  8       ;
    arr[1] = w       & 0xff;
}
template <>
constexpr inline void PUT_BW(uint8_t *arr, uint32_t w) {
    arr[0] = w >> 24       ;
    arr[1] = w >> 16 & 0xff;
    arr[2] = w >>  8 & 0xff;
    arr[3] = w       & 0xff;
}
template <>
constexpr inline void PUT_BW(uint8_t *arr, uint64_t w) {
    arr[0] = w >> 56       ;
    arr[1] = w >> 48 & 0xff;
    arr[2] = w >> 40 & 0xff;
    arr[3] = w >> 32 & 0xff;
    arr[4] = w >> 24 & 0xff;
    arr[5] = w >> 16 & 0xff;
    arr[6] = w >>  8 & 0xff;
    arr[7] = w       & 0xff;
}
template <std::unsigned_integral T>
constexpr inline void READ_FW(T *a, uint8_t const *arr, int n) {
    memcpy(a, arr, sizeof(T) * n);
}
template <std::unsigned_integral T>
constexpr inline void READ_BW(T *a, uint8_t const *arr, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = GET_BW<T>(arr + sizeof(T) * i);
    }
}
template <std::unsigned_integral T>
constexpr inline void WRITE_FW(uint8_t *arr, T const *a, int n) {
    memcpy(arr, a, sizeof(T) * n);
}
template <std::unsigned_integral T>
constexpr inline void WRITE_BW(uint8_t *arr, T const *a, int n) {
    for (int i = 0; i < n; i++) {
        PUT_BW(arr + sizeof(T) * i, a[i]);
    }
}
template <std::unsigned_integral T>
constexpr inline uint8_t &BYTE_FW(T *a, int i) {
    return ((uint8_t *)a)[i];
}
template <std::unsigned_integral T>
constexpr inline uint8_t &BYTE_BW(T *a, int i) {
    return ((uint8_t (*)[sizeof(T)])a)[i / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)];
}
template <std::unsigned_integral T>
constexpr inline uint8_t const &BYTE_FW(T const *a, int i) {
    return ((uint8_t *)a)[i];
}
template <std::unsigned_integral T>
constexpr inline uint8_t const &BYTE_BW(T const *a, int i) {
    return ((uint8_t (*)[sizeof(T)])a)[i / sizeof(T)][sizeof(T) - 1 - i % sizeof(T)];
}
template <std::unsigned_integral T>
constexpr inline void READB_FW(T *a, uint8_t const *arr, int n) {
    memcpy(a, arr, n);
}
template <std::unsigned_integral T>
constexpr inline void READB_BW(T *a, uint8_t const *arr, int n) {
    READ_BW(a, arr, n / sizeof(T));
    for (int i = 0; i < n % sizeof(T); i++) {
        ((uint8_t (*)[sizeof(T)])a)[n / sizeof(T)][i] = arr[n - 1 - i];
    }
}
template <std::unsigned_integral T>
constexpr inline void WRITEB_FW(uint8_t *arr, T const *a, int n) {
    memcpy(arr, a, n);
}
template <std::unsigned_integral T>
constexpr inline void WRITEB_BW(uint8_t *arr, T const *a, int n) {
    WRITE_BW(arr, a, n / sizeof(T));
    for (int i = 0; i < n % sizeof(T); i++) {
        arr[n - 1 - i] = ((uint8_t (*)[sizeof(T)])a)[n / sizeof(T)][i];
    }
}
template <std::unsigned_integral T>
constexpr inline void XORB_FW(T *a, uint8_t const *arr, int n) {
    for (int i = 0; i < n; i++) {
        BYTE_FW(a, i) ^= arr[i];
    }
}
template <std::unsigned_integral T>
constexpr inline void XORB_BW(T *a, uint8_t const *arr, int n) {
    for (int i = 0; i < n; i++) {
        BYTE_BW(a, i) ^= arr[i];
    }
}
template <std::unsigned_integral T>
constexpr inline void XORB_FW(uint8_t *arr, T const *a, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] ^= BYTE_FW(a, i);
    }
}
template <std::unsigned_integral T>
constexpr inline void XORB_BW(uint8_t *arr, T const *a, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] ^= BYTE_BW(a, i);
    }
}
#if 'ABCD' == 0x41424344 // little endian
#define GET_LE    GET_FW
#define GET_BE    GET_BW
#define PUT_LE    PUT_FW
#define PUT_BE    PUT_BW
#define READ_LE   READ_FW
#define READ_BE   READ_BW
#define WRITE_LE  WRITE_FW
#define WRITE_BE  WRITE_BW
#define BYTE_LE   BYTE_FW
#define BYTE_BE   BYTE_BW
#define READB_LE  READB_FW
#define READB_BE  READB_BW
#define WRITEB_LE WRITEB_FW
#define WRITEB_BE WRITEB_BW
#define XORB_LE   XORB_FW
#define XORB_BE   XORB_BW
#elif 'ABCD' == 0x44434241 // big endian
#define GET_LE    GET_BW
#define GET_BE    GET_FW
#define PUT_LE    PUT_BW
#define PUT_BE    PUT_FW
#define READ_LE   READ_BW
#define READ_BE   READ_FW
#define WRITE_LE  WRITE_BW
#define WRITE_BE  WRITE_FW
#define BYTE_LE   BYTE_BW
#define BYTE_BE   BYTE_FW
#define READB_LE  READB_BW
#define READB_BE  READB_FW
#define WRITEB_LE WRITEB_BW
#define WRITEB_BE WRITEB_FW
#define XORB_LE   XORB_BW
#define XORB_BE   XORB_FW
#endif
// C++20 Loop unrolling
template <int Start, int Stop, int Step = 1, bool Eq = false, typename F>
constexpr inline void FOR(F &&f) {
    if constexpr (Step > 0 && (Start < Stop || Eq && Start == Stop) || Step < 0 && (Start > Stop || Eq && Start == Stop)) {
        f(std::integral_constant<int, Start>{});
        FOR<Start + Step, Stop, Step, Eq>(std::forward<F>(f));
    }
}
