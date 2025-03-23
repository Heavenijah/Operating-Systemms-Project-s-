#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
} Process;

void read_processes(Process processes[], int *n) {
    FILE *file = fopen("processes.txt", "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    char line[100];
    *n = 0;
    fgets(line, sizeof(line), file); // Skip header
    while (fscanf(file, "%d %d %d %d", &processes[*n].pid, &processes[*n].arrival_time, 
                  &processes[*n].burst_time, &processes[*n].priority) == 4) {
        (*n)++;
    }
    fclose(file);
}

void display_gantt_chart(int process_order[], int start_times[], int num) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < num; i++) {
        printf("| P%d ", process_order[i]);
    }
    printf("|\n");

    for (int i = 0; i < num; i++) {
        printf("%d    ", start_times[i]);
    }
    printf("%d\n", start_times[num - 1]);
}

void fcfs_scheduling(Process processes[], int n) {
    int waiting_time[MAX_PROCESSES] = {0}, turnaround_time[MAX_PROCESSES] = {0};
    int completion_time[MAX_PROCESSES], process_order[MAX_PROCESSES], start_times[MAX_PROCESSES];
    int total_wt = 0, total_tat = 0;

    // Step 1: Sort processes by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[j].arrival_time < processes[i].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    // Step 2: Calculate completion, waiting, and turnaround times
    int time = processes[0].arrival_time;
    for (int i = 0; i < n; i++) {
        if (time < processes[i].arrival_time) {
            time = processes[i].arrival_time; // Idle time
        }

        start_times[i] = time;
        process_order[i] = processes[i].pid;

        completion_time[i] = time + processes[i].burst_time;
        waiting_time[i] = time - processes[i].arrival_time;
        turnaround_time[i] = waiting_time[i] + processes[i].burst_time;

        total_wt += waiting_time[i];
        total_tat += turnaround_time[i];

        time += processes[i].burst_time;
    }

    // Step 3: Display Gantt Chart
    display_gantt_chart(process_order, start_times, n);

    // Step 4: Display results
    printf("\nProcess\tArrival\tBurst\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, 
               processes[i].burst_time, waiting_time[i], turnaround_time[i]);
    }
    printf("\nAverage WT: %.2f\n", (float)total_wt / n);
    printf("Average TAT: %.2f\n", (float)total_tat / n);
}

void sjf_scheduling(Process processes[], int n) {
    int waiting_time[MAX_PROCESSES] = {0}, turnaround_time[MAX_PROCESSES] = {0};
    int completed[MAX_PROCESSES] = {0}, process_order[MAX_PROCESSES], start_times[MAX_PROCESSES];
    int total_wt = 0, total_tat = 0, current_time = 0, completed_count = 0;

    while (completed_count < n) {
        int shortest_job = -1;
        for (int i = 0; i < n; i++) {
            if (!completed[i] && processes[i].arrival_time <= current_time) {
                if (shortest_job == -1 || processes[i].burst_time < processes[shortest_job].burst_time) {
                    shortest_job = i;
                }
            }
        }

        if (shortest_job == -1) {
            current_time++; // Idle time
            continue;
        }

        int idx = completed_count;
        process_order[idx] = processes[shortest_job].pid;
        start_times[idx] = current_time;

        waiting_time[shortest_job] = current_time - processes[shortest_job].arrival_time;
        turnaround_time[shortest_job] = waiting_time[shortest_job] + processes[shortest_job].burst_time;

        total_wt += waiting_time[shortest_job];
        total_tat += turnaround_time[shortest_job];

        current_time += processes[shortest_job].burst_time;
        completed[shortest_job] = 1;
        completed_count++;
    }

    // Display Gantt Chart
    display_gantt_chart(process_order, start_times, n);

    // Display results
    printf("\nProcess\tArrival\tBurst\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, 
               processes[i].burst_time, waiting_time[i], turnaround_time[i]);
    }
    printf("\nAverage WT: %.2f\n", (float)total_wt / n);
    printf("Average TAT: %.2f\n", (float)total_tat / n);
}

int main() {
    Process processes[MAX_PROCESSES];
    int n, choice;

    read_processes(processes, &n);

    printf("Select Scheduling Algorithm:\n");
    printf("1. First-Come, First-Served (FCFS)\n");
    printf("2. Shortest Job First (SJF)\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("\nExecuting FCFS Scheduling...\n");
            fcfs_scheduling(processes, n);
            break;
        case 2:
            printf("\nExecuting SJF Scheduling...\n");
            sjf_scheduling(processes, n);
            break;
        default:
            printf("Invalid choice! Exiting...\n");
    }

    return 0;
}
