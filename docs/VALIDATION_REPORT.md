# Validation Report

This package was checked in a Linux build environment before packaging.

## Checks performed

### 1. C++ compilation

Command:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src/system_monitor_app.cpp -o system_monitor
```

Result: **PASS**

### 2. C++ demo execution

Command:

```bash
./system_monitor --demo
```

Result: **PASS**

Observed output format:

```text
==================================================
          LINUX SYSTEM MONITOR
==================================================

SYSTEM RESOURCES
--------------------------------------------------
CPU Usage       : <runtime value>%
Memory Total    : <runtime value> MB
Memory Used     : <runtime value> MB (<runtime value>%)
Memory Available : <runtime value> MB
Processes       : <runtime value>

DRIVER COMMUNICATION
--------------------------------------------------
Mode            : DEMO MODE
Device          : /dev/system_monitor (not accessed)
Status          : Application test completed

==================================================
```

### 3. Kernel module compilation

Command:

```bash
make KDIR=/usr/src/linux-headers-6.12.96+deb13-amd64
```

Result: **PASS**

The module was compiled as:

```text
src/system_monitor.ko
```

### 4. Module metadata check

The resulting module reported:

```text
name:        system_monitor
version:     1.0
description: Custom character device driver for a Linux system monitor
license:     GPL
vermagic:    6.12.96+deb13-amd64 SMP preempt mod_unload modversions
```

## What was not performed here

The kernel module was not inserted into the running environment during packaging. Loading a kernel module requires root privileges and a compatible target kernel. On the target Linux machine, run the documented `load_driver.sh` script and then execute the C++ application in normal mode.

## Important

The exact runtime values for CPU usage, memory, process count, kernel release, uptime, and CPU count are machine-dependent. The sample output in `EXPECTED_OUTPUT.md` is intentionally an example of the expected format, not a fixed result.
