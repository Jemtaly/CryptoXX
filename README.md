# CryptoXX

A modern, high speed and elegant OOP cryptographic library implemented in C++. Currently supports DES, 3DES, AES, SM4 block cipher algorithms in ECB and CTR modes, RC4 and ZUC stream cipher algorithm, as well as SM3, MD5 and CRC hashing algorithms.

一个现代、高速、优雅的面向对象 C++ 密码库，目前支持 DES，3DES，AES-128，AES-192，AES-256 以及 SM4 等块密码算法在 ECB 和 CTR 模式下的加解密，RC4 和 ZUC 流密码，以及 SM3，MD5 和 CRC 哈希算法。

## Compiling and running the sample program / 编译并运行示例程序

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX
clang++ example.cpp -std=c++2a -O3 -o example.exe
```

```sh
./example.exe -6 -k 0123456789abcdeffedcba9876543210 -e -i in.txt -o out.txt
# Encrypt in.txt in ECB mode with the AES-192 algorithm and output to out.txt. (-4 means AES-128, -6 means AES-192, -8 means AES-256, -S means SM4, KEY and IV are entered in hexadecimal format)
./example.exe -6 -k 0123456789abcdeffedcba9876543210 -d -i out.txt
# Decrypt out.txt and output to command line.
./example.exe -S -k 0123456789abcdeffedcba9876543210 -c 0123456789abcdeffedcba9876543210 -o out.txt
# Read from command line and encrypt/decrypt in CTR mode with the SM4 algorithm.
```
