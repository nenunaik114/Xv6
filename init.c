#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#ifndef USERNAME
#define USERNAME "default_user"
#endif
#ifndef PASSWORD
#define PASSWORD "default_pass"
#endif



#define MAX_ATTEMPTS 3

void read_input(char *buf, int size) {
    int n = read(0, buf, size - 1);
    if (n > 0) {
        buf[n - 1] = '\0';  // Remove newline
    }
}

void login() {
    char username[32], password[32];
    int attempts = MAX_ATTEMPTS;

    while (attempts > 0) {
        printf(1, "Enter Username: ");
        read_input(username, sizeof(username));

        if (strcmp(username, USERNAME) != 0) {
            printf(1, "Invalid Username\n");
            attempts--;
            continue;
        }

        printf(1, "Enter Password: ");
        read_input(password, sizeof(password));

        if (strcmp(password, PASSWORD) == 0) {
            printf(1, "Login successful\n");
            memset(password, 0, sizeof(password));  // Clear password buffer
            return;
        } else {
            printf(1, "Invalid Password\n");
            attempts--;
        }
    }

    printf(1, "Login failed. System halted.\n");
    printf(1, "Reboot the system to try again.\n");
    for(;;) sleep(100);
}

char *argv[] = { "sh", 0 };

int main(void) {
    int pid, wpid;

    if (open("console", O_RDWR) < 0) {
        mknod("console", 1, 1);
        open("console", O_RDWR);
    }
    dup(0);
    dup(0);

    login();

    for (;;) {
        printf(1, "init: starting sh\n");
        pid = fork();
        if (pid < 0) {
            printf(1, "init: fork failed\n");
            exit();
        }
        if (pid == 0) {
            exec("sh", argv);
            printf(1, "init: exec sh failed\n");
            exit();
        }
        while ((wpid = wait()) >= 0 && wpid != pid)
            printf(1, "zombie!\n");
    }
}
