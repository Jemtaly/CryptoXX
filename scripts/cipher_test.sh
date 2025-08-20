#!/usr/bin/bash

# Enhanced cipher test script with better error handling and reporting
set -euo pipefail  # Exit on error, undefined variables, and pipe failures

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_FILE_SIZE=${TEST_FILE_SIZE:-123456789}
VERBOSE=${VERBOSE:-0}

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $*"
}

# Usage function
usage() {
    cat << EOF
Usage: $0 <path to cipher binary> [options]

Options:
    -v, --verbose    Enable verbose output
    -s, --size SIZE  Test file size in bytes (default: $TEST_FILE_SIZE)
    -h, --help       Show this help message

Environment Variables:
    TEST_FILE_SIZE   Override default test file size
    VERBOSE          Enable verbose output (0 or 1)
EOF
}

# Parse command line arguments
parse_args() {
    if [[ $# -lt 1 ]]; then
        usage
        exit 1
    fi
    
    CIPHER_BINARY="$1"
    shift
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -s|--size)
                TEST_FILE_SIZE="$2"
                shift 2
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done
    
    # Validate cipher binary
    if [[ ! -x "$CIPHER_BINARY" ]]; then
        log_error "Cipher binary not found or not executable: $CIPHER_BINARY"
        exit 1
    fi
}

# Test data
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

# Main test function
run_tests() {
    local total_tests=0
    local passed_tests=0
    local skipped_tests=0
    local failed_tests=0
    
    log_info "Starting cipher algorithm tests..."
    log_info "Test file size: $TEST_FILE_SIZE bytes"
    log_info "Cipher binary: $CIPHER_BINARY"
    
    # Create temporary test file
    local big_file
    big_file=$(mktemp)
    trap "rm -f $big_file" EXIT
    
    log_info "Generating test data..."
    if ! head -c "$TEST_FILE_SIZE" /dev/urandom > "$big_file"; then
        log_error "Failed to generate test data"
        exit 1
    fi
    
    # Run tests for each algorithm
    for i in "${!algs[@]}"; do
        local alg="${algs[$i]}"
        local ssl="${ssls[$i]}"
        local key_len="${keys[$i]}"
        local civ_len="${civs[$i]}"
        
        # Generate random key and IV
        local key
        local civ
        key=$(head -c "$key_len" /dev/urandom | od -An -tx1 | tr -d ' \n')
        civ=$(head -c "$civ_len" /dev/urandom | od -An -tx1 | tr -d ' \n')
        
        total_tests=$((total_tests + 1))
        
        if [[ $ssl != "" ]]; then
            # Test with OpenSSL comparison
            if test_with_openssl "$alg" "$ssl" "$key" "$big_file"; then
                passed_tests=$((passed_tests + 1))
            else
                failed_tests=$((failed_tests + 1))
            fi
        else
            # Test without OpenSSL comparison
            if test_without_openssl "$alg" "$key" "$big_file"; then
                passed_tests=$((passed_tests + 1))
            else
                failed_tests=$((failed_tests + 1))
            fi
        fi
    done
    
    # Print summary
    echo
    log_info "Test Summary:"
    log_info "  Total:   $total_tests"
    log_success "  Passed:  $passed_tests"
    log_warning "  Skipped: $skipped_tests"
    if [[ $failed_tests -gt 0 ]]; then
        log_error "  Failed:  $failed_tests"
        exit 1
    else
        log_success "All tests passed!"
    fi
}

# Test algorithm with OpenSSL comparison
test_with_openssl() {
    local alg="$1"
    local ssl="$2"
    local key="$3"
    local test_file="$4"
    
    [[ $VERBOSE -eq 1 ]] && log_info "Testing $alg with OpenSSL comparison..."
    
    # Check if OpenSSL supports this cipher
    if ! echo "test" | openssl enc -"$ssl"-ecb -K "$key" >/dev/null 2>&1; then
        log_warning "$alg: OpenSSL doesn't support $ssl-ecb (skipped)"
        return 0
    fi
    
    # Get our implementation result
    local enc_start enc_end enc_time enc_hash
    enc_start=$(date +%s.%N)
    if ! enc_hash=$("$CIPHER_BINARY" "$alg" "ECBEnc" "$key" < "$test_file" | md5sum | cut -d' ' -f1); then
        log_error "$alg: Cipher binary test failed"
        return 1
    fi
    enc_end=$(date +%s.%N)
    enc_time=$(echo "$enc_end - $enc_start" | bc -l)
    
    # Get OpenSSL result
    local ssl_start ssl_end ssl_time ssl_hash
    ssl_start=$(date +%s.%N)
    if ! ssl_hash=$(openssl enc -"$ssl"-ecb -K "$key" < "$test_file" | md5sum | cut -d' ' -f1); then
        log_error "$alg: OpenSSL test failed"
        return 1
    fi
    ssl_end=$(date +%s.%N)
    ssl_time=$(echo "$ssl_end - $ssl_start" | bc -l)
    
    # Compare results
    if [[ "$enc_hash" == "$ssl_hash" ]]; then
        log_success "$alg: OK (${enc_time}s vs ${ssl_time}s)"
        return 0
    else
        log_error "$alg: Hash mismatch!"
        [[ $VERBOSE -eq 1 ]] && log_error "  Expected: $ssl_hash"
        [[ $VERBOSE -eq 1 ]] && log_error "  Got:      $enc_hash"
        return 1
    fi
}

# Test algorithm without OpenSSL comparison
test_without_openssl() {
    local alg="$1"
    local key="$2"
    local test_file="$3"
    
    [[ $VERBOSE -eq 1 ]] && log_info "Testing $alg (no OpenSSL comparison)..."
    
    local enc_start enc_end enc_time enc_hash
    enc_start=$(date +%s.%N)
    if ! enc_hash=$("$CIPHER_BINARY" "$alg" "ECBEnc" "$key" < "$test_file" | md5sum | cut -d' ' -f1); then
        log_error "$alg: Cipher binary test failed"
        return 1
    fi
    enc_end=$(date +%s.%N)
    enc_time=$(echo "$enc_end - $enc_start" | bc -l)
    
    log_success "$alg: OK (${enc_time}s)"
    return 0
}

# Main execution
main() {
    parse_args "$@"
    run_tests
}

# Run main function if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
