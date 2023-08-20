#!/bin/bash
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <path to hash binary>"
    exit 1
fi
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
big_file=$(mktemp)
big_size=123456789
head -c $big_size /dev/urandom >$big_file
for i in ${!algs[@]}; do
    alg=${algs[$i]}
    ssl=${ssls[$i]}
    echo -n "Testing $alg... "
    beg_time=$(date +%s.%N)
    ssl_hash=$(
        cat $big_file |
            openssl dgst -$ssl | cut -d' ' -f2
    )
    end_time=$(date +%s.%N)
    ssl_time=$(echo "$end_time - $beg_time" | bc)
    beg_time=$(date +%s.%N)
    alg_hash=$(
        cat $big_file |
            "$@" $alg
    )
    end_time=$(date +%s.%N)
    alg_time=$(echo "$end_time - $beg_time" | bc)
    if [[ ${alg_hash:0:${#ssl_hash}} == $ssl_hash ]]; then
        echo "OK ($alg_time vs $ssl_time)"
    else
        echo "FAIL"
        exit 1
    fi
done
