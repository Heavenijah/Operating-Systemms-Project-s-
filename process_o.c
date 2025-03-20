#include <stdio.h>

typedef struct {
    int pid, arrival, burst, priority;
} Process;

void read_processes(const char *filename, Process processes[], int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    *count = 0;
    while (fscanf(file, "%d %d %d %d", &processes[*count].pid, &processes[*count].arrival,
                  &processes[*count].burst, &processes[*count].priority) != EOF) {
        (*count)++;
    }
    fclose(file);
}
