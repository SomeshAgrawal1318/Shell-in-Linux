#include "system_program.h"

int main()
{
    struct utsname uts;
    uname(&uts);

    // Read total memory from /proc/meminfo (Linux-specific)
    long total_mem_kb = 0;
    FILE *memfile = fopen("/proc/meminfo", "r");
    if (memfile)
    {
        char memline[128];
        while (fgets(memline, sizeof(memline), memfile))
        {
            if (sscanf(memline, "MemTotal: %ld kB", &total_mem_kb) == 1)
                break;
        }
        fclose(memfile);
    }

    // Read the first CPU model name from /proc/cpuinfo
    char cpu_model[256] = "Unknown";
    FILE *cpufile = fopen("/proc/cpuinfo", "r");
    if (cpufile)
    {
        char cpuline[256];
        while (fgets(cpuline, sizeof(cpuline), cpufile))
        {
            if (strncmp(cpuline, "model name", 10) == 0)
            {
                char *colon = strchr(cpuline, ':');
                if (colon)
                {
                    strncpy(cpu_model, colon + 2, sizeof(cpu_model) - 1);
                    cpu_model[strcspn(cpu_model, "\n")] = '\0';
                }
                break;
            }
        }
        fclose(cpufile);
    }

    // Get the current logged-in username from the password database
    struct passwd *pw = getpwuid(getuid());
    const char *username = pw ? pw->pw_name : "unknown";

    printf(COLOR_CYAN "========== System Info ==========\n" COLOR_RESET);
    printf(COLOR_GREEN "OS:      " COLOR_RESET "%s %s\n", uts.sysname, uts.release);
    printf(COLOR_GREEN "Kernel:  " COLOR_RESET "%s\n", uts.version);
    printf(COLOR_GREEN "Host:    " COLOR_RESET "%s\n", uts.nodename);
    printf(COLOR_GREEN "User:    " COLOR_RESET "%s\n", username);
    printf(COLOR_GREEN "CPU:     " COLOR_RESET "%s\n", cpu_model);
    printf(COLOR_GREEN "Memory:  " COLOR_RESET "%ld MB total\n", total_mem_kb / 1024);
    printf(COLOR_CYAN "=================================\n" COLOR_RESET);

    return 0;
}
