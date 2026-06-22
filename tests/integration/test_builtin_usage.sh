#!/usr/bin/env bash
# tests/integration/test_builtin_usage.sh
#
# Verifies that `usage` prints help for all builtins and that
# `usage <cmd>` prints help for a specific builtin.

set -euo pipefail

OUTPUT=$(printf "usage\nexit\n" | timeout 3s ./cseshell)

# usage with no argument must list all builtins
for cmd in cd help exit usage env setenv unsetenv history; do
  if ! echo "$OUTPUT" | grep -F "$cmd" > /dev/null; then
    echo "FAIL: 'usage' did not mention '$cmd'"
    echo "----- shell output -----"
    echo "$OUTPUT"
    echo "------------------------"
    exit 1
  fi
done

# usage <cmd> must print specific help text for that builtin
OUTPUT2=$(printf "usage cd\nexit\n" | timeout 3s ./cseshell)
if ! echo "$OUTPUT2" | grep -iF "cd" > /dev/null; then
  echo "FAIL: 'usage cd' did not print cd-specific help"
  echo "----- shell output -----"
  echo "$OUTPUT2"
  echo "------------------------"
  exit 1
fi

echo "PASS: usage lists all builtins and shows per-command help"
