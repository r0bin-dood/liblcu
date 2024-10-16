include config.mk

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule to build the static library
$(BUILD_DIR)/$(TARGET_LIB): $(OBJECT_FILES)
	ar rcs $@ $^

$(BUILD_DIR)/$(EXAMPLE_EXEC): $(BUILD_DIR)/$(TARGET_LIB) $(EXAMPLE_SOURCE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(EXAMPLE_SOURCE) -L$(BUILD_DIR) -l:$(TARGET_LIB) -o $@ $(LDFLAGS)

all: $(BUILD_DIR)/$(TARGET_LIB) $(BUILD_DIR)/$(EXAMPLE_EXEC)

examples: $(BUILD_DIR)/$(EXAMPLE_EXEC) 

clean:
	rm -rf $(BUILD_DIR)

install: $(BUILD_DIR)/$(TARGET_LIB)
	mkdir -p $(INSTALL_LIB)
	mkdir -p $(INSTALL_INCLUDE)
	cp $(BUILD_DIR)/$(TARGET_LIB) $(INSTALL_LIB)
	cp -r $(INCLUDE_DIR)/* $(INSTALL_INCLUDE)

uninstall:
	rm -rf $(INSTALL_LIB)/$(TARGET_LIB)
	rm -rf $(INSTALL_INCLUDE)