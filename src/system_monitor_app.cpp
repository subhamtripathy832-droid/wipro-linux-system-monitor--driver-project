#include <algorithm>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

namespace {

struct CpuSample {
    unsigned long long total = 0;
    unsigned long long idle = 0;
};

bool readCpuSample(CpuSample &sample) {
    std::ifstream file("/proc/stat");
    std::string line;
    if (!file || !std::getline(file, line)) {
        return false;
    }

    std::istringstream input(line);
    std::string cpu;
    unsigned long long user = 0, nice = 0, system = 0, idle = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0;

    input >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    if (cpu != "cpu") {
        return false;
    }

    sample.idle = idle + iowait;
    sample.total = user + nice + system + idle + iowait + irq + softirq + steal;
    return true;
}

double getCpuUsage() {
    CpuSample first;
    CpuSample second;

    if (!readCpuSample(first)) {
        return -1.0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (!readCpuSample(second)) {
        return -1.0;
    }

    const auto totalDelta = second.total - first.total;
    const auto idleDelta = second.idle - first.idle;

    if (totalDelta == 0) {
        return 0.0;
    }

    const double busy = static_cast<double>(totalDelta - std::min(idleDelta, totalDelta));
    return (busy * 100.0) / static_cast<double>(totalDelta);
}

bool readMemory(long long &totalKb, long long &availableKb) {
    std::ifstream file("/proc/meminfo");
    std::string key;
    long long value = 0;
    std::string unit;

    totalKb = 0;
    availableKb = 0;

    while (file >> key >> value >> unit) {
        if (key == "MemTotal:") {
            totalKb = value;
        } else if (key == "MemAvailable:") {
            availableKb = value;
        }

        if (totalKb > 0 && availableKb > 0) {
            return true;
        }
    }

    return false;
}

int countProcesses() {
    DIR *dir = opendir("/proc");
    if (!dir) {
        return -1;
    }

    int count = 0;
    while (dirent *entry = readdir(dir)) {
        const char *name = entry->d_name;
        if (*name == '\0') {
            continue;
        }

        bool numeric = true;
        for (const char *p = name; *p; ++p) {
            if (!std::isdigit(static_cast<unsigned char>(*p))) {
                numeric = false;
                break;
            }
        }

        if (numeric) {
            ++count;
        }
    }

    closedir(dir);
    return count;
}

std::string formatMiB(long long kib) {
    const double mib = static_cast<double>(kib) / 1024.0;
    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << mib << " MB";
    return out.str();
}

bool communicateWithDriver(std::string &driverResponse) {
    const char devicePath[] = "/dev/system_monitor";
    const char command[] = "status\n";

    int writeFd = open(devicePath, O_WRONLY);
    if (writeFd < 0) {
        return false;
    }

    const ssize_t written = write(writeFd, command, sizeof(command) - 1);
    const int writeErrno = errno;
    close(writeFd);

    if (written < 0) {
        errno = writeErrno;
        return false;
    }

    int readFd = open(devicePath, O_RDONLY);
    if (readFd < 0) {
        return false;
    }

    char buffer[512]{};
    const ssize_t bytesRead = read(readFd, buffer, sizeof(buffer) - 1);
    const int readErrno = errno;
    close(readFd);

    if (bytesRead < 0) {
        errno = readErrno;
        return false;
    }

    buffer[bytesRead] = '\0';
    driverResponse = buffer;
    return true;
}

void printReport(bool demoMode) {
    std::cout << "==================================================\n";
    std::cout << "          LINUX SYSTEM MONITOR\n";
    std::cout << "==================================================\n\n";

    const double cpuUsage = getCpuUsage();
    long long totalKb = 0;
    long long availableKb = 0;
    const bool memoryOk = readMemory(totalKb, availableKb);
    const int processCount = countProcesses();

    std::cout << "SYSTEM RESOURCES\n";
    std::cout << "--------------------------------------------------\n";
    if (cpuUsage >= 0.0) {
        std::cout << "CPU Usage       : " << std::fixed << std::setprecision(1)
                  << cpuUsage << "%\n";
    } else {
        std::cout << "CPU Usage       : unavailable\n";
    }

    if (memoryOk) {
        const long long usedKb = totalKb - availableKb;
        const double usedPercent = totalKb > 0
            ? (static_cast<double>(usedKb) * 100.0 / static_cast<double>(totalKb))
            : 0.0;

        std::cout << "Memory Total    : " << formatMiB(totalKb) << "\n";
        std::cout << "Memory Used     : " << formatMiB(usedKb) << " ("
                  << std::fixed << std::setprecision(1) << usedPercent << "%)\n";
        std::cout << "Memory Available : " << formatMiB(availableKb) << "\n";
    } else {
        std::cout << "Memory          : unavailable\n";
    }

    if (processCount >= 0) {
        std::cout << "Processes       : " << processCount << "\n";
    } else {
        std::cout << "Processes       : unavailable\n";
    }

    std::cout << "\nDRIVER COMMUNICATION\n";
    std::cout << "--------------------------------------------------\n";

    if (demoMode) {
        std::cout << "Mode            : DEMO MODE\n";
        std::cout << "Device          : /dev/system_monitor (not accessed)\n";
        std::cout << "Status          : Application test completed\n";
    } else {
        std::string response;
        if (communicateWithDriver(response)) {
            std::cout << response;
        } else {
            std::cout << "Status          : Unable to access /dev/system_monitor\n";
            std::cout << "Hint            : Load the kernel module first.\n";
        }
    }

    std::cout << "\n==================================================\n";
}

} // namespace

int main(int argc, char *argv[]) {
    bool demoMode = false;

    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [--demo]\n";
        return 1;
    }

    if (argc == 2) {
        if (std::string(argv[1]) == "--demo") {
            demoMode = true;
        } else {
            std::cerr << "Unknown option: " << argv[1] << "\n";
            std::cerr << "Usage: " << argv[0] << " [--demo]\n";
            return 1;
        }
    }

    printReport(demoMode);
    return 0;
}
