#!/usr/bin/bash

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <path to cipher binary>"
    exit 1
fi

# Test a few key algorithms with proper IV sizes
# Format: algorithm:key_size:iv_size
test_configs=(
    "AES128:16:16"
    "AES256:32:16"
    "DES:8:8"
)

# Test lengths focusing on block boundaries
test_lengths=(0 1 7 8 9 15 16 17 31 32 33 63 64 65)

# Modes to test
modes_to_test=("ECB" "CBC")

error_count=0
test_count=0

echo "=== Quick Cipher Reconstruction Test ==="

test_data_dir=$(mktemp -d)
trap "rm -rf $test_data_dir" EXIT

# Pre-generate test data
for len in "${test_lengths[@]}"; do
    if [[ $len -eq 0 ]]; then
        touch "$test_data_dir/data_$len"
    else
        head -c $len /dev/urandom > "$test_data_dir/data_$len"
    fi
done

for config in "${test_configs[@]}"; do
    IFS=':' read -r alg key_size iv_size <<< "$config"
    
    echo "Testing algorithm: $alg (key=$key_size, iv=$iv_size)"
    
    # Generate deterministic key and IV
    key=$(printf "%02x" $(seq 1 $key_size) | tr -d '\n')
    if [[ $iv_size -eq 0 ]]; then
        civ=""
    else
        civ=$(printf "%02x" $(seq 1 $iv_size) | tr -d '\n')
    fi
    
    for mode in "${modes_to_test[@]}"; do
        echo "  Mode: $mode"
        
        for len in "${test_lengths[@]}"; do
            ((test_count++))
            test_file="$test_data_dir/data_$len"
            encrypted_file="$test_data_dir/encrypted_${alg}_${mode}_$len"
            decrypted_file="$test_data_dir/decrypted_${alg}_${mode}_$len"
            
            echo -n "    Length $len bytes ... "
            
            # For ECB, always use empty IV
            if [[ "$mode" == "ECB" ]]; then
                test_civ=""
            else
                test_civ="$civ"
            fi
            
            # Test encryption  
            cat "$test_file" | "$@" "$alg" "${mode}Enc" "$key" "$test_civ" > "$encrypted_file" 2>/dev/null
            enc_exit_code=$?
            
            if [[ $enc_exit_code -ne 0 ]]; then
                echo "FAIL (encryption error)"
                ((error_count++))
                continue
            fi
            
            # Test decryption
            cat "$encrypted_file" | "$@" "$alg" "${mode}Dec" "$key" "$test_civ" > "$decrypted_file" 2>/dev/null
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
                ((error_count++))
            fi
        done
    done
    echo
done

echo "=== Quick Test Summary ==="
echo "Total tests: $test_count"
echo "Errors: $error_count"
if [[ $test_count -gt 0 ]]; then
    echo "Success rate: $(( (test_count - error_count) * 100 / test_count ))%"
fi

if [[ $error_count -ne 0 ]]; then
    echo "There were $error_count errors."
    exit 1
else
    echo "All tests passed successfully."
fi