#include "system_program.h"

int main()
{
    // Read the target path from the environment
    char *backup_target = getenv("BACKUP_DIR");
    if (backup_target == NULL)
    {
        fprintf(stderr, "Error: BACKUP_DIR is not set.\n");
        fprintf(stderr, "Usage: export BACKUP_DIR=/path/to/directory_or_file\n");
        return 1;
    }

    // Build the archive/ folder path relative to where the shell is running.
    // We do this before anything else so we always know the project directory.
    char project_dir[PATH_MAX];
    if (getcwd(project_dir, sizeof(project_dir)) == NULL)
    {
        perror("getcwd");
        return 1;
    }

    // PATH_MAX + 16 to safely fit "/archive" appended to a max-length project path
    char archive_dir[PATH_MAX + 16];
    snprintf(archive_dir, sizeof(archive_dir), "%s/archive", project_dir);

    // Create archive/ if it doesn't already exist (ignore error if it does)
    mkdir(archive_dir, 0755);

    // Get the current date and time to embed in the archive filename
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char datetime[64];
    strftime(datetime, sizeof(datetime), "%Y%m%d_%H%M%S", t);

    // Use just the base name of the target so the archive file has a readable name.
    // e.g. "/home/user/mydir" → base becomes "mydir"
    const char *base = strrchr(backup_target, '/');
    base = (base != NULL) ? base + 1 : backup_target;

    // Build the full tar command:
    //   tar -czf  → compress with gzip into a new file
    //   archive_dir/base_datetime.tar.gz  → destination
    //   backup_target  → what to compress
    char tar_cmd[PATH_MAX * 3];
    snprintf(tar_cmd, sizeof(tar_cmd),
             "tar -czf \"%s/%s_%s.tar.gz\" \"%s\"",
             archive_dir, base, datetime, backup_target);

    printf("Backing up '%s'...\n", backup_target);

    int result = system(tar_cmd);
    if (result == 0)
    {
        printf("Backup saved to: %s/%s_%s.tar.gz\n", archive_dir, base, datetime);
    }
    else
    {
        fprintf(stderr, "Backup failed. Make sure BACKUP_DIR points to a valid path.\n");
        return 1;
    }

    return 0;
}
