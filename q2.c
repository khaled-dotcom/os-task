// q2.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/*
 Creates file1.txt, file2.txt, file3.txt with header lines,
 then forks 3 children; each child execs 'cat' to display its file.
*/

void create_files(const char *name, const char *reg) {
    FILE *f;
    for (int i = 1; i <= 3; ++i) {
        char fname[32];
        snprintf(fname, sizeof(fname), "file%d.txt", i);
        f = fopen(fname, "w");
        if (!f) { perror("fopen"); exit(1); }
        fprintf(f, "#File%d, %s, %s\n", i, name, reg);
        fclose(f);
    }
}

int main() {
    const char *name = "khaled ghalwash";
    const char *reg  = "2205018"; // غيّر هذا لرقم تسجيلك

    // 1) Create the 3 files (so the program is self-contained)
    create_files(name, reg);

    // 2) Spawn exactly 3 children: for i=1..3 fork once per child from parent
    pid_t pids[3];

    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Child: execute cat file{i+1}.txt
            char fname[32];
            snprintf(fname, sizeof(fname), "file%d.txt", i+1);
            // Using execlp so PATH is used to find 'cat'
            execlp("cat", "cat", fname, (char *)NULL);
            // If exec returns, it failed
            perror("execlp");
            exit(1);
        } else {
            // Parent: record child pid and continue to spawn next child
            pids[i] = pid;
        }
    }

    // Parent: wait for all 3 children to finish
    for (int i = 0; i < 3; ++i) {
        int status;
        pid_t w = waitpid(pids[i], &status, 0);
        if (w == -1) {
            perror("waitpid");
        } else {
            // Optional: show exit status
            if (WIFEXITED(status))
                ; // printf("Child %d exited with %d\n", (int)w, WEXITSTATUS(status));
        }
    }

    return 0;
}
