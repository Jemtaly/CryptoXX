#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CryptoXX/hash.hpp"
#include "CryptoXX/hash/md5.hpp"
#include "CryptoXX/hash/sha256.hpp"
#include "CryptoXX/hash/blake3.hpp"

// Simple test framework
class SimpleTestRunner {
private:
    int total_tests = 0;
    int passed_tests = 0;
    std::string current_test = "";

public:
    void start_test(const std::string& name) {
        current_test = name;
        total_tests++;
        std::cout << "Running test: " << name << " ... ";
    }
    
    void assert_equal(const std::string& expected, const std::string& actual) {
        if (expected == actual) {
            passed_tests++;
            std::cout << "PASS" << std::endl;
        } else {
            std::cout << "FAIL" << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Actual:   " << actual << std::endl;
        }
    }
    
    void assert_equal(size_t expected, size_t actual) {
        if (expected == actual) {
            passed_tests++;
            std::cout << "PASS" << std::endl;
        } else {
            std::cout << "FAIL" << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Actual:   " << actual << std::endl;
        }
    }
    
    int summary() {
        std::cout << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "  Total:  " << total_tests << std::endl;
        std::cout << "  Passed: " << passed_tests << std::endl;
        std::cout << "  Failed: " << (total_tests - passed_tests) << std::endl;
        
        if (passed_tests == total_tests) {
            std::cout << "All tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << "Some tests failed!" << std::endl;
            return 1;
        }
    }
};

// Helper function to convert bytes to hex string
std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : bytes) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

// Helper function to convert array to hex string
template<size_t N>
std::string array_to_hex(const std::array<uint8_t, N>& arr) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : arr) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

void test_md5_empty() {
    SimpleTestRunner runner;
    
    runner.start_test("MD5 empty string");
    HashWrapper<MD5> hasher;
    auto result = hasher.digest();
    
    // MD5 of empty string: d41d8cd98f00b204e9800998ecf8427e
    std::string expected = "d41d8cd98f00b204e9800998ecf8427e";
    std::string actual = array_to_hex(result);
    
    runner.assert_equal(expected, actual);
}

void test_md5_hello() {
    SimpleTestRunner runner;
    
    runner.start_test("MD5 'hello'");
    HashWrapper<MD5> hasher;
    std::string input = "hello";
    hasher.update(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    auto result = hasher.digest();
    
    // MD5 of "hello": 5d41402abc4b2a76b9719d911017c592
    std::string expected = "5d41402abc4b2a76b9719d911017c592";
    std::string actual = array_to_hex(result);
    
    runner.assert_equal(expected, actual);
}

void test_sha256_empty() {
    SimpleTestRunner runner;
    
    runner.start_test("SHA256 empty string");
    HashWrapper<SHA256> hasher;
    auto result = hasher.digest();
    
    // SHA256 of empty string: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    std::string expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    std::string actual = array_to_hex(result);
    
    runner.assert_equal(expected, actual);
}

void test_sha256_hello() {
    SimpleTestRunner runner;
    
    runner.start_test("SHA256 'hello'");
    HashWrapper<SHA256> hasher;
    std::string input = "hello";
    hasher.update(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    auto result = hasher.digest();
    
    // SHA256 of "hello": 2cf24dba4f21d4288094e9b265a7dc1c3e3fef30b2a73a24de3a4dd7a096d0f8
    std::string expected = "2cf24dba4f21d4288094e9b265a7dc1c3e3fef30b2a73a24de3a4dd7a096d0f8";
    std::string actual = array_to_hex(result);
    
    runner.assert_equal(expected, actual);
}

void test_blake3_empty() {
    SimpleTestRunner runner;
    
    runner.start_test("BLAKE3 empty string");
    HashWrapper<BLAKE3> hasher;
    auto result = hasher.digest();
    
    // BLAKE3 of empty string: af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262
    std::string expected = "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262";
    std::string actual = array_to_hex(result);
    
    runner.assert_equal(expected, actual);
}

void test_large_input() {
    SimpleTestRunner runner;
    
    runner.start_test("Large input performance test");
    
    // Create a large input buffer (1MB)
    std::vector<uint8_t> large_input(1024 * 1024, 0x42);
    
    // Test MD5
    HashWrapper<MD5> md5_hasher;
    md5_hasher.update(large_input.data(), large_input.size());
    auto md5_result = md5_hasher.digest();
    
    // Test SHA256
    HashWrapper<SHA256> sha256_hasher;
    sha256_hasher.update(large_input.data(), large_input.size());
    auto sha256_result = sha256_hasher.digest();
    
    // Test BLAKE3
    HashWrapper<BLAKE3> blake3_hasher;
    blake3_hasher.update(large_input.data(), large_input.size());
    auto blake3_result = blake3_hasher.digest();
    
    // Verify sizes
    runner.assert_equal(size_t(16), md5_result.size());   // MD5 is 128-bit
    runner.assert_equal(size_t(32), sha256_result.size()); // SHA256 is 256-bit
    runner.assert_equal(size_t(32), blake3_result.size()); // BLAKE3 is 256-bit by default
}

void test_incremental_hashing() {
    SimpleTestRunner runner;
    
    runner.start_test("Incremental hashing test");
    
    // Hash "hello" + "world" in parts
    HashWrapper<SHA256> hasher1;
    std::string part1 = "hello";
    std::string part2 = "world";
    
    hasher1.update(reinterpret_cast<const uint8_t*>(part1.data()), part1.size());
    hasher1.update(reinterpret_cast<const uint8_t*>(part2.data()), part2.size());
    auto result1 = hasher1.digest();
    
    // Hash "helloworld" all at once
    HashWrapper<SHA256> hasher2;
    std::string combined = "helloworld";
    hasher2.update(reinterpret_cast<const uint8_t*>(combined.data()), combined.size());
    auto result2 = hasher2.digest();
    
    // Results should be identical
    std::string hex1 = array_to_hex(result1);
    std::string hex2 = array_to_hex(result2);
    
    runner.assert_equal(hex1, hex2);
}

int main() {
    std::cout << "=== CryptoXX Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    int total_failures = 0;
    
    try {
        test_md5_empty();
        test_md5_hello();
        test_sha256_empty();
        test_sha256_hello();
        test_blake3_empty();
        test_large_input();
        test_incremental_hashing();
        
        std::cout << std::endl;
        std::cout << "=== All Unit Tests Completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        total_failures++;
    }
    
    return total_failures;
}