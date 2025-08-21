#!/usr/bin/bash

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <path to hash binary>"
    exit 1
fi

# Test data of varying lengths - focus on block boundaries where padding is most critical
# Common hash block sizes: MD5/SHA1=64, SHA256=64, SHA512=128, SHA3 varies
test_lengths=(
    0 1 2 3 7 8 15 16 17 31 32 33 47 48 49 
    55 56 57 63 64 65 71 72 73 79 80 81 
    95 96 97 111 112 113 127 128 129 135 136 137
    200 255 256 257 511 512 513 1023 1024 1025
)

# Hash algorithms to test
algs=(
    "MD5" "SHA1" "SHA0"
    "SHA224" "SHA256"
    "SHA384" "SHA512"
    "SHA3-224" "SHA3-256"
    "SHA3-384" "SHA3-512"
    "SHAKE128" "SHAKE256"
    "SM3" "Whirlpool"
    "BLAKE2b512" "BLAKE2b384"
    "BLAKE2s256" "BLAKE2s224"
    "BLAKE3" "CRC32" "CRC64"
)

# Corresponding OpenSSL algorithms (empty string means not supported)
ssls=(
    "md5" "sha1" ""
    "sha224" "sha256"
    "sha384" "sha512"
    "sha3-224" "sha3-256"
    "sha3-384" "sha3-512"
    "shake128" "shake256"
    "sm3" "whirlpool"
    "blake2b512" ""
    "blake2s256" ""
    "" "" ""
)

error_count=0
test_count=0

echo "=== Optimized Hash Reconstruction Testing ==="
echo "Testing varying input lengths to validate padding behavior"
echo "Focus on block boundary conditions where errors are most likely"
echo

# Create temporary files for test data
test_data_dir=$(mktemp -d)
trap "rm -rf $test_data_dir" EXIT

# Pre-generate test data of different lengths
for len in "${test_lengths[@]}"; do
    if [[ $len -eq 0 ]]; then
        touch "$test_data_dir/data_$len"
    else
        head -c $len /dev/urandom > "$test_data_dir/data_$len"
    fi
done

for i in "${!algs[@]}"; do
    alg="${algs[$i]}"
    ssl="${ssls[$i]}"
    
    echo "Testing algorithm: $alg"
    
    for len in "${test_lengths[@]}"; do
        ((test_count++))
        test_file="$test_data_dir/data_$len"
        
        echo -n "  Length $len bytes ... "
        
        # Test with CryptoXX
        alg_hash=$(cat "$test_file" | "$@" "$alg" 2>/dev/null)
        if [[ $? -ne 0 ]]; then
            echo "FAIL (CryptoXX error)"
            ((error_count++))
            continue
        fi
        
        # If OpenSSL supports this algorithm, compare results
        if [[ $ssl != "" ]]; then
            if echo "test" | openssl dgst -$ssl >/dev/null 2>&1; then
                ssl_hash=$(cat "$test_file" | openssl dgst -$ssl 2>/dev/null | cut -d' ' -f2)
                if [[ $? -ne 0 ]]; then
                    echo "FAIL (OpenSSL error)"
                    ((error_count++))
                    continue
                fi
                
                # Compare hashes (handle case where CryptoXX might output longer hash for SHAKE)
                if [[ ${alg_hash:0:${#ssl_hash}} == $ssl_hash ]]; then
                    echo "OK"
                else
                    echo "FAIL"
                    echo "    Expected: $ssl_hash"
                    echo "    Got:      $alg_hash"
                    ((error_count++))
                fi
            else
                echo "OK (no OpenSSL comparison)"
            fi
        else
            # For algorithms not supported by OpenSSL, just verify we got a hash
            if [[ -n "$alg_hash" ]]; then
                echo "OK (no OpenSSL comparison)"
            else
                echo "FAIL (empty hash)"
                ((error_count++))
            fi
        fi
    done
    echo
done

echo "=== Hash Reconstruction Test Summary ==="
echo "Total tests: $test_count"
echo "Errors: $error_count"
echo "Success rate: $(( (test_count - error_count) * 100 / test_count ))%"

if [[ $error_count -ne 0 ]]; then
    echo "There were $error_count errors."
    exit 1
else
    echo "All tests passed successfully."
fi