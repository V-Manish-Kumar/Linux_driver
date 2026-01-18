# Linux Kernel Queue Driver

A Linux kernel module implementing a typed message queue with blocking operations, designed for inter-process communication with support for logging and job submission systems.

## Project Structure

```
Linux_driver-main/
├── src/                    # Kernel module source code
│   └── queue_driver.c     # Main kernel driver implementation
├── tools/                  # User-space tools source code
│   ├── configurator.c     # Queue size configuration tool
│   ├── filler.c           # Log message producer
│   ├── reader.c           # Log message consumer
│   ├── job_submitter.c    # Job submission tool
│   └── worker.c           # Job execution worker
├── build/                  # Compiled binaries and build artifacts (auto-generated)
│   ├── queue_driver.ko    # Compiled kernel module
│   ├── configurator       # Compiled executables
│   ├── filler
│   ├── reader
│   ├── job_submitter
│   └── worker
├── Makefile               # Build automation
├── build.sh               # Quick setup script
├── .gitignore             # Git ignore patterns
├── README.md              # This file
└── LICENSE                # Apache 2.0 license
```

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Components](#components)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [API Documentation](#api-documentation)
- [Monitoring](#monitoring)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)
- [License](#license)
- [Author](#author)

## Overview

This project implements a custom Linux kernel character device driver that provides a circular queue for typed messages. The driver supports blocking operations, allowing user-space applications to efficiently send and receive messages with automatic synchronization.

### Key Capabilities

- **Typed Message Queue**: Supports different message types (e.g., logs, jobs)
- **Blocking I/O**: Automatic blocking when queue is full or empty
- **Thread-Safe**: Protected by mutexes with wait queues
- **Dynamic Sizing**: Configurable queue size at runtime
- **Statistics Monitoring**: Real-time stats via `/proc` interface
- **User-Space Tools**: Complete suite of helper programs

## Features

- Circular buffer implementation with configurable size
- Type-based message routing (MSG_LOG, MSG_JOB)
- Blocking push/pop operations with wait queues
- Mutex-protected critical sections
- Atomic statistics counters
- `/proc/queue_driver_stats` for monitoring
- IOCTL-based control interface
- User-space utilities for queue management

## Architecture

### Kernel Module: `queue_driver.c`

The kernel module provides three main IOCTL operations:

1. **SET_SIZE_OF_QUEUE**: Configure queue capacity
2. **PUSH_DATA**: Add typed messages to queue (blocks if full)
3. **POP_DATA**: Retrieve messages from queue (blocks if empty)

#### Data Structure

```c
struct data {
    int type;           /* Message type (MSG_LOG=100, MSG_JOB=200) */
    int length;         /* Payload length in bytes */
    char __user *data;  /* User-space pointer to payload */
};
```

#### Queue Management

- **Circular Buffer**: Efficient memory utilization
- **Head/Tail Pointers**: Track read/write positions
- **Count Tracking**: Monitor used/free space
- **Type + Length Headers**: 8 bytes per message

### Synchronization

- **Mutex Lock**: `q_lock` protects queue operations
- **Wait Queues**: 
  - `read_wait`: Blocks readers when queue is empty
  - `write_wait`: Blocks writers when queue is full

## Components

| Component | Type | Description | Key Features |
|-----------|------|-------------|-------------|
| **queue_driver.c** | Kernel Module | Core driver implementing message queue | - `queue_init()`: Device initialization<br>- `queue_exit()`: Cleanup<br>- `queue_ioctl()`: IOCTL handler<br>- `queue_stats_show()`: Statistics |
| **configurator.c** | User-Space Tool | Sets the queue size | - Default: 64 bytes<br>- Must run before operations<br>- Usage: `./configurator` |
| **filler.c** | Log Producer | Pushes log messages (MSG_LOG=100) | - Reads from stdin<br>- Usage: `echo "msg" \| ./filler`<br>- Can pipe from files |
| **reader.c** | Log Consumer | Reads and displays log messages | - Continuous reading<br>- Outputs to stdout<br>- Usage: `./reader` |
| **job_submitter.c** | Job Producer | Submits jobs (MSG_JOB=200) | - Interactive mode<br>- Auto job ID assignment<br>- Format: `job_id:command` |
| **worker.c** | Job Consumer | Executes jobs from queue | - Processes MSG_JOB<br>- Uses `system()` for execution<br>- Shows PID and job ID |

## Environment Setup

### Platform Requirements

**This is a Linux kernel module and requires a Linux environment to run.** Since you're on Windows, you have several options:

| Option | Pros | Cons | Recommended For |
|--------|------|------|-----------------|
| **WSL2 (Windows Subsystem for Linux)** | Easy setup, native Windows integration | Limited kernel module support | Testing user-space tools only |
| **VirtualBox VM** | Full Linux environment, safe testing | Requires more resources | Development and testing |
| **VMware Workstation** | Better performance than VirtualBox | Commercial license for some features | Professional development |
| **Dual Boot Linux** | Native performance | Requires disk partitioning | Serious kernel development |
| **Cloud VM (AWS/Azure/GCP)** | No local setup needed | Costs money, requires internet | Quick testing |

### Recommended: VirtualBox Virtual Machine

**Step-by-step setup:**

1. **Download and Install VirtualBox**
   - Download from: https://www.virtualbox.org/
   - Install VirtualBox on your Windows machine

2. **Download a Linux Distribution**
   - **Ubuntu 22.04 LTS** (Recommended): https://ubuntu.com/download/desktop
   - Alternatives: Debian, Fedora, or CentOS

3. **Create a Virtual Machine**
   ```
   - Open VirtualBox → New
   - Name: Linux_Driver_Dev
   - Type: Linux
   - Version: Ubuntu (64-bit)
   - Memory: 2048 MB (minimum) or 4096 MB (recommended)
   - Hard Disk: Create virtual hard disk (20 GB minimum)
   - Create
   ```

4. **Install Linux in VM**
   ```
   - Select your VM → Settings → Storage
   - Click "Empty" under Controller: IDE
   - Click disk icon → Choose a disk file
   - Select your Ubuntu ISO file
   - Start the VM
   - Follow Ubuntu installation wizard
   ```

5. **Install Guest Additions** (Optional but recommended)
   ```bash
   # In the VM, after Linux is installed
   sudo apt-get update
   sudo apt-get install build-essential dkms linux-headers-$(uname -r)
   # Then: Devices → Insert Guest Additions CD image
   # Follow prompts
   ```

6. **Transfer Project Files to VM**
   
   **Option A: Shared Folder**
   ```
   - VM Settings → Shared Folders
   - Add new shared folder
   - Folder Path: C:\Users\manis\Downloads\Linux_driver-main
   - Folder Name: driver_project
   - Auto-mount: Yes
   - Access in VM: /media/sf_driver_project
   ```
   
   **Option B: Copy/Paste** (if Guest Additions installed)
   ```
   - VM Settings → General → Advanced
   - Shared Clipboard: Bidirectional
   - Copy files from Windows to VM
   ```
   
   **Option C: Git Clone**
   ```bash
   # If your project is on GitHub
   git clone <your-repo-url>
   cd Linux_driver-main
   ```

### Alternative: WSL2 (Limited Support)

**Note:** WSL2 has limited kernel module support. You can compile but may not be able to load modules.

```powershell
# On Windows PowerShell (Admin)
wsl --install -d Ubuntu-22.04

# After installation
wsl

# Inside WSL
cd /mnt/c/Users/manis/Downloads/Linux_driver-main
```

**Limitation:** `insmod` may fail in WSL2 as it uses a custom kernel.

## Requirements

### System Requirements

- **Linux Kernel**: Version 5.x or higher
- **Kernel Headers**: Matching your kernel version
- **GCC Compiler**: For building kernel modules and user-space tools
- **Make**: Build automation tool

### Development Packages

```bash
# Debian/Ubuntu
sudo apt-get install build-essential linux-headers-$(uname -r)

# Red Hat/CentOS/Fedora
sudo yum install kernel-devel kernel-headers gcc make

# Arch Linux
sudo pacman -S linux-headers base-devel
```

## Installation

### Complete Installation Steps

**Prerequisites:** Make sure you're in a Linux environment (VM or native Linux).

#### Step 1: Navigate to Project Directory

```bash
# If using shared folder in VirtualBox
cd /media/sf_driver_project

# Or if you copied files
cd ~/Linux_driver-main

# Or if cloned from git
cd ~/Linux_driver-main

# Verify structure
ls -la
# Should see: src/ tools/ build/ Makefile README.md LICENSE
```

#### Step 2: Install Dependencies

```bash
# For Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential linux-headers-$(uname -r) gcc make

# For Fedora/RHEL/CentOS
sudo yum install kernel-devel kernel-headers gcc make

# For Arch Linux
sudo pacman -S linux-headers base-devel
```

#### Step 3: Verify Kernel Headers

```bash
# Check if headers are installed
ls /lib/modules/$(uname -r)/build

# If not found, insEverything (Module + Tools)

**Option A: Using the automated script (Recommended)**

```bash
# Make script executable
chmod +x build.sh

# Run the build script
./build.sh
```

The script will:
- Clean previous builds
- Compile kernel module
- Compile all user-space tools
- Load the kernel module
- Set device permissions
- Verify installation

**Option B: Using Makefile manually**

```bash
# Clean any previous builds
make clean

# Build everything (module + tools)
make all

# Or build separately:
make module    # Build only kernel module
make tools     # Build only user-space tools
```

**Expected output:**
```
Building kernel module...
make -C /lib/modules/5.x.x-generic/build M=/path/to/src modules
  CC [M]  /path/to/src/queue_driver.o
  LD [M]  /path/to/src/queue_driver.ko
Building configurator...
Building filler...
Building reader...
Building job_submitter...
Building worker...
All tools built successfully!
```

All compiled files will be in the `build/` directory.

#### Step 5: Load the Kernel Module

```bash
# Option A: Use Makefile
make install

# Option B: Manual loading
sudo insmod build/queue_driver.ko
sudo chmod 666 /dev/queue_driver
```

#### Step 6: Verify Installation

```bash
# Use Makefile to check status
make info

# Or check manually:
lsmod | grep queue_driver
ls -l /dev/queue_driver
cat /proc/queue_driver_stats
```

**Expected output:**
```
=== Module Status ===
queue_driver           16384  0

=== Device File ===
crw-rw-rw- 1 root root 240, 0 Jan 18 17:30 /dev/queue_driver

=== Statistics ===
queue_size_bytes: 0
used_bytes: 0
free_bytes: 0
total_pushes: 0
total_pops: 0
blocked_readers: 0
blocked_writers: 0
```

### Makefile Targets

| Target | Description |
|--------|-------------|
| `make all` | Build kernel module and all user-space tools (default) |
| `make module` | Build only the kernel module |
| `make tools` | Build only user-space tools |
| `make install` | Build and install kernel module with permissions |
| `make uninstall` | Unload kernel module |
| `make clean` | Remove all build artifacts |
| `make info` | Display module status and statistics |
| `make help` | Show available targets | it with:
```bash
chmod +x build.sh
./build.sh
```

## Usage

### Quick Start Guide

**Complete workflow from start to finish:**

#### Step 1: Open Terminal in Linux VM

```bash
# Navigate to project directory
cd ~/Linux_driver-main

# Verify module is loaded
lsmod | grep queue_driver

# If not loaded, load it:
sudo insmod queue_driver.ko
sudo chmod 666 /dev/queue_driver
```

#### Step 2: Configure Queue Size

```bash
# Run configurator (sets queue to 64 bytes by default)
./configurator

# Verify in proc stats
cat /proc/queue_driver_stats
```

**Expected output:**
```
queue_size_bytes: 64
used_bytes: 0
free_bytes: 64
total_pushes: 0
total_pops: 0
blocked_readers: 0
blocked_writers: 0
```

#### Step 3: Open Multiple Terminals

**In Linux VM, open 4 terminal windows (or tabs):**

- Press `Ctrl+Alt+T` to open terminal
- Or use terminal tabs: `Ctrl+Shift+T`

#### Step 4: Start Consumers

**Terminal 1 - Log Reader:**
```bash
cd ~/Linux_driver-main
./reader
# This will block waiting for log messages
```

**Terminal 2 - Job Worker:**
```bash
cd ~/Linux_driver-main
./worker
# This will block waiting for jobs
```

#### Step 5: Send Messages

**Terminal 3 - Log Filler:**
```bash
cd ~/Linux_driver-main
echo "System started" | ./filler
echo "User logged in" | ./filler
echo "Processing request" | ./filler
```

You should see these messages appear in **Terminal 1** (reader).

**Terminal 4 - Job Submitter:**
```bash
cd ~/Linux_driver-main
./job_submitter
```

Then type commands (each on a new line):
```
ls -la
date
echo "Hello from worker"
pwd
uname -a
```

Press `Ctrl+D` to finish.

You should see these jobs execute in **Terminal 2** (worker).

#### Step 6: Monitor Statistics

**In any terminal:**
```bash
# View current stats
cat /proc/queue_driver_stats

# Watch in real-time
watch -n 1 cat /proc/queue_driver_stats
```

### Testing Workflow Summary

| Step | Terminal | Command | What Happens |
|------|-------build/configurator` | Queue initialized to 64 bytes |
| 2 | 1 | `./build/reader` | Starts waiting for log messages |
| 3 | 2 | `./build/worker` | Starts waiting for jobs |
| 4 | 3 | `echo "test" \| ./build/filler` | Sends log → appears in Terminal 1 |
| 5 | 4 | `./build/job_submitter` then type commands | Sends jobs → execute in Terminal 2 |
| 6 | Any | `cat /proc/queue_driver_stats` | View statistics |

### Basic Workflow

1. **Load the driver:**
   ```bash
   make install
   # Or manually:
   # sudo insmod build/queue_driver.ko
   # sudo chmod 666 /dev/queue_driver
   ```

2. **Configure queue size:**
   ```bash
   ./build/configurator
   ```

3. **Start consumers (in separate terminals):**
   ```bash
   # Terminal 1: Log reader
   ./build/reader

   # Terminal 2: Job worker
   ./build/worker
   ```

4. **Send messages:**
   ```bash
   # Terminal 3: Send logs
   echo "System started" | ./build/filler
   echo "Processing data" | ./build/filler

   # Terminal 4: Submit jobs
   ./build Terminal 4: Submit jobs
   ./job_submitter
   # Then type commands:
   ls -la
   date
   echo "Hello from worker"
   ```

### Advanced Usage

#### Logging System

```bashbuild/filler

# Monitor application output
./myapp 2>&1 | ./build/filler
```

#### Job Queue System

```bash
# Start multiple workers
./build/worker &
./build/worker &
./build/worker &

# Submit batch jobs
cat jobs.txt | ./build
# Submit batch jobs
cat jobs.txt | ./job_submitter
```

Example `jobs.txt`:
```
echo "Task 1"
sleep 2
echo "Task 2"
date
```

## API Documentation

### IOCTL Commands

| Command | Code | Direction | Parameters | Behavior | Returns |
|---------|------|-----------|------------|----------|--------|
| **SET_SIZE_OF_QUEUE** | `_IOW(IOCTL_MAGIC, 'a', int)` | Write | Queue size (bytes) | Resets queue; all data lost | 0 on success, negative on error |
| **PUSH_DATA** | `_IOW(IOCTL_MAGIC, 'b', struct data)` | Write | `struct data` (type, length, payload) | Blocks if insufficient space | 0 on success, -EFAULT on error |
| **POP_DATA** | `_IOWR(IOCTL_MAGIC, 'c', struct data)` | Read/Write | `struct data` (user buffer) | Blocks if queue empty | 0 on success, -EFAULT on error |

### Message Types

| Type | Value | Purpose | Used By |
|------|-------|---------|---------|
| MSG_LOG | 100 | Log messages | filler.c, reader.c |
| MSG_JOB | 200 | Job commands | job_submitter.c, worker.c |

## Monitoring

### View Statistics

```bash
cat /proc/queue_driver_stats
```

**Output:**
```
queue_size_bytes: 64
used_bytes: 24
free_bytes: 40
total_pushes: 150
total_pops: 126
blocked_readers: 5
blocked_writers: 3
```

### Statistics Explained

| Statistic | Description |
|-----------|-------------|
| **queue_size_bytes** | Total queue capacity |
| **used_bytes** | Currently occupied space |
| **free_bytes** | Available space |
| **total_pushes** | Cumulative push operations |
| **total_pops** | Cumulative pop operations |
| **blocked_readers** | Times readers waited for data |
| **blocked_writers** | Times writers waited for space |

### Monitoring Script

```bash
# Watch stats in real-time
watch -n 1 cat /proc/queue_driver_stats
```

## Examples

### Example 1: Simple Logging

```bash
# Terminal 1: Start reader
./build/reader > output.log

# Terminal 2: Send logs
./build/configurator
echo "Application started" | ./build/filler
echo "Processing request 1" | ./build/filler
echo "Processing request 2" | ./build/filler
```

### Example 2: Job Processing

```bash
# Terminal 1: Start worker
./build/configurator
./build/worker

# Terminal 2: Submit jobs
./build/job_submitter
# Type:
echo "Job 1" > result1.txt
echo "Job 2" > result2.txt
sleep 1
ls -l result*.txt
```

### Example 3: Multi-Consumer

```bash
# Configure queue
./build/configurator

# Start 2 readers
./build/reader > logs1.txt &
./build/reader > logs2.txt &

# Send logs (will be distributed)
for i in {1..10}; do
  echo "Log message $i" | ./build/filler
done
```

### Example 4: Stress Test

```bash
# Large queue
./build/configurator  # Modify to set larger size

# Continuous logging
while true; do
  date | ./build/filler
  sleep 0.1
done &

# Continuous reading
./build/reader &

# Monitor performance
watch -n 1 cat /proc/queue_driver_stats
```

## Troubleshooting

### Common Problems and Solutions

| Problem | Cause | Solution |
|---------|-------|----------|
| **"No such device"** | Module not loaded | `lsmod \| grep queue_driver`<br>`sudo insmod queue_driver.ko`<br>`ls -l /dev/queue_driver` |
| **"Permission denied"** | Insufficient permissions | `sudo chmod 666 /dev/queue_driver` |
| **Programs hang indefinitely** | Queue full (writer) or empty (reader) | - Start reader before filler<br>- Increase queue size<br>- Balance producers/consumers |
| **Module won't load** | Kernel version mismatch or build errors | `dmesg \| tail -20`<br>`uname -r`<br>`make clean && make` |
| **Incomplete messages** | Queue size too small | Set queue size ≥ max_message_length + 8 bytes |

### Debugging Tips

```bash
# Enable verbose kernel logging
sudo dmesg -w

# Monitor system calls
strace ./build/filler
strace ./build/reader

# Check module info
modinfo build/queue_driver.ko

# View loaded module details
cat /sys/module/queue_driver/parameters/* 2>/dev/null || echo "No parameters"

# Use Makefile info command
make info
```

## Cleanup

### Unload Module

```bash
# Stop all user-space programs first (Ctrl+C)

# Option A: Use Makefile
make uninstall

# Option B: Manual unload
sudo rmmod queue_driver

# Verify removal
lsmod | grep queue_driver
dmesg | tail
```

### Clean Build Files

```bash
# Clean everything
make clean

# This removes all contents of build/ directory
```

## Security Considerations

1. **Permissions**: The device file `/dev/queue_driver` is accessible to all users (666). In production, restrict to specific groups.
   
2. **Input Validation**: User data is copied from user-space; ensure message lengths are validated.

3. **Command Execution**: `worker.c` uses `system()` which executes arbitrary commands. Use with caution in production environments.

4. **Resource Limits**: No hard limit on queue operations; can be DoS vector if not properly controlled.

## Performance Optimization

| Optimization | Recommendation | Impact |
|--------------|----------------|--------|
| **Queue Size** | Adjust based on workload (64 bytes is minimal) | Reduces blocking operations |
| **Multiple Workers** | Scale consumers for high job throughput | Increases parallel processing |
| **Batch Operations** | Group related messages together | Reduces syscall overhead |
| **Buffer Sizing** | Allocate appropriate user-space buffers (256 bytes default) | Prevents buffer overflows |

## Future Enhancements

- [ ] Support for priority queues
- [ ] Multiple named queues
- [ ] Message filtering by type
- [ ] Configurable timeout for blocking operations
- [ ] Persistent queue (survive reboot)
- [ ] Network-based remote queue access
- [ ] Binary protocol for efficiency
- [ ] Message acknowledgment system

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

## Author

**Manish**

- Kernel module and driver implementation
- User-space utility suite
- Documentation and examples

## Contributing

Contributions are welcome! Areas for improvement:

1. Add configurable timeouts
2. Implement priority queuing
3. Add unit tests
4. Enhance error handling
5. Support for multiple device instances
6. Add ioctl for queue statistics

## Support

For issues, questions, or suggestions:

1. Check the [Troubleshooting](#troubleshooting) section
2. Review kernel logs with `dmesg`
3. Verify compatibility with your kernel version

## Learning Resources

- [Linux Kernel Module Programming Guide](https://www.kernel.org/doc/html/latest/)
- [Linux Device Drivers (LDD3)](https://lwn.net/Kernel/LDD3/)
- [Character Device Drivers](https://www.kernel.org/doc/html/latest/driver-api/index.html)
- [IOCTL Interface](https://www.kernel.org/doc/html/latest/userspace-api/ioctl/ioctl-number.html)

## Project Statistics

| Metric | Value |
|--------|-------|
| **Lines of Code** | ~540 (kernel + user-space) |
| **Kernel Module** | 239 lines |
| **User-Space Tools** | 5 programs |
| **Language** | C (C99 standard) |
| **Target** | Linux Kernel 5.x+ |

---

**Note**: This driver is designed for educational purposes and demonstration. For production use, additional hardening, error handling, and security measures should be implemented.