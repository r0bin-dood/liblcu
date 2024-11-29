include config.mk

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -g3 $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET_LIB): $(OBJECT_FILES)
	ar rcs $@ $^

$(BUILD_DIR)/$(EXAMPLE_EXEC): $(BUILD_DIR)/$(TARGET_LIB) $(EXAMPLE_SOURCE)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(CPPFLAGS) $(EXAMPLE_SOURCE) -L$(BUILD_DIR) -l:$(TARGET_LIB) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(TEST_EXEC): $(BUILD_DIR)/$(TARGET_LIB) $(TEST_SOURCE)
	$(CC) -g3 $(CFLAGS) $(CPPFLAGS) $(TEST_SOURCE) -L$(BUILD_DIR) -l:$(TARGET_LIB) -lcunit -o $@ $(LDFLAGS)

all: $(BUILD_DIR)/$(TARGET_LIB) $(BUILD_DIR)/$(EXAMPLE_EXEC)

examples: $(BUILD_DIR)/$(EXAMPLE_EXEC) 

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_DIR)/out/*

install: $(BUILD_DIR)/$(TARGET_LIB)
	mkdir -p $(INSTALL_LIB)
	mkdir -p $(INSTALL_INCLUDE)
	cp $(BUILD_DIR)/$(TARGET_LIB) $(INSTALL_LIB)
	cp -r $(INCLUDE_DIR)/* $(INSTALL_INCLUDE)

uninstall:
	rm -rf $(INSTALL_LIB)/$(TARGET_LIB)
	rm -rf $(INSTALL_INCLUDE)

docs:
	doxygen Doxyfile

tests: $(BUILD_DIR)/$(TEST_EXEC)
	./$(BUILD_DIR)/$(TEST_EXEC)

clean_tests:
	rm -rf $(TEST_DIR)/out/*
	rm -rf $(BUILD_DIR)/$(MOCK_LIB) $(BUILD_DIR)/$(TEST_EXEC)