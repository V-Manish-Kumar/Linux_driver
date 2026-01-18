# Project Structure Documentation

## Directory Layout

```
Linux_driver-main/
│
├── src/                          # Kernel Module Source
│   └── queue_driver.c           # Main kernel driver (239 lines)
│
├── tools/                        # User-Space Tools Source
│   ├── configurator.c           # Queue size configuration
│   ├── filler.c                 # Log message producer (MSG_LOG=100)
│   ├── reader.c                 # Log message consumer
│   ├── job_submitter.c          # Job submission tool (MSG_JOB=200)
│   └── worker.c                 # Job execution worker
│
├── build/                        # Build Artifacts (auto-generated)
│   ├── queue_driver.ko          # Compiled kernel module
│   ├── queue_driver.o           # Object files
│   ├── queue_driver.mod*        # Module metadata
│   ├── Module.symvers           # Kernel symbol versions
│   ├── modules.order            # Module load order
│   ├── configurator             # Compiled executables
│   ├── filler
│   ├── reader
│   ├── job_submitter
│   └── worker
│
├── Makefile                      # Build automation
├── build.sh                      # Quick setup script (automated build & install)
├── .gitignore                    # Git ignore patterns
├── README.md                     # Complete documentation
├── STRUCTURE.md                  # This file
└── LICENSE                       # Apache 2.0 license
```

## File Descriptions

### Source Code Files

| File | Type | Lines | Purpose |
|------|------|-------|---------|
| `src/queue_driver.c` | Kernel Module | 239 | Character device driver with circular queue |
| `tools/configurator.c` | User Tool | ~18 | Sets queue size via IOCTL |
| `tools/filler.c` | User Tool | ~42 | Pushes log messages from stdin |
| `tools/reader.c` | User Tool | ~45 | Pops and displays log messages |
| `tools/job_submitter.c` | User Tool | ~47 | Submits jobs with auto ID |
| `tools/worker.c` | User Tool | ~56 | Executes jobs using system() |

### Build System Files

| File | Purpose |
|------|---------|
| `Makefile` | Automated build system with multiple targets |
| `build.sh` | One-click build, install, and verification script |
| `.gitignore` | Excludes build artifacts from version control |

### Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Complete user and developer documentation |
| `STRUCTURE.md` | This file - project organization guide |
| `LICENSE` | Apache 2.0 license terms |

## Build Artifacts

All compiled files are placed in the `build/` directory:

### Kernel Module Artifacts
- `queue_driver.ko` - Loadable kernel module (main output)
- `queue_driver.o` - Compiled object file
- `queue_driver.mod.o` - Module metadata object
- `queue_driver.mod.c` - Generated module info
- `queue_driver.mod` - Module signature
- `Module.symvers` - Exported kernel symbols
- `modules.order` - Module loading order

### User-Space Executables
- `configurator` - Binary executable
- `filler` - Binary executable
- `reader` - Binary executable
- `job_submitter` - Binary executable
- `worker` - Binary executable

## Workflow: How Files Interact

```
┌─────────────────────────────────────────────────────────────┐
│                    KERNEL SPACE                             │
│                                                             │
│  ┌────────────────────────────────────────────────────┐   │
│  │  src/queue_driver.c                                │   │
│  │  - Character device: /dev/queue_driver            │   │
│  │  - Circular queue buffer                           │   │
│  │  - IOCTL handlers (SET_SIZE, PUSH, POP)          │   │
│  │  - Proc stats: /proc/queue_driver_stats           │   │
│  └────────────────────────────────────────────────────┘   │
│                         ▲                                   │
└─────────────────────────┼───────────────────────────────────┘
                          │ IOCTL calls
                          │
┌─────────────────────────┼───────────────────────────────────┐
│                    USER SPACE                               │
│                         │                                   │
│  ┌──────────────────────┼───────────────┐                  │
│  │  tools/configurator.c                │                  │
│  │  SET_SIZE_OF_QUEUE ──┘                │                  │
│  └───────────────────────────────────────┘                  │
│                                                             │
│  ┌───────────────────────┐      ┌───────────────────────┐ │
│  │  tools/filler.c       │      │  tools/reader.c       │ │
│  │  PUSH_DATA            │      │  POP_DATA             │ │
│  │  (MSG_LOG = 100)      │──────▶  (MSG_LOG = 100)      │ │
│  └───────────────────────┘      └───────────────────────┘ │
│                                                             │
│  ┌───────────────────────┐      ┌───────────────────────┐ │
│  │ tools/job_submitter.c │      │  tools/worker.c       │ │
│  │ PUSH_DATA             │      │  POP_DATA             │ │
│  │ (MSG_JOB = 200)       │──────▶  (MSG_JOB = 200)      │ │
│  └───────────────────────┘      └───────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Build Process

### Step 1: Source Organization
```
src/queue_driver.c   ──┐
tools/*.c            ──┤ Source files
Makefile             ──┘
```

### Step 2: Compilation
```
make all
├── make module
│   ├── Compile src/queue_driver.c → queue_driver.o
│   └── Link → queue_driver.ko
│
└── make tools
    ├── gcc tools/configurator.c → configurator
    ├── gcc tools/filler.c → filler
    ├── gcc tools/reader.c → reader
    ├── gcc tools/job_submitter.c → job_submitter
    └── gcc tools/worker.c → worker
```

### Step 3: Output
```
All artifacts → build/
```

## Development Workflow

### Initial Setup
1. Clone or copy project
2. Navigate to project root
3. Run `./build.sh` (automated) or `make all` (manual)

### Development Cycle
1. Edit source files in `src/` or `tools/`
2. Run `make clean` to remove old artifacts
3. Run `make all` to rebuild
4. Test changes

### Deployment
1. `make install` - Load kernel module
2. Run user-space tools from `build/` directory
3. Monitor with `cat /proc/queue_driver_stats`
4. `make uninstall` - Remove kernel module

## Version Control

The `.gitignore` file excludes:
- All files in `build/` directory
- Temporary editor files
- OS-specific files

Only source files and documentation are tracked:
- `src/*.c`
- `tools/*.c`
- `Makefile`
- `*.md`
- `LICENSE`
- `build.sh`

## Best Practices

### Source Code Organization
- Kernel code stays in `src/`
- User-space tools stay in `tools/`
- Never mix source and compiled files

### Building
- Always use `make clean` before rebuilding
- Use `make info` to check module status
- Use `build.sh` for first-time setup

### Testing
- Test in VM before production
- Use separate terminals for producers/consumers
- Monitor `/proc/queue_driver_stats` during testing

### Cleanup
- Unload module before removing: `make uninstall`
- Clean artifacts: `make clean`
- Remove all build outputs before committing to git

## Maintenance

### Adding New User Tool
1. Create `tools/newtool.c`
2. Add to `TOOLS` variable in Makefile
3. Run `make tools`
4. Update documentation

### Modifying Kernel Module
1. Edit `src/queue_driver.c`
2. Unload current module: `make uninstall`
3. Rebuild: `make module`
4. Load new module: `make install`
5. Test thoroughly

### Cleaning Up
```bash
# Remove all build artifacts
make clean

# Unload kernel module
make uninstall

# Complete cleanup
make uninstall && make clean
```

## Common Locations

| Item | Location |
|------|----------|
| Kernel module source | `src/queue_driver.c` |
| User tools source | `tools/*.c` |
| Compiled module | `build/queue_driver.ko` |
| Executables | `build/configurator`, etc. |
| Device file | `/dev/queue_driver` |
| Statistics | `/proc/queue_driver_stats` |
| Build rules | `Makefile` |
| Setup script | `build.sh` |

## Summary

This structure separates concerns:
- **Source** (`src/`, `tools/`) - Version controlled, human-edited
- **Build** (`build/`) - Generated, ignored by git
- **Config** (`Makefile`, `build.sh`) - Build automation
- **Docs** (`README.md`, this file) - Documentation

Benefits:
- Clean separation of source and artifacts
- Easy to clean and rebuild
- Clear for new developers
- Works well with version control
- Follows Linux kernel development conventions
