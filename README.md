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

### cipher.cpp

- Compile

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX
clang++ cipher.cpp -std=c++20 -O2 -o cipher.exe
```

- Usage

```txt
Description: SM4/AES Encryption/Decryption Tool
Usage: cipher.exe <algorithm> <mode> <key> [iv]
- The modes supported by block ciphers include:
  CTREnc, CTRDec, CTRGen, OFBEnc, OFBDec, OFBGen,
  CFBEnc, CFBDec, ECBEnc, ECBDec, CBCEnc, CBCDec, 
  PCBCEnc, PCBCDec.
- The modes supported by stream ciphers include:
  Enc (encryption mode), Dec (decryption mode), Gen (Pseudo random generator mode).
```

```sh
./cipher.exe AES192 ECBEnc 0123456789abcdeffedcba9876543210 < in.txt > out.txt
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.txt.

./cipher.exe AES192 ECBDec 0123456789abcdeffedcba9876543210 < out.txt
# Decrypt out.txt and output to command line.

./cipher.exe RC4 Enc 0123456789abcdeffedcba9876543210 > out.txt
# Read from command line and encrypt with RC4 algorithm.

./cipher.exe SM4 CTRGen 0123456789abcdeffedcba9876543210 0123456789abcdeffedcba9876543210 | head -c 1024 > random.txt
# Generate 1024 random bits with SM4-CTR mode.
```

### hash.cpp

- Compile

```sh
clang++ hash.cpp -std=c++20 -O2 -o hash.exe
```

- Usage

```
Description: HMAC/Hash Calculator
Usage: hash.exe <algorithm> [key]
```

```sh
./hash.exe MD5 < in.txt
# Output the MD5 checksum of in.txt.

./hash.exe SHA3-256 6b6579 < in.txt
# Output the SHA3-256-HMAC of in.txt with key "\x6b\x65\x79" ("key").
```

## Library usage documentation / 密码库使用说明

To be continued...
