#!/usr/bin/env bash
# tests/integration/test_backup.sh
#
# Verifies two behaviors of the backup program:
#   1. Prints an informative error when BACKUP_DIR is not set.
#   2. Creates a dated .tar.gz file in archive/ when BACKUP_DIR is set.

set -euo pipefail

# Remove any archive directory from a previous run so this test is clean.
trap 'rm -rf ./archive' EXIT

# ── Test 1: backup without BACKUP_DIR should print an error ──────────────────
# We explicitly unset the variable in case it is set in the outer environment.
OUTPUT=$(printf "unsetenv BACKUP_DIR\nbackup\nexit\n" | timeout 3s ./cseshell 2>&1)

if ! echo "$OUTPUT" | grep -iF "BACKUP_DIR" > /dev/null; then
  echo "FAIL: backup did not mention BACKUP_DIR when it was unset"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

# ── Test 2: backup with BACKUP_DIR=./files should create an archive ──────────
OUTPUT=$(printf "setenv BACKUP_DIR=./files\nbackup\nexit\n" | timeout 5s ./cseshell)

if ! echo "$OUTPUT" | grep -F ".tar.gz" > /dev/null; then
  echo "FAIL: backup did not report a .tar.gz file being created"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

if ! ls ./archive/*.tar.gz > /dev/null 2>&1; then
  echo "FAIL: no .tar.gz file found in ./archive/"
  exit 1
fi

echo "PASS: backup handles missing BACKUP_DIR and creates the archive correctly"
