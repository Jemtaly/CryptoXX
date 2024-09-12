CXX = g++
CXXFLAGS = -std=c++20 -O3 -I include
BUILD_DIR = build
SAMPLES_DIR = samples
SCRIPTS_DIR = scripts
HASH_SRC = $(SAMPLES_DIR)/hash.cpp
HASH_BIN = $(BUILD_DIR)/hash
HASH_TEST = $(SCRIPTS_DIR)/hash_test.sh
CIPHER_SRC = $(SAMPLES_DIR)/cipher.cpp
CIPHER_BIN = $(BUILD_DIR)/cipher
CIPHER_TEST = $(SCRIPTS_DIR)/cipher_test.sh
all: $(HASH_BIN) $(CIPHER_BIN)
$(HASH_BIN): $(HASH_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@
$(CIPHER_BIN): $(CIPHER_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@
test: $(HASH_TEST) $(CIPHER_TEST)
$(HASH_TEST): $(HASH_BIN)
	$(HASH_TEST) $(HASH_BIN)
$(CIPHER_TEST): $(CIPHER_BIN)
	$(CIPHER_TEST) $(CIPHER_BIN)
clean:
	rm -rf $(BUILD_DIR)
