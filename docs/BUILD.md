# Build System Documentation

This document describes the modern build and test infrastructure for CryptoXX.

## Overview

CryptoXX now provides two modern build systems:

1. **Enhanced Makefile** - Traditional GNU Make with modern features
2. **CMake** - Cross-platform build system with advanced configuration

## Enhanced Makefile

### Build Types

```bash
make                    # Default Release build
make release           # Optimized release build
make debug             # Debug build with symbols
make relwithdebinfo    # Release with debug info
make minsizerel        # Size-optimized build
```

### Build Configuration

The Makefile supports multiple build types with different compiler flags:

- **Debug**: `-g -O0 -DDEBUG -Wall -Wextra -Wpedantic`
- **Release**: `-O3 -DNDEBUG -Wall` (default)
- **RelWithDebInfo**: `-g -O2 -DNDEBUG -Wall -Wextra`
- **MinSizeRel**: `-Os -DNDEBUG -Wall`

### Testing

```bash
make test              # Run all tests
make test-hash         # Run hash tests only
make test-cipher       # Run cipher tests only
```

### Code Quality

```bash
make format            # Format code with clang-format
make check-format      # Check code formatting
make lint              # Run static analysis with clang-tidy
```

### Other Targets

```bash
make help              # Show available targets
make clean             # Remove build directory
make install           # Install binaries to system
```

### Environment Variables

- `CXX`: Override compiler (default: g++)
- `BUILD_TYPE`: Override build type
- `MAKEFLAGS`: Parallel build options (auto-detected)

## CMake Build System

### Quick Start

```bash
# Configure and build
cmake -B build
cmake --build build --parallel

# Alternative: specify build type
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Build Options

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DENABLE_SANITIZERS=ON \
    -DENABLE_STATIC_ANALYSIS=ON
```

### Available Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_TESTS` | Build test executables | ON |
| `BUILD_BENCHMARKS` | Build benchmark executables | OFF |
| `ENABLE_SANITIZERS` | Enable AddressSanitizer/UBSan in Debug | OFF |
| `ENABLE_STATIC_ANALYSIS` | Enable clang-tidy integration | OFF |

### Testing with CMake

```bash
cd build

# Run individual test suites
make test-hash
make test-cipher
make test-shell

# Run all tests
make test-all

# Run with CTest
ctest --verbose
```

### Installation

```bash
cmake --build build --target install

# Or with custom prefix
cmake -B build -DCMAKE_INSTALL_PREFIX=/opt/cryptoxx
cmake --build build --target install
```

## Enhanced Test Scripts

Both build systems use enhanced test scripts with better error handling and reporting.

### Features

- **Colored output** with clear pass/fail indicators
- **Performance timing** comparisons with OpenSSL
- **Verbose mode** for detailed debugging
- **Configurable test file sizes**
- **Comprehensive error handling** and exit codes
- **Test summaries** with statistics

### Usage

```bash
# Basic usage
./scripts/hash_test.sh build/hash
./scripts/cipher_test.sh build/cipher

# Verbose mode
./scripts/hash_test.sh build/hash --verbose

# Custom test file size
./scripts/cipher_test.sh build/cipher --size 50000000

# Show help
./scripts/hash_test.sh --help
```

### Environment Variables

- `TEST_FILE_SIZE`: Override default test file size (123456789 bytes)
- `VERBOSE`: Enable verbose output (0 or 1)

## CI/CD Pipeline

The enhanced GitHub Actions workflow includes:

### Build Matrix

- **Compilers**: GCC, Clang
- **Build Types**: Release, Debug
- **Build Systems**: Make, CMake
- **Special Configurations**: Sanitizers

### Code Quality Checks

- **Code formatting** verification with clang-format
- **Static analysis** with cppcheck and clang-tidy
- **Build caching** for faster CI runs

### Performance Testing

- **Benchmarking** with large test files
- **Performance comparisons** between implementations

### Artifact Management

- **Build artifacts** uploaded for Release builds
- **Test results** and logs preserved

## Cross-Platform Support

### Dependencies

**Required:**
- C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.16+ (for CMake builds)
- GNU Make (for Makefile builds)

**Optional:**
- OpenSSL (for cross-validation)
- clang-format (for code formatting)
- clang-tidy (for static analysis)
- cppcheck (for additional static analysis)

### Platform-Specific Notes

**Linux:**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake clang-format clang-tidy

# Install dependencies (RHEL/CentOS)
sudo yum install gcc-c++ cmake clang-tools-extra
```

**macOS:**
```bash
# Install dependencies
brew install cmake clang-format
```

**Windows:**
```bash
# Use Visual Studio or MSYS2/MinGW
# CMake provides Visual Studio project generation
cmake -B build -G "Visual Studio 16 2019"
```

## Integration with IDEs

### VS Code

```json
{
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.configureSettings": {
    "BUILD_TESTS": "ON"
  }
}
```

### CLion

CLion automatically detects CMakeLists.txt and provides full integration.

### Visual Studio

```bash
cmake -B build -G "Visual Studio 16 2019"
```

## Performance Optimization

### Build Performance

- **Parallel builds** enabled by default
- **ccache support** in CI/CD
- **Incremental builds** with proper dependency tracking

### Runtime Performance

- **Optimized release builds** with `-O3`
- **Link-time optimization** available
- **CPU-specific optimizations** with `-march=native`

## Troubleshooting

### Common Issues

1. **Missing dependencies**: Install required packages
2. **Compilation warnings**: Use Debug build to see all warnings
3. **Test failures**: Check OpenSSL version compatibility
4. **Permission errors**: Ensure scripts are executable

### Debug Build

```bash
make debug
# or
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

### Sanitizer Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
```

## Contributing

When contributing to the build system:

1. **Test both build systems** (Make and CMake)
2. **Run code formatting** before committing
3. **Update documentation** for new features
4. **Ensure CI passes** on all configurations

### Code Formatting

```bash
make format
# or
cmake --build build --target format
```

### Static Analysis

```bash
make lint
# or
cmake -B build -DENABLE_STATIC_ANALYSIS=ON
cmake --build build
```