[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/CBJ_yXMW)
# CSEShell

CSEShell is a simple, custom shell for Unix-based systems, designed to provide an interface for executing system programs. This project includes a basic shell implementation, a set of system programs (`find`, `ld`, `ldr`), and some test files.

## Directory Structure

The project is organized as follows:

- `bin/` - Contains compiled executables for system programs.
  - `find` - Program to find files.
  - `ld` - Program for listing the contents of the current directory.
  - `ldr` - Program for listing the contents of the current directory recursively.
- `cseshell` - The main executable for the CSEShell.
- `files/` - Contains various test files used with the shell and system programs.
  - `combined.txt`, `file1.txt`, `file2.txt`, ... - Test text files.
  - `notes.pdf` - A PDF file for testing.
  - `ss.png` - An image file.
- `makefile` - Makefile for building the CSEShell and system programs.
- `source/` - Source code for the shell and system programs.
  - `libs/` - Contain matching `foo.h` and `foo.c` helper functions, unit-testable.
  - `shell.c` and `shell.h` - Source and header files for the shell.
  - `system_programs/` - Source code and header for the system programs.
- `tests/` - Unit and integration tests.
  - `unit/` - C unit tests using the Unity framework.
  - `integration/` - Bash scripts that run `./cseshell` as a black box.
  - `unity/` - Vendored Unity test framework (`unity.c`, `unity.h`, `unity_internals.h`).
- `scripts/` - Helper scripts, including the AI-assisted test generator.
- `prompts/` - Prompt templates used by the AI test generator.
- `AGENTS.md` - Guide for AI coding agents working in this project.

## Building the Project

To build the CSEShell and system programs, run the following command in the root directory:

```bash
make
```

This will compile the source code and place the executable files in the appropriate directories.

## Running CSEShell

After building, you can start the shell by running:

```bash
./cseshell
```

From there, you can execute built-in commands and any of the included system programs (e.g., `find`, `ld`, `ldr`).

## System Programs

- `find.c` - Searches for files in a directory.
- `ld.c` - List the contents of the current directory.
- `ldr.c` - List the contents of the current directory recursively.
- `sys.c` - Displays OS, kernel, user, CPU, and memory info.
- `dspawn.c` - Spawns a background daemon process that logs to `dspawn.log`.
- `dcheck.c` - Reports the number of active dspawn daemons.
- `backup.c` - Compresses the directory in `$BACKUP_DIR` into a dated archive.

## Additional Features

### Command History (`history`)

CSEShell keeps a running record of the last 10 commands entered. Type `history` at any time to see them listed with sequence numbers, just like in bash. The history wraps around once 10 entries are full so the most recent commands are always visible.

```
$$ ld
$$ cd files
$$ history
  1  ld
  2  cd files
  3  history
```

**How it works** (easy to explain in the demo):

1. `add_to_history(cmd)` in `shell.c` joins the command tokens back into a single string and stores it in a circular array of 10 slots.
2. This is called once per command from the main loop in `main.c`, before dispatch.
3. The `history` builtin iterates the array in order and prints each entry with its sequence number.

## Sustainability and Inclusivity

**Sustainability**: CSEShell is intentionally lightweight. It spawns no background threads and holds no persistent resources. The `dspawn` daemon writes to disk only every 10 seconds and terminates after 10 iterations rather than running indefinitely. The `backup` program produces compressed archives, keeping storage use low. No GUI or heavy framework dependencies are required — `make && ./cseshell` is the entire deployment.

**Inclusivity**: All interfaces use standard ASCII and POSIX system calls so the shell works identically on any Unix-like OS regardless of locale or language settings. Error messages explain what went wrong and what to do next (e.g., `export BACKUP_DIR=...`) rather than printing numeric codes. The `usage` builtin provides in-shell documentation for every command so users do not need to consult external man pages.

Each program can be executed from the CSEShell once it is running. This starter code only allows the shell to execute a command once before exiting because `execv` replace the entire process' address space. Students need to fix this and allow the shell to prompt for more commands in Programming Assignment 1.

## Files Directory

The `files/` directory contains various text, PDF, and image files for testing the functionality of the CSEShell and its system programs.

## Makefile

The Makefile contains rules for compiling the shell and system programs. You can clean the build by running:

```bash
make clean
```

## Source Directory

Contains all the necessary source code for the shell and system programs. It is divided into the shell implementation (`shell.c`, `shell.h`) and system programs (`system_programs/`).

## Testing

This project ships with two layers of tests:

- **Unit tests** in `tests/unit/`. Small C programs that exercise pure helper functions directly, using the Unity framework. You can create any matching `test_foo.c` under `tests/unit` to test any libs under `source/libs/foo.c` by including the matching `source/libs/foo.h` header file in the unit test. See `tests/unit/test_perms.c` (or `test_rc_parser.c`) for example.
- **Integration tests** in `tests/integration/`. Bash scripts that run the compiled `./cseshell` as a subprocess, feed it `stdin`, and check `stdout`.
  - You should create your own integration tests. These samples given are just samples, adjust it accordingly.

Run all tests:

```bash
make test
```

Run only unit tests:

```bash
make unit
```

Run only integration tests (requires that `make` has been run first so `cseshell` and the system program binaries exist):

```bash
make integration
```

For an explanation of what to test and how to structure your testable code, see the testing handout.

## Testing Note

### Unit tests

- **`test_perms.c`** — Tests `perms_to_string()` in `source/libs/perms.c`. Covers: empty mode (all dashes), a regular 0644 file, a directory 0755, and a file with only owner permissions. All 4 pass.
- **`test_rc_parser.c`** — Tests `classify_rc_line()` in `source/libs/rc_parser.c`. Covers: empty string, whitespace-only line, `PATH=value`, `PATH=` with no value, a line starting with `PATH` but no `=` (should be a command, not a PATH line), a plain command, and a command with leading whitespace. All 7 pass.

Functions not suited for unit testing here: `fork`/`execv` in the shell loop, daemon creation in `dspawn`, and any function that requires a real process or filesystem side effect.

### Integration tests

- **`test_exit.sh`** — `exit` terminates the shell cleanly within 3 s.
- **`test_loop.sh`** — Shell loops over multiple commands and survives an unknown command without crashing.
- **`test_builtin_help.sh`** — `help` lists all seven builtin commands.
- **`test_builtin_cd.sh`** — `cd files` followed by `ld` shows files inside that directory.
- **`test_builtin_env.sh`** — `setenv`/`env`/`unsetenv` round-trip: variable appears then disappears.
- **`test_system_programs_bundled.sh`** — `ld`, `ldr`, and `find` run through the shell and produce expected output.
- **`test_sys.sh`** — `sys` prints all five expected fields: OS, Kernel, User, CPU, Memory.
- **`test_dspawn.sh`** — `dspawn` returns control to the shell immediately (daemon detaches) and creates `dspawn.log` with a startup message.
- **`test_dcheck.sh`** — `dcheck` runs and prints the "Active dspawn daemons:" summary line.
- **`test_backup.sh`** — `backup` without `BACKUP_DIR` prints an error mentioning the variable; with `BACKUP_DIR=./files` it creates a `.tar.gz` file inside `archive/`.
- **`test_history.sh`** — runs `ld`, `cd files`, then `history`; checks that both previous commands appear in the numbered output.

### AI use

AI (Claude) assisted with: implementing the four Part 5 system programs (`sys`, `dspawn`, `dcheck`, `backup`), implementing `classify_rc_line`, the `history` additional feature, writing all integration tests for Part 5 and the history feature, and fixing two pre-existing bugs (`static` mismatch on `process_rc_file`, wrong include path in `TEST_CFLAGS`). All generated code was reviewed and understood before submission.

### AI-Assisted Unit Test Generation

This project includes an optional wrapper for drafting unit tests with an AI agent. After writing a helper, you can run:

```bash
make ai-unit-tests MODULE=<helper_name>
```

This invokes `scripts/gen_unit_tests.sh`, which builds a prompt from `prompts/generate-unit-tests.md` and `AGENTS.md`. The script either pipes the prompt to your configured agent (via the `CSESHELL_AGENT_CMD` environment variable) or prints it to stdout for you to paste into a chat interface. See `AGENTS.md` for setup details and the rules agents follow.

You remain responsible for every test in your submission. You must be able to explain each test during checkoff.
