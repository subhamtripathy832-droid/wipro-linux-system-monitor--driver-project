#!/bin/bash
set -e

echo "=== Module file ==="
ls -l src/system_monitor.ko 2>/dev/null || echo "Module has not been built yet."

echo

echo "=== Device node ==="
if [[ -e /dev/system_monitor ]]; then
    ls -l /dev/system_monitor
else
    echo "/dev/system_monitor is not present."
fi

echo

echo "=== Kernel log ==="
dmesg | grep "system_monitor" | tail -20 || true
