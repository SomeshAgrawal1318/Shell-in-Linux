#!/usr/bin/env bash
# tests/integration/test_sys.sh
#
# Verifies that the `sys` program prints the expected system info fields.

set -euo pipefail

OUTPUT=$(printf "sys\nexit\n" | timeout 3s ./cseshell)

MISSING=""
for field in "OS:" "Kernel:" "User:" "CPU:" "Memory:"; do
  if ! echo "$OUTPUT" | grep -F "$field" > /dev/null; then
    MISSING="$MISSING $field"
  fi
done

if [[ -n "$MISSING" ]]; then
  echo "FAIL: sys output is missing fields:$MISSING"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

echo "PASS: sys prints all expected system info fields"
