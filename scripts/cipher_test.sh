#!/bin/bash
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <path to cipher binary>"
    exit 1
fi
algs=(
    "AES128" "AES192" "AES256"
    "ARIA128" "ARIA192" "ARIA256"
    "Camellia128" "Camellia192" "Camellia256"
    "SM4" "SEED" "Blowfish"
    "DES" "TDES2K" "TDES3K"
    "Twofish128" "Twofish192" "Twofish256"
    "Serpent128" "Serpent192" "Serpent256"
    "CAST128" "CAST256" "IDEA"
)
ssls=(
    "aes-128" "aes-192" "aes-256"
    "aria-128" "aria-192" "aria-256"
    "camellia-128" "camellia-192" "camellia-256"
    "sm4" "seed" "bf"
    "des" "des-ede" "des-ede3"
    "" "" ""
    "" "" ""
    "" "" ""
)
keys=(
    16 24 32
    16 24 32
    16 24 32
    16 16 16
    8 16 24
    16 24 32
    16 24 32
    16 32 16
)
civs=(
    16 16 16
    16 16 16
    16 16 16
    16 16 8
    8 8 8
    16 16 16
    16 16 16
    8 16 8
)
modes=(
    "OFB" "CTR" "CFB"
    "CBC" "PCBC" "ECB"
)
big_file=$(mktemp)
big_size=123456789
head -c $big_size /dev/urandom >$big_file
tmp_file=$(mktemp)
tmp_size=+01234567
head -c $tmp_size /dev/urandom >$tmp_file
tmp_hash=$(
    cat $tmp_file |
        md5sum | cut -d' ' -f1
)
for i in ${!algs[@]}; do
    alg=${algs[$i]}
    key=$(head -c ${keys[$i]} /dev/urandom | od -An -tx1 | tr -d ' \n')
    civ=$(head -c ${civs[$i]} /dev/urandom | od -An -tx1 | tr -d ' \n')
    ssl=${ssls[$i]}
    if [[ $ssl != "" ]]; then
        echo -n "Comparing $alg with openssl... "
        beg_time=$(date +%s.%N)
        enc_hash=$(
            cat $big_file |
                "$@" $alg "ECBEnc" $key |
                md5sum | cut -d' ' -f1
        )
        end_time=$(date +%s.%N)
        enc_time=$(echo "$end_time - $beg_time" | bc)
        beg_time=$(date +%s.%N)
        ssl_hash=$(
            cat $big_file |
                openssl $ssl-ecb -K $key |
                md5sum | cut -d' ' -f1
        )
        end_time=$(date +%s.%N)
        ssl_time=$(echo "$end_time - $beg_time" | bc)
        if [[ $enc_hash == $ssl_hash ]]; then
            echo "OK ($enc_time vs $ssl_time)"
        else
            echo "FAIL"
            exit 1
        fi
    fi
    for mode in ${modes[@]}; do
        echo -n "Testing $alg $mode... "
        if [[ $mode == "ECB" ]]; then
            civ=""
        fi
        dec_hash=$(
            cat $tmp_file |
                "$@" $alg $mode"Enc" $key $civ |
                "$@" $alg $mode"Dec" $key $civ |
                md5sum | cut -d' ' -f1
        )
        if [[ $dec_hash == $tmp_hash ]]; then
            echo "OK"
        else
            echo "FAIL"
            exit 1
        fi
    done
done
