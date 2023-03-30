# CryptoXX

A modern, high-speed, easy-to-use object-oriented C++ cryptographic library, which aims to use the object-oriented features of C++ to simplify the steps of using, and make cryptographic algorithms modular, so that different cryptographic algorithms have a unified interface, thereby improving the convenience of implementation (scalability) and flexibility of use. Optimize algorithm performance at compile time using various new features of C++17 and C++20.

一个现代、高速、易用的面向对象 C++ 密码库，旨在利用 C++ 的面向对象特性简化使用步骤，同时使密码算法模块化、让不同密码算法具有统一的接口，从而提升实现时的便捷性（易扩展性）和使用的灵活性。使用 C++17 和 C++20 的多种新特性在编译时对算法运行效率进行优化。

## Currently supported algorithms / 目前支持的算法

- Block cipher algorithms (ECB, CBC, CFB, OFB and CTR mode) / 块密码算法 (多种工作模式)
  - DES
  - 3DES
  - AES (AES-128, AES-192, AES-256)
  - SM4
- Stream cipher algorithms / 流密码算法
  - RC4
  - ZUC (祖冲之算法)
  - ChaCha20
- Hash Algorithms / 哈希算法（及对应的 HMAC 算法）
  - CRC
  - MD5
  - SM3
  - SHA-1
  - SHA-2 (SHA-256, SHA-224, SHA-512, SHA-384)
  - SHA-3, Keccak, SHAKE-128, SHAKE-256
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
Usage: cipher.exe [-i INFILE] [-o OUTFILE] (-c IV | -e | -d) (-S KEY | -4 KEY | -6 KEY | -8 KEY)
Options:
  -i INFILE   input file (default: stdin)
  -o OUTFILE  output file (default: stdout)
  -c IV       counter mode (IV: 128-bit IV in hex)
  -e          encrypt mode
  -d          decrypt mode
  -S KEY      SM4 (KEY: 128-bit key in hex)
  -4 KEY      AES-128 (KEY: 128-bit key in hex)
  -6 KEY      AES-192 (KEY: 192-bit key in hex)
  -8 KEY      AES-256 (KEY: 256-bit key in hex)
```

```sh
./cipher.exe -6 0123456789abcdeffedcba9876543210 -e -i in.txt -o out.txt
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.txt.

./cipher.exe -6 0123456789abcdeffedcba9876543210 -d -i out.txt
# Decrypt out.txt and output to command line.

./cipher.exe -S 0123456789abcdeffedcba9876543210 -c 0123456789abcdeffedcba9876543210 -o out.txt
# Read from command line and encrypt/decrypt in CTR mode with the SM4 algorithm.
```

### hash.cpp

- Compile

```sh
clang++ hash.cpp -std=c++20 -O2 -o hash.exe
```

- Usage

```
Description: HMAC/Hash Calculator
Usage: hash.exe [FILE] (-5 | -6)
       hash.exe [FILE] (-M | -X | -7 | -8 | -3 | -4 | -S) [-H LEN KEY]
Options:
  FILE        input file (default: stdin)
  -H LEN KEY  HMAC (LEN: key byte length, KEY: key in hex)
  -M          MD5
  -X          SHA-1
  -7          SHA-224
  -8          SHA-256
  -3          SHA-384
  -4          SHA-512
  -S          SM3
  -5          CRC-32
  -6          CRC-64
```

```sh
cat in.txt | ./hash.exe -M
# Output the MD5 checksum of in.txt.

./hash.exe -8 -H 3 6b6579 in.txt
# Output the SHA-256-HMAC of in.txt with key "\x6b\x65\x79" ("key").
```

## Library usage documentation / 密码库使用说明

To be continued...
