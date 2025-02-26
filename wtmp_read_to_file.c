#include <stdio.h>
#include <utmp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void print_record(struct utmp *ut) {
    // Format and output the timestamp in readable format
    char time_str[100];
    struct tm *tm_info;
    
    // Casting to time_t to resolve incompatible pointer type
    time_t timestamp = (time_t)ut->ut_tv.tv_sec;
    tm_info = localtime(&timestamp);

    strftime(time_str, sizeof(time_str), "%m/%d/%Y:%H:%M:%S", tm_info);

    // Open the output file
    FILE *output_file = fopen("output.txt", "a");  // Open file for appending
    if (output_file == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    // Write the information to the file
    fprintf(output_file, "Record Type: ");
    switch (ut->ut_type) {
        case USER_PROCESS:
            fprintf(output_file, "USER_PROCESS\n");
            break;
        case LOGIN_PROCESS:
            fprintf(output_file, "LOGIN_PROCESS\n");
            break;
        case DEAD_PROCESS:
            fprintf(output_file, "DEAD_PROCESS\n");
            break;
        case INIT_PROCESS:
            fprintf(output_file, "INIT_PROCESS\n");
            break;
        // Remove or modify this case if necessary
        // case 5:
        //     fprintf(output_file, "SHUTDOWN_TIME\n");
        //     break;
        default:
            fprintf(output_file, "UNKNOWN (%d)\n", ut->ut_type);
            break;
    }

    fprintf(output_file, "PID: %d\n", ut->ut_pid);
    fprintf(output_file, "Terminal: %s\n", ut->ut_line);
    fprintf(output_file, "Username: %s\n", ut->ut_user);
    fprintf(output_file, "Host: %s\n", ut->ut_host);
    fprintf(output_file, "Exit Status: %d\n", ut->ut_exit.e_termination);
    fprintf(output_file, "Session ID: %d\n", ut->ut_session);
    fprintf(output_file, "Timestamp: %s\n\n", time_str);  // Print the formatted timestamp

    // Close the file after writing the record
    fclose(output_file);
}

int main() {
    struct utmp ut;
    FILE *wtmp_file = fopen(_PATH_WTMP, "rb");  // Use _PATH_WTMP from utmp.h

    if (wtmp_file == NULL) {
        perror("Failed to open wtmp file");
        return 1;
    }

    while (fread(&ut, sizeof(struct utmp), 1, wtmp_file)) {
        print_record(&ut);
    }

    fclose(wtmp_file);
    printf("wtmp data has been written to wtmp_output.txt\n");

    return 0;
}
