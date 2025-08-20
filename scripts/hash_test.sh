#!/usr/bin/bash

# Enhanced hash test script with better error handling and reporting
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
Usage: $0 <path to hash binary> [options]

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
    
    HASH_BINARY="$1"
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
    
    # Validate hash binary
    if [[ ! -x "$HASH_BINARY" ]]; then
        log_error "Hash binary not found or not executable: $HASH_BINARY"
        exit 1
    fi
}

# Test data
algs=(
    "SM3" "MD5"
    "Whirlpool" "SHA1" "SHA0"
    "SHA224" "SHA256"
    "SHA384" "SHA512"
    "SHA3-224" "SHA3-256"
    "SHA3-384" "SHA3-512"
    "SHAKE128" "SHAKE256"
    "BLAKE2b512" "BLAKE2b384"
    "BLAKE2s256" "BLAKE2s224"
    "BLAKE3" "CRC32" "CRC64"
)

ssls=(
    "sm3" "md5"
    "whirlpool" "sha1" ""
    "sha224" "sha256"
    "sha384" "sha512"
    "sha3-224" "sha3-256"
    "sha3-384" "sha3-512"
    "shake128" "shake256"
    "blake2b512" ""
    "blake2s256" ""
    "" "" ""
)

# Main test function
run_tests() {
    local total_tests=0
    local passed_tests=0
    local skipped_tests=0
    local failed_tests=0
    
    log_info "Starting hash algorithm tests..."
    log_info "Test file size: $TEST_FILE_SIZE bytes"
    log_info "Hash binary: $HASH_BINARY"
    
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
        total_tests=$((total_tests + 1))
        
        if [[ $ssl != "" ]]; then
            # Test with OpenSSL comparison
            if test_with_openssl "$alg" "$ssl" "$big_file"; then
                passed_tests=$((passed_tests + 1))
            else
                failed_tests=$((failed_tests + 1))
            fi
        else
            # Test without OpenSSL comparison
            if test_without_openssl "$alg" "$big_file"; then
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
    local test_file="$3"
    
    [[ $VERBOSE -eq 1 ]] && log_info "Testing $alg with OpenSSL comparison..."
    
    # Check if OpenSSL supports this algorithm
    if ! echo "test" | openssl dgst -"$ssl" >/dev/null 2>&1; then
        log_warning "$alg: OpenSSL doesn't support $ssl (skipped)"
        return 0
    fi
    
    # Get OpenSSL result
    local ssl_start ssl_end ssl_time ssl_hash
    ssl_start=$(date +%s.%N)
    if ! ssl_hash=$(openssl dgst -"$ssl" < "$test_file" | cut -d' ' -f2); then
        log_error "$alg: OpenSSL test failed"
        return 1
    fi
    ssl_end=$(date +%s.%N)
    ssl_time=$(echo "$ssl_end - $ssl_start" | bc -l)
    
    # Get our implementation result
    local alg_start alg_end alg_time alg_hash
    alg_start=$(date +%s.%N)
    if ! alg_hash=$("$HASH_BINARY" "$alg" < "$test_file"); then
        log_error "$alg: Hash binary test failed"
        return 1
    fi
    alg_end=$(date +%s.%N)
    alg_time=$(echo "$alg_end - $alg_start" | bc -l)
    
    # Compare results (handle variable length hashes like SHAKE)
    local ssl_len=${#ssl_hash}
    local alg_prefix=${alg_hash:0:$ssl_len}
    
    if [[ "$alg_prefix" == "$ssl_hash" ]]; then
        log_success "$alg: OK (${alg_time}s vs ${ssl_time}s)"
        return 0
    else
        log_error "$alg: Hash mismatch!"
        [[ $VERBOSE -eq 1 ]] && log_error "  Expected: $ssl_hash"
        [[ $VERBOSE -eq 1 ]] && log_error "  Got:      $alg_hash"
        return 1
    fi
}

# Test algorithm without OpenSSL comparison
test_without_openssl() {
    local alg="$1"
    local test_file="$2"
    
    [[ $VERBOSE -eq 1 ]] && log_info "Testing $alg (no OpenSSL comparison)..."
    
    local alg_start alg_end alg_time alg_hash
    alg_start=$(date +%s.%N)
    if ! alg_hash=$("$HASH_BINARY" "$alg" < "$test_file"); then
        log_error "$alg: Hash binary test failed"
        return 1
    fi
    alg_end=$(date +%s.%N)
    alg_time=$(echo "$alg_end - $alg_start" | bc -l)
    
    log_success "$alg: OK (${alg_time}s)"
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
