#include "system_program.h"

// This runs inside the fully detached daemon process.
// It logs its identity and then writes a line every 10 seconds for 10 rounds.
void daemon_work(const char *project_dir)
{
    char log_path[PATH_MAX];
    snprintf(log_path, sizeof(log_path), "%s/dspawn.log", project_dir);

    // We open the log with "a" (append) so multiple daemons can all write to
    // the same file without overwriting each other.
    FILE *log = fopen(log_path, "a");
    if (!log)
        return;

    fprintf(log, "--- Daemon started: PID=%d, PPID=%d ---\n", getpid(), getppid());

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)))
        fprintf(log, "Working directory: %s\n", cwd);

    fflush(log);

    for (int i = 1; i <= 10; i++)
    {
        fprintf(log, "Daemon writing line %d\n", i);
        fflush(log);
        sleep(10);
    }

    fprintf(log, "--- Daemon done: PID=%d ---\n", getpid());
    fclose(log);
}

int main()
{
    // Grab the project directory NOW, before we chdir("/") later.
    // After chdir we'd lose track of where to write the log.
    char project_dir[PATH_MAX];
    if (getcwd(project_dir, sizeof(project_dir)) == NULL)
    {
        perror("getcwd");
        return 1;
    }

    // ── Step 1-2: First fork ──────────────────────────────────────────────
    // The parent exits immediately so the shell gets its prompt back.
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid > 0) return 0;   // parent exits → shell continues

    // ── Step 3: Become a session leader ──────────────────────────────────
    // setsid() creates a new session and detaches from the controlling terminal.
    if (setsid() < 0)
        return 1;

    // ── Step 4: Ignore signals ────────────────────────────────────────────
    // SIGCHLD SIG_IGN  → children we create won't become zombies
    // SIGHUP  SIG_IGN  → we won't die when our session leader exits later
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP,  SIG_IGN);

    // ── Step 5: Second fork ───────────────────────────────────────────────
    // By forking again, the actual daemon is no longer the session leader,
    // so it can never accidentally acquire a controlling terminal.
    pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid > 0) return 0;   // first child exits

    // ── Step 6: Reset the file-creation mask ─────────────────────────────
    // umask(0) means files the daemon creates get the full permissions we ask.
    umask(0);

    // ── Step 7: Move to root ─────────────────────────────────────────────
    // This prevents the daemon from keeping any mounted filesystem busy.
    chdir("/");

    // ── Step 8: Close all file descriptors, redirect to /dev/null ────────
    // A daemon has no terminal, so we don't want random inherited fds open.
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
        close(x);

    // After closing everything, open /dev/null so that fd 0, 1, 2
    // (stdin, stdout, stderr) are not accidentally reused by the log file.
    int fd0 = open("/dev/null", O_RDWR);   // fd 0 = stdin
    int fd1 = dup(0);                       // fd 1 = stdout
    int fd2 = dup(0);                       // fd 2 = stderr
    (void)fd0; (void)fd1; (void)fd2;

    // ── Step 9: Do the actual work ────────────────────────────────────────
    daemon_work(project_dir);

    return 0;
}
