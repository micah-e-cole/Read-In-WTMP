#include <stdio.h>
#include <utmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>  // For time-related functions

void print_ip_address(FILE *out, char *hostname) {
    struct hostent *host_info;
    struct in_addr *address;

    // Try to resolve the IP address from the hostname
    host_info = gethostbyname(hostname);
    if (host_info != NULL) {
        address = (struct in_addr *)host_info->h_addr_list[0];
        fprintf(out, "IP Address: %s\n", inet_ntoa(*address));
    } else {
        // If hostname resolution fails, print the hostname itself
        fprintf(out, "IP Address: Unable to resolve hostname\n");
    }
}

void print_timestamp(FILE *out, time_t timestamp) {
    struct tm *tm_info;
    char buffer[50];  // Buffer to hold formatted date/time string

    tm_info = localtime(&timestamp);  // Convert time_t to struct tm (local time)
    
    // Format timestamp with 12-hour format and AM/PM
    strftime(buffer, sizeof(buffer), "%Y:%m:%d:%HH:%MM:%SS", tm_info);
    fprintf(out, "Timestamp: %s\n", buffer);
}

void print_utmp_record(FILE *out, struct utmp record) {
    // Print record type
    fprintf(out, "Record Type: %d\n", record.ut_type);

    // Print PID (if available)
    if (record.ut_type == USER_PROCESS || record.ut_type == LOGIN_PROCESS || record.ut_type == DEAD_PROCESS) {
        fprintf(out, "PID: %d\n", record.ut_pid);
    }

    // Print Terminal Line
    fprintf(out, "Terminal Line: %-8s\n", record.ut_line);

    // Print Username
    fprintf(out, "Username: %-8s\n", record.ut_user);

    // Print Hostname or attempt to resolve IP address
    fprintf(out, "Hostname: %-15s\n", record.ut_host);
    print_ip_address(out, record.ut_host);  // Try to print the IP address of the host

    // Print Exit Status (if available)
    if (record.ut_type == DEAD_PROCESS) {
        fprintf(out, "Exit Status: %d\n", record.ut_exit.e_exit);
    }

    // Print Session ID
    fprintf(out, "Session ID: %ld\n", record.ut_session);

    // Print Timestamp in readable 12-hour format with AM/PM
    print_timestamp(out, record.ut_tv.tv_sec);

    // Print Additional Address Info (from ut_host)
    fprintf(out, "Additional Address Info: %-15s\n", record.ut_host);

    fprintf(out, "-------------------------------------------------\n");
}

int main() {
    FILE *fp, *out_fp;
    struct utmp record;
    char wtmp_path[1024];
    char *cwd;

    // Get the current working directory (PWD)
    cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("getcwd");
        return 1;
    }

    // Construct the full path to the wtmp file
    snprintf(wtmp_path, sizeof(wtmp_path), "%s/wtmp", cwd);

    // Open the wtmp file for reading
    fp = fopen(wtmp_path, "r");
    if (fp == NULL) {
        perror("Failed to open wtmp");
        free(cwd); // Don't forget to free the memory allocated by getcwd
        return 1;
    }

    // Open the output file for writing
    out_fp = fopen("wtmp_output.txt", "w");
    if (out_fp == NULL) {
        perror("Failed to open output file");
        fclose(fp);
        free(cwd); // Don't forget to free the memory allocated by getcwd
        return 1;
    }

    // Write a header to the output file
    fprintf(out_fp, "UTMP/WTMP Log Output\n");
    fprintf(out_fp, "====================\n");

    // Read each record in the wtmp file
    while (fread(&record, sizeof(record), 1, fp) == 1) {
        // Only process records of type USER_PROCESS, LOGIN_PROCESS, DEAD_PROCESS
        if (record.ut_type == USER_PROCESS || record.ut_type == LOGIN_PROCESS || record.ut_type == DEAD_PROCESS) {
            print_utmp_record(out_fp, record);
        }
    }

    fclose(fp);
    fclose(out_fp);
    free(cwd); // Free the memory allocated by getcwd
    return 0;
}
