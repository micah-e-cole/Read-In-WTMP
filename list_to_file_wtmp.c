#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <time.h>
#include <string.h>

#define MAX_RECORDS 1000  // Adjust based on expected wtmp size

struct record_entry {
    char entry[256];  // Adjust the size based on expected format length
};

void format_record(struct utmp *ut, struct record_entry *rec) {
    struct tm *tm_info;
    char timestamp[20];  // Format: DD/MM/YYYY HH:MM:SS AM/PM

    tm_info = localtime((time_t *)&ut->ut_tv.tv_sec);
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %I:%M:%S %p", tm_info);

    snprintf(rec->entry, sizeof(rec->entry),
             "User: %-8s | Type: %-2d | PID: %-5d | Line: %-8s | Host: %-16s | Time: %s\n",
             ut->ut_user, ut->ut_type, ut->ut_pid, ut->ut_line, ut->ut_host, timestamp);
}

int main() {
    struct utmp ut;
    struct record_entry records[MAX_RECORDS];
    int record_count = 0;

    FILE *wtmp_file = fopen(WTMP_FILE, "rb");
    if (!wtmp_file) {
        perror("Error opening wtmp file");
        return 1;
    }

    // Read records and store them in the list
    while (fread(&ut, sizeof(struct utmp), 1, wtmp_file) == 1) {
        if (record_count < MAX_RECORDS) {
            format_record(&ut, &records[record_count]);
            record_count++;
        } else {
            fprintf(stderr, "Warning: Too many records, increase MAX_RECORDS.\n");
            break;
        }
    }

    fclose(wtmp_file);

    // Write all records at once
    FILE *output_file = fopen("wtmp_output.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 0; i < record_count; i++) {
        fprintf(output_file, "%s", records[i].entry);
    }

    fclose(output_file);
    printf("Successfully written %d records to wtmp_output.txt\n", record_count);

    return 0;
}
