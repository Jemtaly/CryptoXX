# CryptoXX

A modern, high speed and convenient cryptographic library implemented in C++. Currently supports DES, 3DES, AES, SM4 block cipher algorithms in ECB and CTR modes, as well as SM3 and CRC hashing algorithms.

一个现代、高速、便于使用的 C++ 密码库，目前支持 DES，3DES，AES-128，AES-192，AES-256 以及 SM4 等块密码算法在 ECB 和 CTR 模式下的加解密，以及 SM3 和 CRC 哈希算法。

## Compiling and running the sample program / 编译并运行示例程序

```sh
git clone https://github.com/Jemtaly/CryptoXX
cd CryptoXX/block
clang++ main.cpp -std=c++2a -Os -o main.exe
```

```sh
./main.exe -i in.txt -o out.txt -k ABCDEFGHABCDEFGH -e
# Encrypt in.txt in ECB mode with the AES-128 algorithm and output to out.txt.
./main.exe -i out.txt -k ABCDEFGHABCDEFGH -d
# Decrypt out.txt and output to command line.
./main.exe -6 -k XXXXXXXXXXXXXXXXXXXXXXXX -c IVIVIVIVIVIVIVIV -o out.txt
# Read from command line and encrypt/decrypt in CTR mode with the AES-192 algorithm.
```
