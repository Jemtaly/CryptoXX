cmake_minimum_required(VERSION 3.15)

project(CryptoXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/samples")
set(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

add_executable(hash ${SAMPLES_DIR}/hash.cpp)

target_include_directories(hash PUBLIC ${HEADER_DIR})

add_executable(cipher ${SAMPLES_DIR}/cipher.cpp)

target_include_directories(cipher PUBLIC ${HEADER_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

enable_testing()

add_test(
    NAME hash_test
    COMMAND ${SCRIPTS_DIR}/hash_test.sh $<TARGET_FILE:hash>
)

add_test(
    NAME cipher_test
    COMMAND ${SCRIPTS_DIR}/cipher_test.sh $<TARGET_FILE:cipher>
)
