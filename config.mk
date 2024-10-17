TARGET_LIB := liblcu.a

BUILD_DIR := ./build
SOURCE_DIR := ./src
INCLUDE_DIR := ./src/inc
EXAMPLE_DIR := ./examples
EXAMPLE_EXEC := example

SOURCE_FILES := $(shell find $(SOURCE_DIR) -name '*.c')
OBJECT_FILES := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCE_FILES))
EXAMPLE_SOURCE := $(shell find $(EXAMPLE_DIR) -name '*.c')
EXAMPLE_OBJECT := $(patsubst $(EXAMPLE_DIR)/%.c, $(BUILD_DIR)/%.o, $(EXAMPLE_SOURCE))

INC_DIRS := $(shell find $(INCLUDE_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC := gcc
DEBUG_FLAGS := -g3 -fsanitize=address -fsanitize=leak
CFLAGS := -O3 -Wall -Wpedantic
CPPFLAGS := $(INC_FLAGS)
LDFLAGS := -lpthread

PREFIX := /usr/local
INSTALL_LIB := $(PREFIX)/lib
INSTALL_INCLUDE := $(PREFIX)/include/lcu

# Tests
TEST_DIR := ./tests
TEST_EXEC := test_exec

TEST_SOURCE := $(shell find $(TEST_DIR) -name '*.c')
TEST_OBJECT := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SOURCE))