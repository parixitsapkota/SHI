# --- Configuration ---
CC      := clang
CFLAGS  := -fsanitize=address -g -O0 -Wall -Wextra
BUILD   := build

HEADERS := $(wildcard *.h)
TESTS   := $(patsubst %.h, $(BUILD)/%, $(HEADERS))

.PHONY: all clean format prepare test

all: prepare format test

prepare: | $(BUILD)

$(BUILD):
	@mkdir -p $@

$(BUILD)/%: %.h
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) -x c -DSHI_TEST $< -o $@

test: $(TESTS)
	@echo "--- Running Tests ---"
	@for test in $(TESTS); do \
		echo -n "Running $$test -> "; \
		output=$$($$test 2>&1); \
		if [ $$? -ne 0 ]; then \
			echo "FAILED"; \
			echo "#==============================#"; \
			echo "$$output"; \
			echo "#==============================#"; \
			exit 1; \
		else \
			echo "PASSED"; \
		fi; \
	done

# Clean build
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD)

# Format all header files
format:
	@echo "Formatting headers..."
	@clang-format -i $(HEADERS)
