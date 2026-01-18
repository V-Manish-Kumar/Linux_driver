#!/bin/bash
# Build and setup script for Queue Driver

set -e  # Exit on error

echo "========================================"
echo "  Queue Driver Build & Setup Script"
echo "========================================"
echo ""

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Warning: This script is designed for Linux systems."
    echo "You appear to be on: $OSTYPE"
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Step 1: Clean previous builds
echo "[1/6] Cleaning previous builds..."
make clean 2>/dev/null || true
echo "✓ Clean complete"
echo ""

# Step 2: Build kernel module and tools
echo "[2/6] Building kernel module and tools..."
make all
if [ $? -ne 0 ]; then
    echo "✗ Build failed!"
    exit 1
fi
echo "✓ Build complete"
echo ""

# Step 3: Load kernel module
echo "[3/6] Loading kernel module..."
if lsmod | grep -q queue_driver; then
    echo "Module already loaded. Reloading..."
    sudo rmmod queue_driver 2>/dev/null || true
fi
sudo insmod build/queue_driver.ko
if [ $? -ne 0 ]; then
    echo "✗ Failed to load module!"
    echo "Check dmesg for errors:"
    dmesg | tail -10
    exit 1
fi
echo "✓ Module loaded"
echo ""

# Step 4: Set permissions
echo "[4/6] Setting device permissions..."
sleep 1  # Give udev time to create device
sudo chmod 666 /dev/queue_driver
echo "✓ Permissions set"
echo ""

# Step 5: Verify installation
echo "[5/6] Verifying installation..."
if [ ! -c /dev/queue_driver ]; then
    echo "✗ Device file not created!"
    exit 1
fi

if ! lsmod | grep -q queue_driver; then
    echo "✗ Module not loaded!"
    exit 1
fi
echo "✓ Verification passed"
echo ""

# Step 6: Display status
echo "[6/6] Installation complete!"
echo ""
echo "========================================"
echo "  Status"
echo "========================================"
echo ""
echo "Module Status:"
lsmod | grep queue_driver
echo ""
echo "Device File:"
ls -l /dev/queue_driver
echo ""
echo "Executables:"
ls -lh build/{configurator,filler,reader,job_submitter,worker}
echo ""
echo "========================================"
echo "  Quick Start"
echo "========================================"
echo ""
echo "1. Configure queue:"
echo "   ./build/configurator"
echo ""
echo "2. In separate terminals, start consumers:"
echo "   Terminal 1: ./build/reader"
echo "   Terminal 2: ./build/worker"
echo ""
echo "3. Send messages:"
echo "   echo 'Test log' | ./build/filler"
echo "   ./build/job_submitter"
echo ""
echo "4. View statistics:"
echo "   cat /proc/queue_driver_stats"
echo ""
echo "To unload module:"
echo "   sudo rmmod queue_driver"
echo ""
