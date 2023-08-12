#!/bin/bash
hash=./hash.out
algs=(
    "SM3" "MD5" "SHA"
    "SHA224" "SHA256"
    "SHA384" "SHA512"
    "SHA3-224" "SHA3-256"
    "SHA3-384" "SHA3-512"
    "SHAKE128" "SHAKE256"
    "BLAKE2b512" "BLAKE2s256"
    "Whirlpool"
)
ssls=(
    "sm3" "md5" "sha1"
    "sha224" "sha256"
    "sha384" "sha512"
    "sha3-224" "sha3-256"
    "sha3-384" "sha3-512"
    "shake128" "shake256"
    "blake2b512" "blake2s256"
    "whirlpool"
)
tmp_file=$(mktemp)
tmp_size=+01234567
head -c $tmp_size /dev/urandom >$tmp_file
for i in ${!algs[@]}; do
    alg=${algs[$i]}
    ssl=${ssls[$i]}
    echo -n "Testing $alg... "
    ssl_hash=$(cat $tmp_file | openssl dgst -$ssl | cut -d' ' -f2)
    alg_hash=$(cat $tmp_file | $hash $alg)
    if [[ ${alg_hash:0:${#ssl_hash}} == $ssl_hash ]]; then
        echo "OK"
    else
        echo "FAIL"
        exit 1
    fi
done
