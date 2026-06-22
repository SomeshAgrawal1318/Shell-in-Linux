#!/usr/bin/env bash
# tests/integration/test_dspawn.sh
#
# Verifies that dspawn:
#   - Returns control to the shell immediately (the daemon detaches properly).
#     If the shell hangs instead of returning, timeout will kill it at 5s.
#   - Creates dspawn.log in the project directory with a startup message.

set -euo pipefail

rm -f ./dspawn.log

# Kill any leftover dspawn daemons and remove the log when the test exits,
# whether it passes or fails.
trap 'pkill -f "./bin/dspawn" 2>/dev/null || true; rm -f ./dspawn.log' EXIT

# Run dspawn through the shell. A correct daemon detaches immediately so the
# shell can exit within the timeout. If it doesn't detach, this will fail.
OUTPUT=$(printf "dspawn\nexit\n" | timeout 5s ./cseshell)

# Give the daemon a moment to write its first log line.
sleep 1

if [[ ! -f ./dspawn.log ]]; then
  echo "FAIL: dspawn.log was not created"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

if ! grep -F "Daemon started" ./dspawn.log > /dev/null; then
  echo "FAIL: dspawn.log does not contain the expected startup message"
  echo "----- log contents -----"
  cat ./dspawn.log
  echo "------------------------"
  exit 1
fi

echo "PASS: dspawn detaches from the shell and writes to dspawn.log"
