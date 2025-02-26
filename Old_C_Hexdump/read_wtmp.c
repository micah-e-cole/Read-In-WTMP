#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <string.h>
#include <time.h>

//#define WTMP_FILE "/var/log/wtmp"

void print_time(time_t rawtime) {
    struct tm *timeinfo;
    char buffer[80];

    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    printf("%s", buffer);
}

int main() {
    FILE *file;
    struct utmp record;

    // Open wtmp file
    file = fopen(WTMP_FILE, "rb");
    if (!file) {
        perror("Error opening wtmp file");
        return 1;
    }

    // Read each record
    while (fread(&record, sizeof(struct utmp), 1, file)) {
        // Print record type
        switch (record.ut_type) {
            case USER_PROCESS:
                printf("Login: ");
                break;
            case DEAD_PROCESS:
                printf("Logout: ");
                break;
            case BOOT_TIME:
                printf("System Event (Boot or Shutdown): ");
                break;
            default:
                printf("Unknown event: ");
                break;
        }

        // Print timestamp in a readable format
        print_time(record.ut_tv.tv_sec);

        // Print other details such as username and terminal
        printf("\nUser: %s\n", record.ut_user);
        printf("Terminal: %s\n", record.ut_line);
        printf("Hostname/IP: %s\n", record.ut_host);

        printf("\n---\n");
    }

    fclose(file);
    return 0;
}
