# Linux System Monitor Using a Custom Character Device Driver

## Project Overview

The **Linux System Monitor Using a Custom Character Device Driver** is a system-level programming project developed using **C, C++, and Linux**.

The project consists of two major components:

* A custom **Linux character device driver written in C**
* A **C++ user-space system monitoring application**

The application monitors basic system information such as CPU usage, memory usage, and the number of running processes. It also communicates with the custom kernel driver through the device file `/dev/system_monitor`.

The project demonstrates the interaction between **user space and kernel space** using standard character-device operations such as `open()`, `read()`, `write()`, and `release()`.

---

## Objectives

The main objectives of this project are:

* To understand Linux user-space and kernel-space architecture.
* To develop a basic custom Linux character device driver.
* To understand how Linux device files work.
* To implement character-device file operations.
* To monitor basic system resources.
* To demonstrate user-space and kernel-space communication.
* To gain practical experience with Linux system programming.
* To develop a project that can be built, tested, documented, and maintained using Git and GitHub.

---

## Features

* CPU usage monitoring
* Total memory information
* Used memory information
* Available memory information
* Running process count
* Custom Linux character device driver
* `/dev/system_monitor` device
* `open()` operation
* `read()` operation
* `write()` operation
* `release()` operation
* User-space to kernel-space communication
* Kernel-space to user-space communication
* Driver status information
* Demo mode for user-space monitoring
* Makefile-based build system
* Driver loading and unloading scripts

---

## Technologies Used

| Technology          | Purpose                                  |
| ------------------- | ---------------------------------------- |
| C                   | Linux kernel character device driver     |
| C++17               | User-space system monitoring application |
| Linux               | Operating system                         |
| Linux Kernel Module | Kernel-space driver                      |
| `/proc` filesystem  | CPU, memory, and process information     |
| Character Device    | User/kernel communication                |
| GNU Make            | Build automation                         |
| Bash                | Driver management scripts                |
| Git/GitHub          | Version control and project hosting      |

---

## System Architecture

The project follows a user-space to kernel-space architecture.

```text
                    USER SPACE
+---------------------------------------------+
|                                             |
|       C++ System Monitoring Application     |
|                                             |
|    CPU | Memory | Processes | Driver Info  |
|                                             |
+-----------------------+---------------------+
                        |
                        | open()
                        | read()
                        | write()
                        | close()
                        |
                        v
              +---------------------+
              | /dev/system_monitor |
              +----------+----------+
                         |
                         v
                    KERNEL SPACE
+---------------------------------------------+
|                                             |
|       Custom Character Device Driver       |
|                                             |
|   open() | read() | write() | release()   |
|                                             |
+-----------------------+---------------------+
                        |
                        v
                Linux Kernel Interfaces
                        |
                        v
               /proc and system resources
```

### Communication Flow

```text
C++ Application
       |
       | write("status")
       v
/dev/system_monitor
       |
       v
Character Device Driver
       |
       | Process command
       v
Linux Kernel
       |
       | Return driver information
       v
Character Device Driver
       |
       | read()
       v
C++ Application
```

---

## Project Structure

```text
linux-system-monitor-driver/
│
├── src/
│   ├── system_monitor.c
│   └── system_monitor_app.cpp
│
├── scripts/
│   ├── load_driver.sh
│   ├── unload_driver.sh
│   └── check_driver.sh
│
├── docs/
│   ├── EXPECTED_OUTPUT.md
│   ├── VALIDATION_REPORT.md
│   └── MODULE_FLOW.md
│
├── README.md
├── MENTOR_GUIDE.md
├── Makefile
├── LICENSE
└── .gitignore
```

---

## File Description

### `src/system_monitor.c`

Contains the Linux kernel character device driver.

The driver:

* Registers the character device.
* Creates `/dev/system_monitor`.
* Implements the required file operations.
* Receives commands from user space.
* Returns driver status information.

### `src/system_monitor_app.cpp`

Contains the user-space system monitoring application.

The application:

* Calculates CPU usage.
* Reads memory information.
* Counts running processes.
* Communicates with `/dev/system_monitor`.
* Displays the collected information.

### `scripts/load_driver.sh`

Loads the Linux kernel module and prepares the character device.

### `scripts/unload_driver.sh`

Removes the Linux kernel module.

### `scripts/check_driver.sh`

Checks whether the driver and device are available.

### `docs/EXPECTED_OUTPUT.md`

Contains sample output for reference.

### `docs/VALIDATION_REPORT.md`

Contains project validation information.

### `docs/MODULE_FLOW.md`

Contains the driver communication flow.

### `Makefile`

Automates compilation and cleanup.

---

## Requirements and Dependencies

The project requires a Linux environment.

### Requirements

* Linux operating system
* GCC
* G++
* C++17 support
* GNU Make
* Matching Linux kernel headers
* `sudo` or root privileges for loading and unloading the kernel module

### Recommended Environment

Ubuntu or another Debian-based Linux distribution can be used.

---

## Installing Dependencies

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)
```

Check the installed tools:

```bash
gcc --version
g++ --version
make --version
uname -r
```

---

## Installation / Setup

Clone the repository:

```bash
git clone <YOUR-GITHUB-REPOSITORY-URL>
```

Move into the project directory:

```bash
cd linux-system-monitor-driver
```

---

## Build Instructions

Build the complete project:

```bash
make
```

Build only the kernel driver:

```bash
make driver
```

Build only the C++ application:

```bash
make app
```

Clean generated build files:

```bash
make clean
```

---

## Run Instructions

### 1. Load the Driver

```bash
sudo ./scripts/load_driver.sh
```

The driver should create:

```text
/dev/system_monitor
```

Check the device:

```bash
ls -l /dev/system_monitor
```

---

### 2. Check Driver Status

```bash
sudo ./scripts/check_driver.sh
```

---

### 3. Run the System Monitor

```bash
sudo ./system_monitor
```

The application collects system information and communicates with the custom character device.

---

### 4. Check Kernel Messages

```bash
dmesg | grep system_monitor
```

Example:

```text
system_monitor: loaded successfully
system_monitor: device created at /dev/system_monitor
system_monitor: device opened
system_monitor: command received: status
system_monitor: device closed
```

---

### 5. Unload the Driver

```bash
sudo ./scripts/unload_driver.sh
```

---

## Demo Mode

The project includes a demo mode for testing the user-space monitoring logic without loading the kernel module.

Run:

```bash
./system_monitor --demo
```

Demo mode verifies the user-space monitoring functionality.

It does **not** represent actual communication with the kernel driver.

Actual driver communication requires a Linux environment where the kernel module can be built and loaded with the required privileges.

---

## How the System Works

### CPU Monitoring

CPU statistics are read from:

```text
/proc/stat
```

The application takes CPU samples over a short time interval and calculates CPU utilization based on the change between the samples.

---

### Memory Monitoring

Memory information is read from:

```text
/proc/meminfo
```

The application uses values such as:

```text
MemTotal
MemAvailable
```

Approximate used memory is calculated as:

```text
Used Memory = Total Memory - Available Memory
```

---

### Process Monitoring

Linux exposes process information through numeric directories under:

```text
/proc
```

Examples:

```text
/proc/1
/proc/100
/proc/250
```

The application scans these numeric directories to estimate the number of running processes.

---

## Character Device Driver

The kernel component is implemented as a custom Linux character device driver.

The driver creates:

```text
/dev/system_monitor
```

This provides a file-like interface for communication between the user-space application and the kernel driver.

---

## Driver Operations

### `open()`

Called when the device is opened by the user-space application.

### `write()`

Used by the application to send a command to the driver, such as:

```text
status
```

### `read()`

Used by the application to read the driver's response.

The driver can return information such as:

* Kernel release
* Online CPU count
* System uptime
* Last command received

### `release()`

Called when the device is closed.

---

## User Space and Kernel Space

The C++ application runs in **user space**, where applications operate with restricted privileges.

The custom character driver runs in **kernel space**, where it can interact with kernel facilities.

The project demonstrates communication between these two environments through the character device.

---

## Data Transfer

The driver uses safe Linux kernel mechanisms for transferring information between user space and kernel space.

### User space → Kernel space

```c
copy_from_user()
```

### Kernel space → User space

```c
copy_to_user()
```

These mechanisms are used when transferring buffers across the user/kernel boundary.

---

## Expected Output

A typical output may look like:

```text
========================================
        LINUX SYSTEM MONITOR
========================================

CPU Usage        : 18.42%
Total Memory     : 7.72 GB
Used Memory      : 3.61 GB
Available Memory : 4.11 GB
Processes        : 184

----------------------------------------
       CHARACTER DEVICE INFORMATION
----------------------------------------

Device           : /dev/system_monitor
Kernel           : 6.x.x
Online CPUs      : 8
Uptime           : 54231 seconds
Last Command     : status

========================================
```

### Note

The exact values will vary depending on the Linux machine.

CPU usage, memory usage, process count, kernel version, CPU count, and uptime are runtime-dependent values.

---

## Testing and Validation

The project is designed to be validated at multiple levels.

### User-Space Application

* C++ compilation
* CPU monitoring logic
* Memory monitoring logic
* Process counting
* Demo mode execution

### Kernel Module

* Kernel module compilation
* Character-device registration
* File-operation implementation
* Device creation logic
* Driver cleanup logic

### Integration

* `/dev/system_monitor` creation
* `open()` communication
* `write()` command transfer
* `read()` response transfer
* `release()` handling

Actual kernel-module integration requires a compatible Linux environment with permission to load kernel modules.

---

## Limitations

This is an educational system-programming project rather than a production monitoring tool.

Current limitations include:

* Basic system monitoring
* No graphical user interface
* No database
* No remote monitoring
* No hardware control
* Limited driver commands
* Exact runtime values depend on the Linux environment
* Kernel module must be built for an appropriate target kernel

---

## Future Enhancements

Possible future improvements include:

* Real-time monitoring
* Disk usage monitoring
* Network monitoring
* CPU temperature monitoring
* GPU information
* Graphical user interface
* Historical system statistics
* Additional driver commands
* Configurable monitoring intervals
* Advanced kernel statistics

---

## License

This project is licensed under the MIT License.

See the [LICENSE](LICENSE) file for details.

---

## Author

**Subham Kumar Tripathy**

B.Tech — Computer Science and Engineering
