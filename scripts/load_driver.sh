#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

if [[ "$EUID" -ne 0 ]]; then
    echo "Please run as root: sudo $0"
    exit 1
fi

cd "$PROJECT_DIR"

if [[ ! -f "src/system_monitor.ko" ]]; then
    echo "Kernel module not found. Run 'make driver' first."
    exit 1
fi

insmod src/system_monitor.ko
sleep 1

if [[ -e /dev/system_monitor ]]; then
    echo "Driver loaded successfully."
    ls -l /dev/system_monitor
else
    echo "Driver loaded, but /dev/system_monitor was not created."
    exit 1
fi
