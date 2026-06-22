#!/usr/bin/env bash
# tests/integration/test_history.sh
#
# Verifies that the history builtin:
#   - Records commands entered in the session.
#   - Prints them numbered when 'history' is called.

set -euo pipefail

OUTPUT=$(printf "ld\ncd files\nhistory\nexit\n" | timeout 3s ./cseshell)

# history should have recorded 'ld' and 'cd files' before it was called
if ! echo "$OUTPUT" | grep -F "ld" > /dev/null; then
  echo "FAIL: history did not show 'ld'"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

if ! echo "$OUTPUT" | grep -F "cd files" > /dev/null; then
  echo "FAIL: history did not show 'cd files'"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

# Output should contain a numbered line (e.g. "  1  ld")
if ! echo "$OUTPUT" | grep -E "^ *[0-9]+  " > /dev/null; then
  echo "FAIL: history output does not have numbered entries"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

echo "PASS: history records and displays previous commands"
