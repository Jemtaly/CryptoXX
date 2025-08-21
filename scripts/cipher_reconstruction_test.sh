#!/usr/bin/bash

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <path to cipher binary>"
    exit 1
fi

# Test data of varying lengths - focus on block boundaries where padding is most critical
# Common cipher block sizes: AES/ARIA/Camellia=16, DES=8, Blowfish=8
test_lengths=(
    0 1 2 3 7 8 9 15 16 17 23 24 25 31 32 33
    47 48 49 63 64 65 79 80 81 95 96 97
    127 128 129 255 256 257 511 512 513
)

# Block cipher algorithms to test
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

# Key sizes for each algorithm
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

# IV sizes for each algorithm
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

# Modes to test - focus on modes that handle padding
modes_to_test=(
    "ECB"    # Electronic Codebook - shows padding behavior most clearly
    "CBC"    # Cipher Block Chaining - common mode with padding
    "PCBC"   # Propagating CBC - another padding mode
)

error_count=0
test_count=0

echo "=== Optimized Cipher Reconstruction Testing ==="
echo "Testing encrypt->decrypt roundtrip with varying input lengths"
echo "Focus on block boundary conditions where padding errors are most likely"
echo

# Create temporary directory for test data
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
    key_size="${keys[$i]}"
    civ_size="${civs[$i]}"
    
    echo "Testing algorithm: $alg (key=$key_size, iv=$civ_size)"
    
    # Generate fixed key and IV for this algorithm (deterministic for reproducibility)
    key=$(printf "%02x" $(seq 1 $key_size) | tr -d '\n')
    if [[ $civ_size -eq 0 ]]; then
        civ=""
    else
        civ=$(printf "%02x" $(seq 1 $civ_size) | tr -d '\n')
    fi
    
    for mode in "${modes_to_test[@]}"; do
        echo "  Mode: $mode"
        
        for len in "${test_lengths[@]}"; do
            ((test_count++))
            test_file="$test_data_dir/data_$len"
            encrypted_file="$test_data_dir/encrypted_${alg}_${mode}_$len"
            decrypted_file="$test_data_dir/decrypted_${alg}_${mode}_$len"
            
            echo -n "    Length $len bytes ... "
            
            # Test encryption  
            cat "$test_file" | "$@" "$alg" "${mode}Enc" "$key" "$civ" > "$encrypted_file" 2>/dev/null
            enc_exit_code=$?
            
            if [[ $enc_exit_code -ne 0 ]]; then
                echo "FAIL (encryption error)"
                ((error_count++))
                continue
            fi
            
            # Test decryption
            cat "$encrypted_file" | "$@" "$alg" "${mode}Dec" "$key" "$civ" > "$decrypted_file" 2>/dev/null
            dec_exit_code=$?
            
            if [[ $dec_exit_code -ne 0 ]]; then
                echo "FAIL (decryption error)"
                ((error_count++))
                continue
            fi
            
            # Compare original and decrypted data
            if cmp -s "$test_file" "$decrypted_file"; then
                echo "OK"
            else
                echo "FAIL (roundtrip mismatch)"
                echo "      Original size: $(stat -c%s "$test_file")"
                echo "      Encrypted size: $(stat -c%s "$encrypted_file")"
                echo "      Decrypted size: $(stat -c%s "$decrypted_file")"
                ((error_count++))
            fi
        done
    done
    echo
done

# Additional test: Stream cipher modes (CTR, OFB, CFB) which don't use padding
echo "Testing stream cipher modes (no padding)..."
stream_modes=("CTR" "OFB" "CFB")

for i in "${!algs[@]}"; do
    alg="${algs[$i]}"
    key_size="${keys[$i]}"
    civ_size="${civs[$i]}"
    
    # Only test a subset of algorithms for stream modes to keep test time reasonable
    if [[ "$alg" != "AES128" && "$alg" != "AES256" && "$alg" != "DES" ]]; then
        continue
    fi
    
    echo "Testing algorithm: $alg with stream modes"
    key=$(printf "%02x" $(seq 1 $key_size) | tr -d '\n')
    civ=$(printf "%02x" $(seq 1 $civ_size) | tr -d '\n')
    
    for mode in "${stream_modes[@]}"; do
        echo "  Mode: $mode"
        
        # Test a few specific lengths for stream modes
        for len in 0 1 15 16 17 31 32 33 64 128; do
            ((test_count++))
            test_file="$test_data_dir/data_$len"
            encrypted_file="$test_data_dir/encrypted_${alg}_${mode}_$len"
            decrypted_file="$test_data_dir/decrypted_${alg}_${mode}_$len"
            
            echo -n "    Length $len bytes ... "
            
            # Encrypt
            cat "$test_file" | "$@" "$alg" "${mode}Enc" "$key" "$civ" > "$encrypted_file" 2>/dev/null
            if [[ $? -ne 0 ]]; then
                echo "FAIL (encryption error)"
                ((error_count++))
                continue
            fi
            
            # Decrypt
            cat "$encrypted_file" | "$@" "$alg" "${mode}Dec" "$key" "$civ" > "$decrypted_file" 2>/dev/null
            if [[ $? -ne 0 ]]; then
                echo "FAIL (decryption error)"
                ((error_count++))
                continue
            fi
            
            # Compare
            if cmp -s "$test_file" "$decrypted_file"; then
                echo "OK"
            else
                echo "FAIL (roundtrip mismatch)"
                ((error_count++))
            fi
        done
    done
    echo
done

echo "=== Cipher Reconstruction Test Summary ==="
echo "Total tests: $test_count"
echo "Errors: $error_count"
echo "Success rate: $(( (test_count - error_count) * 100 / test_count ))%"

if [[ $error_count -ne 0 ]]; then
    echo "There were $error_count errors."
    exit 1
else
    echo "All tests passed successfully."
fi