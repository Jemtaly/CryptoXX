# CryptoXX

A modern, high-speed, easy-to-use object-oriented C++ cryptographic library, which aims to use the object-oriented features of C++ to simplify the steps of using, and make cryptographic algorithms modular, so that different cryptographic algorithms have a unified interface, thereby improving the convenience of implementation (scalability) and flexibility of use. Optimize algorithm performance at compile time using various new features of C++17 and C++20.

一个现代、高速、易用的面向对象 C++ 密码库，旨在利用 C++ 的面向对象特性简化使用步骤，同时使密码算法模块化、让不同密码算法具有统一的接口，从而提升实现时的便捷性（易扩展性）和使用的灵活性。使用 C++17 和 C++20 的多种新特性在编译时对算法运行效率进行优化。

## Currently supported algorithms / 目前支持的算法

- Block cipher algorithms (ECB mode and CTR mode) / 块密码算法 (ECB 模式和 CTR 模式)
  - DES
  - 3DES
  - AES (AES-128, AES-192, AES-256)
  - SM4
- Stream cipher algorithms / 流密码算法
  - RC4
  - ZUC (祖冲之算法)
  - ChaCha20
- Hash Algorithms / 哈希算法
  - CRC
  - MD5
  - SM3
  - SHA-1
  - SHA-2 (SHA-256, SHA-224, SHA-512, SHA-384)
  - SHA-3 (and Keccak)
  - To be continued...

## Compiling and running the sample program / 编译并运行示例程序

- cipher.cpp

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX
clang++ cipher.cpp -std=c++20 -O2 -o cipher.exe
```

```sh
./cipher.exe -6 0123456789abcdeffedcba9876543210 -e -i in.txt -o out.txt
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.txt.
# (-4 means AES-128, -6 means AES-192, -8 means AES-256, -S means SM4, KEY and IV are entered in hexadecimal format)
./cipher.exe -6 0123456789abcdeffedcba9876543210 -d -i out.txt
# Decrypt out.txt and output to command line.
./cipher.exe -S 0123456789abcdeffedcba9876543210 -c 0123456789abcdeffedcba9876543210 -o out.txt
# Read from command line and encrypt/decrypt in CTR mode with the SM4 algorithm.
```

- hash.cpp

```sh
clang++ hash.cpp -std=c++20 -O2 -o hash.exe
```

```sh
cat in.txt | ./hash.exe -M
# Output the MD5 checksum of in.txt.
# (-M for MD5, -S for SM3, -X for SHA-1, -8 for SHA-256, -7 for SHA-224, -4 for SHA-512, -3 for SHA-384)
./hash.exe -8 -i ./hash.exe
# Output the SHA-256 checksum of hash.exe.
```

## Library usage documentation / 密码库使用说明

To be continued...
