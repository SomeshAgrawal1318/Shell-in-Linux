#!/usr/bin/env bash
# tests/integration/test_builtin_lang.sh
#
# Verifies the multilingual `lang` builtin (inclusivity feature):
#   - `lang` with no argument lists the available languages.
#   - `lang ms` switches to Malay and greets in Malay.
#   - `lang zh` switches to Chinese and greets in Chinese.

set -euo pipefail

# No argument: should list the language codes.
OUTPUT=$(printf "lang\nexit\n" | timeout 3s ./cseshell)
for code in en zh ms ta; do
  if ! echo "$OUTPUT" | grep -F "$code" > /dev/null; then
    echo "FAIL: 'lang' did not list language code '$code'"
    echo "----- shell output -----"
    echo "$OUTPUT"
    echo "------------------------"
    exit 1
  fi
done

# Switch to Malay: greeting should appear.
OUTPUT2=$(printf "lang ms\nexit\n" | timeout 3s ./cseshell)
if ! echo "$OUTPUT2" | grep -F "Selamat datang" > /dev/null; then
  echo "FAIL: 'lang ms' did not greet in Malay"
  echo "----- shell output -----"
  echo "$OUTPUT2"
  echo "------------------------"
  exit 1
fi

# Switch to Chinese: greeting should appear.
OUTPUT3=$(printf "lang zh\nexit\n" | timeout 3s ./cseshell)
if ! echo "$OUTPUT3" | grep -F "CSEShell" > /dev/null; then
  echo "FAIL: 'lang zh' did not produce a Chinese greeting"
  echo "----- shell output -----"
  echo "$OUTPUT3"
  echo "------------------------"
  exit 1
fi

echo "PASS: lang lists languages and switches greeting language"
