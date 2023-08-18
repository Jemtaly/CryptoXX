#!/bin/bash
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <path to cipher binary>"
    exit 1
fi
algs=(
    "AES128" "AES192" "AES256"
    "Twofish128" "Twofish192" "Twofish256"
    "Serpent128" "Serpent192" "Serpent256"
    "Camellia128" "Camellia192" "Camellia256"
    "SM4" "CAST128" "CAST256"
    "DES" "TDES2K" "TDES3K" "IDEA" "Blowfish"
)
keys=(
    16 24 32
    16 24 32
    16 24 32
    16 24 32
    16 16 32
    8 16 24 16 16
)
civs=(
    16 16 16
    16 16 16
    16 16 16
    16 16 16
    16 8 16
    8 8 8 8 8
)
modes=(
    "ECB" "CBC" "CFB"
    "OFB" "CTR" "PCBC"
)
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
    for mode in ${modes[@]}; do
        echo -n "Testing $alg $mode... "
        if [[ $mode == "ECB" ]]; then
            dec_hash=$(
                cat $tmp_file |
                    "$1" $alg $mode"Enc" $key |
                    "$1" $alg $mode"Dec" $key |
                    md5sum | cut -d' ' -f1
            )
        else
            dec_hash=$(
                cat $tmp_file |
                    "$1" $alg $mode"Enc" $key $civ |
                    "$1" $alg $mode"Dec" $key $civ |
                    md5sum | cut -d' ' -f1
            )
        fi
        if [[ $dec_hash == $tmp_hash ]]; then
            echo "OK"
        else
            echo "FAIL"
            exit 1
        fi
    done
done
