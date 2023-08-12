# CryptoXX

A modern, high-speed, easy-to-use object-oriented C++ cryptographic library, which aims to use the object-oriented features of C++ to simplify the steps of using, and make cryptographic algorithms modular, so that different cryptographic algorithms have a unified interface, thereby improving the convenience of implementation (scalability) and flexibility of use.

Algorithms are highly optimized (while ensuring readability), and several algorithms run more efficiently than classic cryptographic libraries such as OpenSSL.

一个现代、高速、易用的面向对象 C++ 密码库，旨在利用 C++ 的面向对象特性简化使用步骤，同时使密码算法模块化、让不同密码算法具有统一的接口，从而提升实现时的便捷性（易扩展性）和使用的灵活性。

算法经过高度优化（同时保证了可读性），多种算法运行效率超过 OpenSSL 等经典密码库。

## Currently supported algorithms / 目前支持的算法

- Block cipher algorithms (ECB, CBC, CFB, OFB and CTR mode) / 块密码算法 (多种工作模式)
  - DES
  - 3DES
  - IDEA
  - Blowfish
  - Twofish
  - Serpent
  - Rijndael (with variable blocksize and keysize), AES-128, AES-192, AES-256
  - Camellia
  - SM4
  - CAST-128
  - CAST-256
- Stream cipher algorithms / 流密码算法
  - RC4
  - ChaCha
  - Salsa
  - ZUC (祖冲之算法)
- Hashing Algorithms (and the corresponding HMAC algorithms) / 哈希算法（及对应的 HMAC 算法）
  - CRC
  - MD5
  - SHA-1
  - SHA-2 (SHA-256, SHA-224, SHA-512, SHA-384)
  - SHA-3, Keccak, SHAKE-128, SHAKE-256
  - BLAKE2 (BLAKE2s-256, BLAKE2s-224, BLAKE2b-512, BLAKE2b-384)
  - BLAKE3 (Experimental)
  - Whirlpool
  - SM3
  - To be continued...

## Compiling and running the sample program / 编译并运行示例程序

### Compile / 编译

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX
clang++ cipher.cpp -std=c++20 -O2 -o cipher.out
clang++ hash.cpp -std=c++20 -O2 -o hash.out
```

### Test / 测试

```sh
./cipher_test.sh # Run the test script of the encryption and decryption algorithms.
./hash_test.sh # Run the test script of the hash algorithms.
```

### Usage / 使用

- cipher.out

```
Usage: ./cipher.out <algorithm> <mode> <key> [iv]
Supported stream ciphers:
    ChaCha20, Salsa20, RC4, ZUC
Supported stream cipher modes:
    Enc, Dec, Gen
Supported block ciphers:
    AES128, Camellia128, Serpent128, Twofish128,
    AES192, Camellia192, Serpent192, Twofish192,
    AES256, Camellia256, Serpent256, Twofish256,
    DES, TDES2K, TDES3K, IDEA, CAST128, CAST256,
    SM4, Blowfish
Supported block cipher modes:
    ECBEnc, ECBDec, CTREnc, CTRDec, CTRGen,
    CFBEnc, CFBDec, OFBEnc, OFBDec, OFBGen,
    CBCEnc, CBCDec, PCBCEnc, PCBCDec
* Program will read from stdin and write to stdout.
* Key and iv should be hex strings.
```

```sh
./cipher.out AES192 ECBEnc 0123456789abcdeffedcba9876543210 < in.txt > out.enc
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.enc.

./cipher.out AES192 ECBDec 0123456789abcdeffedcba9876543210 < out.enc
# Decrypt out.enc and output to command line.

./cipher.out RC4 Enc 0123456789abcdeffedcba9876543210 > out.enc
# Read from command line and encrypt with RC4 algorithm.

./cipher.out SM4 CTRGen 0123456789abcdeffedcba9876543210 0123456789abcdeffedcba9876543210 | head -c 1024 > random.bin
# Generate 1024 random bytes with SM4-CTR mode to random.bin.
```

- hash.out

```
Usage: ./hash.out <algorithm> [key length] [key]
Available algorithms (Hash only):
    CRC32, CRC64, BLAKE3
Available algorithms (Hash and MAC):
    BLAKE2b384, BLAKE2b512, BLAKE2s256, BLAKE2s224
Available algorithms (Hash and HMAC):
    SHA224, SHA256, SHA3-224, SHA3-256, SHAKE128,
    SHA384, SHA512, SHA3-384, SHA3-512, SHAKE256,
    SM3, MD5, SHA, Whirlpool
* Program will read from stdin and output to stdout.
* Key should be a hex string.
```

```sh
./hash.out MD5 < in.txt
# Output the MD5 checksum of in.txt.

./hash.out SHA3-256 3 6b6579 < in.txt
# Output the SHA3-256-HMAC of in.txt with a 3-bytes key "\x6b\x65\x79" ("key").
```

## Library usage documentation / 密码库使用说明

To be continued...
