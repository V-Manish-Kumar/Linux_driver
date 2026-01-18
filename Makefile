# Kernel module source
SRC_DIR := src
TOOLS_DIR := tools
BUILD_DIR := build

# Kernel module
obj-m += queue_driver.o

# User-space tools
TOOLS := configurator filler reader job_submitter worker

# Default target
all: module tools

# Build kernel module
module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/$(SRC_DIR) modules
	@mkdir -p $(BUILD_DIR)
	@cp $(SRC_DIR)/*.ko $(BUILD_DIR)/ 2>/dev/null || true
	@cp $(SRC_DIR)/*.o $(BUILD_DIR)/ 2>/dev/null || true
	@cp $(SRC_DIR)/*.mod* $(BUILD_DIR)/ 2>/dev/null || true
	@cp $(SRC_DIR)/Module.symvers $(BUILD_DIR)/ 2>/dev/null || true
	@cp $(SRC_DIR)/modules.order $(BUILD_DIR)/ 2>/dev/null || true

# Build user-space tools
tools:
	@mkdir -p $(BUILD_DIR)
	@for tool in $(TOOLS); do \
		echo "Building $$tool..."; \
		gcc -o $(BUILD_DIR)/$$tool $(TOOLS_DIR)/$$tool.c; \
	done
	@echo "All tools built successfully!"

# Install module
install: module
	sudo insmod $(BUILD_DIR)/queue_driver.ko
	sudo chmod 666 /dev/queue_driver
	@echo "Module loaded. Device: /dev/queue_driver"

# Uninstall module
uninstall:
	sudo rmmod queue_driver || true
	@echo "Module unloaded."

# Clean build artifacts
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/$(SRC_DIR) clean
	rm -rf $(BUILD_DIR)/*
	@echo "Cleaned build directory."

# Show module info
info:
	@echo "=== Module Status ==="
	@lsmod | grep queue_driver || echo "Module not loaded"
	@echo ""
	@echo "=== Device File ==="
	@ls -l /dev/queue_driver 2>/dev/null || echo "Device not found"
	@echo ""
	@echo "=== Statistics ==="
	@cat /proc/queue_driver_stats 2>/dev/null || echo "Stats not available"

# Show help
help:
	@echo "Available targets:"
	@echo "  make all       - Build kernel module and user-space tools (default)"
	@echo "  make module    - Build only kernel module"
	@echo "  make tools     - Build only user-space tools"
	@echo "  make install   - Build and install kernel module"
	@echo "  make uninstall - Remove kernel module"
	@echo "  make clean     - Remove all build artifacts"
	@echo "  make info      - Show module status and statistics"
	@echo "  make help      - Show this help message"
	@echo ""
	@echo "Project structure:"
	@echo "  src/           - Kernel module source code"
	@echo "  tools/         - User-space tools source code"
	@echo "  build/         - Compiled binaries and build artifacts"

.PHONY: all module tools install uninstall clean info help

