/ q1.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define STR_SIZE 100

int main() {
    int p1[2]; // parent -> child
    int p2[2]; // child  -> parent

    if (pipe(p1) == -1) { perror("pipe p1"); exit(1); }
    if (pipe(p2) == -1) { perror("pipe p2"); exit(1); }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        // Child
        close(p1[1]); // close write end of parent->child
        close(p2[0]); // close read end of child->parent

        char recv_str[STR_SIZE];
        int n;

        // Read fixed-size string buffer
        ssize_t r = read(p1[0], recv_str, STR_SIZE);
        if (r <= 0) { perror("child read str"); exit(1); }
        // Ensure null-terminated
        recv_str[STR_SIZE-1] = '\0';
        // Read integer
        if (read(p1[0], &n, sizeof(n)) != sizeof(n)) {
            perror("child read int");
            exit(1);
        }

        printf("Child read <\"%s\" and n = %d> from Parent\n", recv_str, n);

        // Build repeated string
        size_t len = strlen(recv_str);
        size_t total_len = len * (size_t)n;
        if (total_len + 1 > 10000) { // safety guard
            fprintf(stderr, "result too large\n");
            exit(1);
        }
        char *result = malloc(total_len + 1);
        if (!result) { perror("malloc"); exit(1); }
        result[0] = '\0';
        for (int i = 0; i < n; ++i) strcat(result, recv_str);

        // Send result to parent
        if (write(p2[1], result, strlen(result) + 1) == -1) {
            perror("child write to parent");
        }

        free(result);
        close(p1[0]);
        close(p2[1]);
        exit(0);
    } else {
        // Parent
        close(p1[0]); // close read end of parent->child
        close(p2[1]); // close write end of child->parent

        char send_str[STR_SIZE];
        strncpy(send_str, "hello", STR_SIZE-1);
        send_str[STR_SIZE-1] = '\0';
        int n = 3;

        // Write string (fixed buffer) then integer
        if (write(p1[1], send_str, STR_SIZE) == -1) { perror("parent write str"); }
        if (write(p1[1], &n, sizeof(n)) == -1) { perror("parent write int"); }

        // Close write end to signal EOF to child if needed
        close(p1[1]);

        // Read response
        char resp[10000];
        ssize_t rr = read(p2[0], resp, sizeof(resp));
        if (rr > 0) {
            printf("Parent read <\"%s\"> from Child\n", resp);
        } else {
            if (rr == 0) printf("Parent: no data from child\n");
            else perror("parent read");
        }

        close(p2[0]);
        wait(NULL); // avoid zombie
    }

    return 0;
}
