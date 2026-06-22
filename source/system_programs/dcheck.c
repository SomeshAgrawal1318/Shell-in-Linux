#include "system_program.h"

int main()
{
    // Look for dspawn processes that have no controlling terminal.
    // -Ev 'tty|pts|grep' removes:
    //   • lines that have a terminal (real user processes)
    //   • the grep command itself (so we don't count ourselves)
    const char *cmd = "ps -efj | grep dspawn | grep -Ev 'tty|pts|grep'";

    FILE *pipe = popen(cmd, "r");
    if (!pipe)
    {
        perror("popen");
        return 1;
    }

    int count = 0;
    char line[512];
    while (fgets(line, sizeof(line), pipe))
    {
        printf("%s", line);
        count++;
    }
    pclose(pipe);

    printf("\nActive dspawn daemons: %d\n", count);
    return 0;
}
