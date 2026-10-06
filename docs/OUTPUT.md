# Expected Output

The exact CPU, memory, process count, kernel version, CPU count, and uptime will vary by Linux machine. The following is an example of the expected format.

## 1. Build Output

```text
make
make -C /lib/modules/6.12.96+deb13-amd64/build M=/path/to/linux-system-monitor-driver modules
  CC [M]  /path/to/linux-system-monitor-driver/src/system_monitor.o
  MODPOST /path/to/linux-system-monitor-driver/Module.symvers
  CC [M]  /path/to/linux-system-monitor-driver/src/system_monitor.mod.o
  LD [M]  /path/to/linux-system-monitor-driver/src/system_monitor.ko
  BTF [M] /path/to/linux-system-monitor-driver/src/system_monitor.ko
make -C /usr/src/linux-headers-6.12.96+deb13-amd64 M=/path/to/linux-system-monitor-driver modules

g++ -std=c++17 -O2 -Wall -Wextra -pedantic src/system_monitor_app.cpp -o system_monitor
```

The exact compiler messages can differ between distributions and kernel versions.

## 2. Driver Load Output

```text
$ sudo ./scripts/load_driver.sh
Driver loaded successfully.
crw------- 1 root root 511, 0 Oct  4 20:10 /dev/system_monitor
```

The major/minor device numbers can differ.

## 3. Application Output

```text
$ sudo ./system_monitor
==================================================
          LINUX SYSTEM MONITOR
==================================================

SYSTEM RESOURCES
--------------------------------------------------
CPU Usage       : 12.7%
Memory Total    : 15884.0 MB
Memory Used     : 6120.5 MB (38.6%)
Memory Available : 9763.5 MB
Processes       : 214

DRIVER COMMUNICATION
--------------------------------------------------
Linux System Monitor Driver
Device: /dev/system_monitor
Kernel: 6.12.96+deb13-amd64
Online CPUs: 8
System Uptime: 18342 seconds
Last Command: status

==================================================
```

Values are examples only; they are intentionally not presented as fixed machine results.

## 4. Demo Output Without the Driver

This mode is useful for testing the C++ application on a Linux machine before loading the module:

```text
$ ./system_monitor --demo
==================================================
          LINUX SYSTEM MONITOR
==================================================

SYSTEM RESOURCES
--------------------------------------------------
CPU Usage       : 9.4%
Memory Total    : 15884.0 MB
Memory Used     : 6001.2 MB (37.8%)
Memory Available : 9882.8 MB
Processes       : 209

DRIVER COMMUNICATION
--------------------------------------------------
Mode            : DEMO MODE
Device          : /dev/system_monitor (not accessed)
Status          : Application test completed

==================================================
```
