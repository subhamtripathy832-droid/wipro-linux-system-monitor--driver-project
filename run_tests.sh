#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
APP="$ROOT_DIR/system_monitor"
MODULE="$ROOT_DIR/system_monitor.ko"
PASS=0
FAIL=0

pass() {
    echo "[PASS] $1"
    PASS=$((PASS + 1))
}

fail() {
    echo "[FAIL] $1"
    FAIL=$((FAIL + 1))
}

echo "========================================"
echo " Linux System Monitor - Test Suite"
echo "========================================"

echo
echo "[1] Checking required source files..."
for f in \
    "$ROOT_DIR/src/system_monitor.c" \
    "$ROOT_DIR/src/system_monitor_app.cpp" \
    "$ROOT_DIR/Makefile"; do
    if [ -f "$f" ]; then
        pass "Found $(basename "$f")"
    else
        fail "Missing $(basename "$f")"
    fi
done

echo
echo "[2] Building the user-space application..."
if make -C "$ROOT_DIR" app >/tmp/linux_system_monitor_app_build.log 2>&1; then
    pass "C++ application build completed"
else
    fail "C++ application build failed"
    cat /tmp/linux_system_monitor_app_build.log
fi

echo
echo "[3] Checking kernel module build prerequisites..."
KDIR="${KDIR:-/lib/modules/$(uname -r)/build}"
if [ -d "$KDIR" ]; then
    if make -C "$ROOT_DIR" driver >/tmp/linux_system_monitor_driver_build.log 2>&1; then
        pass "Kernel module build completed"
    else
        fail "Kernel module build failed"
        cat /tmp/linux_system_monitor_driver_build.log
    fi
else
    echo "[INFO] Matching kernel headers/build directory not available: $KDIR"
    echo "[INFO] Kernel module build is skipped in this environment."
fi

echo
echo "[4] Checking user-space executable..."
if [ -x "$APP" ]; then
    pass "system_monitor executable exists"
else
    fail "system_monitor executable not found"
fi

echo
echo "[5] Running demo mode..."
if [ -x "$APP" ] && "$APP" --demo >/tmp/linux_system_monitor_demo.log 2>&1; then
    if grep -q "LINUX SYSTEM MONITOR" /tmp/linux_system_monitor_demo.log; then
        pass "Demo mode executed successfully"
    else
        fail "Demo mode ran but expected output was not found"
    fi
else
    fail "Demo mode execution failed"
fi

echo
echo "[6] Checking kernel module artifact..."
if [ -f "$MODULE" ]; then
    pass "Kernel module system_monitor.ko exists"
else
    echo "[INFO] Kernel module was not produced. This may be expected if matching kernel headers are unavailable."
fi

echo
echo "[7] Checking driver source for required file operations..."
DRIVER="$ROOT_DIR/src/system_monitor.c"
for op in "monitor_open" "monitor_read" "monitor_write" "monitor_release"; do
    if grep -q "${op}" "$DRIVER"; then
        pass "Driver implements ${op}()"
    else
        fail "Driver operation ${op}() not found"
    fi
done

echo
echo "========================================"
echo " Tests completed"
echo " Passed: $PASS"
echo " Failed: $FAIL"
echo "========================================"

if [ "$FAIL" -eq 0 ]; then
    exit 0
else
    exit 1
fi
