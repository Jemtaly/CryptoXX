# CryptoXX

A modern, efficient and easy-to-use object-oriented C++ cryptographic library designed to make cryptographic algorithms modular using object-oriented techniques, allowing different cryptographic algorithms to have a uniform interface, thus improving ease of implementation (scalability) and flexibility of use. The efficiency of the algorithms is optimized at compile time using various new features of C++17 and C++20.

一个现代、高效、易用的面向对象 C++ 密码库，该密码库旨在利用面向对象技术让密码算法模块化，让不同密码算法具有统一的接口，从而提升实现时的便捷性（易扩展性）和使用的灵活性。使用 C++17 和 C++20 的多种新特性在编译时对算法运行效率进行优化。

## Currently supported algorithms / 目前支持的算法

- Block cipher algorithms (ECB mode and CTR mode) / 块密码算法 (ECB 模式和 CTR 模式)
  - DES
  - 3DES
  - AES (AES-128, AES-192, AES-256)
  - SM4
- Stream cipher algorithms / 流密码算法
  - RC4
  - ZUC (祖冲之算法)
  - ChaCha
- Hash Algorithms / 哈希算法
  - CRC
  - MD5
  - SM3
  - To be continued...

## Compiling and running the sample program / 编译并运行示例程序

- example.cpp

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX
clang++ example.cpp -std=c++17 -O3 -o example.exe
# Don't use -O3 option if you are compiling with gcc.
```

```sh
./example.exe -6 -k 0123456789abcdeffedcba9876543210 -e -i in.txt -o out.txt
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.txt.
# (-4 means AES-128, -6 means AES-192, -8 means AES-256, -S means SM4, KEY and IV are entered in hexadecimal format)
./example.exe -6 -k 0123456789abcdeffedcba9876543210 -d -i out.txt
# Decrypt out.txt and output to command line.
./example.exe -S -k 0123456789abcdeffedcba9876543210 -c 0123456789abcdeffedcba9876543210 -o out.txt
# Read from command line and encrypt/decrypt in CTR mode with the SM4 algorithm.
```

- hash.cpp

```sh
clang++ hash.cpp -std=c++17 -O3 -o hash.exe
```

```sh
cat in.txt | ./hash.exe
# Output the MD5 and SM3 checksums of in.txt.
```

## Library usage documentation / 密码库使用说明

To be continued...
