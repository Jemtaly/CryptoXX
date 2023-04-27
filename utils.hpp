#pragma once
#include <array>
#include <utility> // std::forward
#include <stdint.h>
#include <string.h>
#include <concepts>
using bits_t = uint8_t;
template <std::unsigned_integral T>
constexpr inline T ROTL(T const x, bits_t const n) {
    return (x << (n & (sizeof(T) * 8 - 1)) | (x) >> (-n & (sizeof(T) * 8 - 1)));
}
template <std::unsigned_integral T>
constexpr inline T ROTR(T const x, bits_t const n) {
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
// Loop unrolling
// template <int Start, int Stop, int Step = 1, bool Eq = false, typename F>
// constexpr inline void FOR(F &&f) {
//     if constexpr (Step > 0 && (Start < Stop || Eq && Start == Stop) || Step < 0 && (Start > Stop || Eq && Start == Stop)) {
//         f(std::integral_constant<int, Start>{});
//         FOR<Start + Step, Stop, Step, Eq>(std::forward<F>(f));
//     }
// }
#define FOR(i, Init, Next, Cond, Proc) do {                                      \
    static constexpr auto Arr = []() {                                           \
        auto i = Init;                                                           \
        bool b = Cond;                                                           \
        std::array<std::pair<decltype(i), bool>, 65> Arr;                        \
        for (int j = 0; j < 64; j++) {                                           \
            Arr[j] = {i, b};                                                     \
            i = Next;                                                            \
            b = b && Cond;                                                       \
        }                                                                        \
        Arr[64] = {i, b};                                                        \
        return Arr;                                                              \
    }();                                                                         \
    if constexpr (static constexpr auto &i = Arr[ 0].first; Arr[ 0].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 1].first; Arr[ 1].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 2].first; Arr[ 2].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 3].first; Arr[ 3].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 4].first; Arr[ 4].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 5].first; Arr[ 5].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 6].first; Arr[ 6].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 7].first; Arr[ 7].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 8].first; Arr[ 8].second) Proc \
    if constexpr (static constexpr auto &i = Arr[ 9].first; Arr[ 9].second) Proc \
    if constexpr (static constexpr auto &i = Arr[10].first; Arr[10].second) Proc \
    if constexpr (static constexpr auto &i = Arr[11].first; Arr[11].second) Proc \
    if constexpr (static constexpr auto &i = Arr[12].first; Arr[12].second) Proc \
    if constexpr (static constexpr auto &i = Arr[13].first; Arr[13].second) Proc \
    if constexpr (static constexpr auto &i = Arr[14].first; Arr[14].second) Proc \
    if constexpr (static constexpr auto &i = Arr[15].first; Arr[15].second) Proc \
    if constexpr (static constexpr auto &i = Arr[16].first; Arr[16].second) Proc \
    if constexpr (static constexpr auto &i = Arr[17].first; Arr[17].second) Proc \
    if constexpr (static constexpr auto &i = Arr[18].first; Arr[18].second) Proc \
    if constexpr (static constexpr auto &i = Arr[19].first; Arr[19].second) Proc \
    if constexpr (static constexpr auto &i = Arr[20].first; Arr[20].second) Proc \
    if constexpr (static constexpr auto &i = Arr[21].first; Arr[21].second) Proc \
    if constexpr (static constexpr auto &i = Arr[22].first; Arr[22].second) Proc \
    if constexpr (static constexpr auto &i = Arr[23].first; Arr[23].second) Proc \
    if constexpr (static constexpr auto &i = Arr[24].first; Arr[24].second) Proc \
    if constexpr (static constexpr auto &i = Arr[25].first; Arr[25].second) Proc \
    if constexpr (static constexpr auto &i = Arr[26].first; Arr[26].second) Proc \
    if constexpr (static constexpr auto &i = Arr[27].first; Arr[27].second) Proc \
    if constexpr (static constexpr auto &i = Arr[28].first; Arr[28].second) Proc \
    if constexpr (static constexpr auto &i = Arr[29].first; Arr[29].second) Proc \
    if constexpr (static constexpr auto &i = Arr[30].first; Arr[30].second) Proc \
    if constexpr (static constexpr auto &i = Arr[31].first; Arr[31].second) Proc \
    if constexpr (static constexpr auto &i = Arr[32].first; Arr[32].second) Proc \
    if constexpr (static constexpr auto &i = Arr[33].first; Arr[33].second) Proc \
    if constexpr (static constexpr auto &i = Arr[34].first; Arr[34].second) Proc \
    if constexpr (static constexpr auto &i = Arr[35].first; Arr[35].second) Proc \
    if constexpr (static constexpr auto &i = Arr[36].first; Arr[36].second) Proc \
    if constexpr (static constexpr auto &i = Arr[37].first; Arr[37].second) Proc \
    if constexpr (static constexpr auto &i = Arr[38].first; Arr[38].second) Proc \
    if constexpr (static constexpr auto &i = Arr[39].first; Arr[39].second) Proc \
    if constexpr (static constexpr auto &i = Arr[40].first; Arr[40].second) Proc \
    if constexpr (static constexpr auto &i = Arr[41].first; Arr[41].second) Proc \
    if constexpr (static constexpr auto &i = Arr[42].first; Arr[42].second) Proc \
    if constexpr (static constexpr auto &i = Arr[43].first; Arr[43].second) Proc \
    if constexpr (static constexpr auto &i = Arr[44].first; Arr[44].second) Proc \
    if constexpr (static constexpr auto &i = Arr[45].first; Arr[45].second) Proc \
    if constexpr (static constexpr auto &i = Arr[46].first; Arr[46].second) Proc \
    if constexpr (static constexpr auto &i = Arr[47].first; Arr[47].second) Proc \
    if constexpr (static constexpr auto &i = Arr[48].first; Arr[48].second) Proc \
    if constexpr (static constexpr auto &i = Arr[49].first; Arr[49].second) Proc \
    if constexpr (static constexpr auto &i = Arr[50].first; Arr[50].second) Proc \
    if constexpr (static constexpr auto &i = Arr[51].first; Arr[51].second) Proc \
    if constexpr (static constexpr auto &i = Arr[52].first; Arr[52].second) Proc \
    if constexpr (static constexpr auto &i = Arr[53].first; Arr[53].second) Proc \
    if constexpr (static constexpr auto &i = Arr[54].first; Arr[54].second) Proc \
    if constexpr (static constexpr auto &i = Arr[55].first; Arr[55].second) Proc \
    if constexpr (static constexpr auto &i = Arr[56].first; Arr[56].second) Proc \
    if constexpr (static constexpr auto &i = Arr[57].first; Arr[57].second) Proc \
    if constexpr (static constexpr auto &i = Arr[58].first; Arr[58].second) Proc \
    if constexpr (static constexpr auto &i = Arr[59].first; Arr[59].second) Proc \
    if constexpr (static constexpr auto &i = Arr[60].first; Arr[60].second) Proc \
    if constexpr (static constexpr auto &i = Arr[61].first; Arr[61].second) Proc \
    if constexpr (static constexpr auto &i = Arr[62].first; Arr[62].second) Proc \
    if constexpr (static constexpr auto &i = Arr[63].first; Arr[63].second) Proc \
    static_assert(not Arr[64].second, "Too many iterations");                    \
} while (false)
