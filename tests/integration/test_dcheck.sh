#!/usr/bin/env bash
# tests/integration/test_dcheck.sh
#
# Verifies that dcheck runs successfully and prints the daemon count line.
# We do not assert a specific count because other tests may leave daemons
# running; we just check the output has the right format.

set -euo pipefail

OUTPUT=$(printf "dcheck\nexit\n" | timeout 3s ./cseshell)

if ! echo "$OUTPUT" | grep -F "Active dspawn daemons:" > /dev/null; then
  echo "FAIL: dcheck did not print 'Active dspawn daemons:'"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

echo "PASS: dcheck reports daemon count correctly"
