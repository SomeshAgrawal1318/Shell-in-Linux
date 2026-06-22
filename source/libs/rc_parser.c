/*
 * rc_parser.c
 *
 * Stub implementation. The current code returns RC_LINE_EMPTY for every
 * input, which makes the unit tests in tests/unit/test_rc_parser.c fail.
 * Your task (if you choose to use this helper) is to make the tests pass.
 *
 * The expected behaviour is documented in rc_parser.h. Run:
 *
 *   make unit
 *
 * to see which tests fail, then fix the implementation until they pass.
 *
 * This file is OPTIONAL. See the comment at the top of rc_parser.h.
 */

#include "libs/rc_parser.h"

#include <ctype.h>
#include <string.h>

rc_line_type_t classify_rc_line(const char *line, const char **value)
{
    /* Skip leading whitespace to check if the whole line is blank */
    const char *p = line;
    while (*p && isspace((unsigned char)*p))
        p++;

    if (*p == '\0') {
        *value = NULL;
        return RC_LINE_EMPTY;
    }

    /* A PATH line must start with the exact string "PATH=" (no leading spaces).
     * Something like "PATHETIC" does NOT match because there is no '=' at index 4. */
    if (strncmp(line, "PATH=", 5) == 0) {
        *value = line + 5;   /* point to whatever comes after "PATH=" */
        return RC_LINE_PATH;
    }

    /* Everything else is a command; skip leading whitespace in the value */
    *value = p;
    return RC_LINE_COMMAND;
}
