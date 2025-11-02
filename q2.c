
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


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
    const char *reg  = "2205018";

    create_files(name, reg);

    pid_t pids[3];

    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
          
            char fname[32];
            snprintf(fname, sizeof(fname), "file%d.txt", i+1);
       
            execlp("cat", "cat", fname, (char *)NULL);
            
            perror("execlp");
            exit(1);
        } else {
            
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
