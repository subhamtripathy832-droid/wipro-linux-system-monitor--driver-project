#!/bin/bash
set -e

if [[ "$EUID" -ne 0 ]]; then
    echo "Please run as root: sudo $0"
    exit 1
fi

rmmod system_monitor

echo "Driver unloaded successfully."
