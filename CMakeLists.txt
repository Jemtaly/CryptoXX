cmake_minimum_required(VERSION 3.16)

project(CryptoXX 
    VERSION 1.0.0
    DESCRIPTION "A modern, high-speed, easy-to-use object-oriented C++ cryptographic library"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable export of compile commands for IDEs and static analysis tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type specific configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Common flags for GCC and Clang
    add_compile_options(-Wall -Wextra)
    
    # Debug build
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    
    # Release build
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    
    # Release with debug info
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    
    # Minimum size release
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
endif()

# Include directories
include_directories(include)

# Find dependencies
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(OPENSSL openssl)
endif()

# Option to enable/disable tests
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_BENCHMARKS "Build benchmark executables" OFF)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan in Debug builds" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis with clang-tidy" OFF)

# Sanitizers for debug builds
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Static analysis with clang-tidy
if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found!")
    endif()
endif()

# Find all header files for dependency tracking
file(GLOB_RECURSE HEADERS "include/*.hpp")

# Create header-only library target for easier dependency management
add_library(cryptoxx INTERFACE)
target_include_directories(cryptoxx INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(cryptoxx INTERFACE cxx_std_20)

# Build sample executables
add_executable(hash samples/hash.cpp)
target_link_libraries(hash PRIVATE cryptoxx)

add_executable(cipher samples/cipher.cpp)
target_link_libraries(cipher PRIVATE cryptoxx)

# Set output directory
set_target_properties(hash cipher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Include modern C++ testing framework
    include(cmake/Testing.cmake)
    
    # Custom test targets for shell script tests
    add_custom_target(test-hash
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/hash_test.sh ${CMAKE_BINARY_DIR}/bin/hash
        DEPENDS hash
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running hash algorithm tests"
    )
    
    add_custom_target(test-cipher
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cipher_test.sh ${CMAKE_BINARY_DIR}/bin/cipher
        DEPENDS cipher
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cipher algorithm tests"
    )
    
    add_custom_target(test-shell
        DEPENDS test-hash test-cipher
        COMMENT "Running shell script tests"
    )
    
    add_custom_target(test-all
        DEPENDS test-shell
        COMMENT "Running all tests"
    )
    
    # Add unit tests to test-all if they exist
    if(TARGET unit_tests)
        add_dependencies(test-all unit_tests)
    endif()
endif()

# Code formatting
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE FORMAT_SOURCES 
        "samples/*.cpp" 
        "include/*.hpp"
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${FORMAT_SOURCES}
        COMMENT "Formatting code with clang-format"
    )
    
    add_custom_target(check-format
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${FORMAT_SOURCES}
        COMMENT "Checking code formatting"
    )
endif()

# Installation
install(TARGETS hash cipher
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

install(TARGETS cryptoxx
    EXPORT CryptoXXTargets
    INCLUDES DESTINATION include
)

install(EXPORT CryptoXXTargets
    FILE CryptoXXTargets.cmake
    NAMESPACE CryptoXX::
    DESTINATION lib/cmake/CryptoXX
)

# Generate config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "CryptoXXConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CryptoXXConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CryptoXXConfig.cmake"
    INSTALL_DESTINATION lib/cmake/CryptoXX
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CryptoXXConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CryptoXXConfigVersion.cmake"
    DESTINATION lib/cmake/CryptoXX
)

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "CryptoXX ${PROJECT_VERSION} Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Enable sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "  Enable static analysis: ${ENABLE_STATIC_ANALYSIS}")
if(OPENSSL_FOUND)
    message(STATUS "  OpenSSL version: ${OPENSSL_VERSION}")
endif()
message(STATUS "")