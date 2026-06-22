# Changes Made

## Prompt 1 — Part 5: System Programs

### New files created

**`source/system_programs/sys.c`**
Prints OS information to the terminal (like a minimal neofetch).
Reads `/proc/meminfo` for total RAM, `/proc/cpuinfo` for the CPU model name,
calls `uname()` for OS/kernel/hostname, and `getpwuid(getuid())` for the
current username. Prints everything in a coloured box using the colour macros
already defined in `system_program.h`.

**`source/system_programs/dspawn.c`**
Spawns a background daemon process and immediately returns control to the shell.
Follows the standard Unix double-fork recipe:
1. First fork — parent exits so the shell gets its prompt back.
2. `setsid()` — child becomes a new session leader with no controlling terminal.
3. Ignore `SIGCHLD` and `SIGHUP`.
4. Second fork — grandchild is no longer a session leader, so it can never accidentally acquire a terminal.
5. `umask(0)`, `chdir("/")` — clean up inherited state.
6. Close all open file descriptors and redirect stdin/stdout/stderr to `/dev/null`.
7. `daemon_work()` — logs PID, PPID, and working directory to `dspawn.log` in the
   project directory, then writes "Daemon writing line N" every 10 seconds for
   10 iterations.

The project directory path is captured with `getcwd()` before the first fork so it
is still known after `chdir("/")`.

**`source/system_programs/dcheck.c`**
Reports how many dspawn daemons are currently running.
Uses `popen()` to run `ps -efj | grep dspawn | grep -Ev 'tty|pts|grep'`,
prints each matching line, then prints the total count.

**`source/system_programs/backup.c`**
Compresses a directory or file into a dated `.tar.gz` archive.
1. Reads the `BACKUP_DIR` environment variable; prints an error and exits if unset.
2. Gets the current date/time with `strftime()` to embed in the filename.
3. Creates `./archive/` with `mkdir()` if it does not already exist.
4. Runs `tar -czf archive/<name>_<datetime>.tar.gz <BACKUP_DIR>` via `system()`.

### Fixes to existing files

**`source/shell.c`** — removed the `static` keyword from `process_rc_file`.
It was declared non-static in `shell.h` but defined static in the `.c` file,
which is a C compile error. One word removed; no logic changed.

---

## Prompt 2 — Testing

### New files created

**`tests/integration/test_sys.sh`**
Runs `sys` through the shell and checks that all five expected fields appear in
the output: `OS:`, `Kernel:`, `User:`, `CPU:`, `Memory:`.

**`tests/integration/test_dspawn.sh`**
Runs `dspawn` through the shell inside a `timeout 5s` wrapper. If the daemon
does not detach, the shell would hang and the test would fail. After the shell
exits it sleeps 1 second then checks that `dspawn.log` was created and contains
the "Daemon started" startup message. A `trap` cleans up the log file and any
leftover daemon processes when the test exits.

**`tests/integration/test_dcheck.sh`**
Runs `dcheck` through the shell and verifies the output contains the line
`Active dspawn daemons:`. Does not assert a specific count because other tests
may leave daemons running in the background.

**`tests/integration/test_backup.sh`**
Two sub-tests in one script:
- With `BACKUP_DIR` unset: checks that the error output mentions `BACKUP_DIR`.
  Uses `2>&1` to capture stderr alongside stdout because the error goes to stderr.
- With `BACKUP_DIR=./files`: checks the shell output mentions `.tar.gz` and that
  a file actually exists in `./archive/`. A `trap` removes `./archive/` on exit.

### Fixes to existing files

**`makefile`** — added `-I$(INC_DIR)/libs` to `TEST_CFLAGS`.
The unit test files include `"perms.h"` and `"rc_parser.h"` directly, but those
headers live under `includes/libs/`. The compiler could not find them, so every
unit test failed to compile. Adding the subdirectory to the include path fixes it
without touching any test source file.

**`source/libs/rc_parser.c`** — implemented `classify_rc_line()`.
The file shipped as a stub that returned `RC_LINE_EMPTY` for every input, making
all 7 `test_rc_parser` unit tests fail. The implementation:
- Skips leading whitespace; if nothing remains, returns `RC_LINE_EMPTY`.
- If the line starts with the exact string `"PATH="`, returns `RC_LINE_PATH` with
  `*value` pointing to the substring after the `=`.
- Everything else is `RC_LINE_COMMAND`; `*value` points to the first
  non-whitespace character.

**`README.md`** — added a "Testing Note" section documenting unit test coverage,
integration test scenarios, and AI usage, as required by the submission checklist.

---

## Prompt 3 — Part 6: Additional Feature and Submission Requirements

### New files created

**`tests/integration/test_history.sh`**
Runs `ld`, `cd files`, then `history` through the shell. Checks that both earlier
commands appear in the output and that at least one line matches the numbered
format (`  N  command`).

### Changes to existing files

**`includes/shell.h`**
- Added `#define MAX_HISTORY 10`.
- Declared `void add_to_history(char **cmd)` and `int shell_history(char **args)`.

**`source/shell.c`**
- Added `"history"` to `builtin_commands[]`.
- Added `"history - Show the last 10 commands entered."` to `builtin_usage[]`.
- Added `&shell_history` to `builtin_command_func[]`.
- Implemented `add_to_history(char **cmd)`:
  Joins the command tokens back into a single string and stores it in a static
  circular array of `MAX_HISTORY` slots. When the array is full, the oldest slot
  is overwritten. Uses `strdup()` for storage and `free()` before overwriting.
- Implemented `shell_history(char **args)`:
  Walks the circular buffer in chronological order and prints each entry with a
  global sequence number, matching the style of bash `history`.

**`source/main.c`**
- Added one line — `add_to_history(cmd)` — inside the `if (cmd[0] != NULL)` block
  before the builtin/external dispatch. This ensures every non-empty command is
  recorded regardless of whether it is a builtin or an external program.

**`README.md`**
- Updated "System Programs" list to include `sys`, `dspawn`, `dcheck`, `backup`.
- Added "Additional Features" section explaining the `history` builtin with a live
  demo snippet and a plain-English explanation of how it works (suitable for the
  60-second Q&A).
- Added "Sustainability and Inclusivity" section (mandatory for full marks):
  covers lightweight resource use (no persistent background processes, compressed
  backups, daemon self-terminates) and inclusive design (POSIX/locale-neutral
  interfaces, human-readable error messages, in-shell `usage` documentation).
- Updated the "Testing Note" section to include `test_history.sh` and the updated
  AI usage disclosure.
