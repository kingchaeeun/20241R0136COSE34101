#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define MAX_PROCESSES 10
#define TIME_QUANTUM 4

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
    int io_burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    bool completed;
} Process;

// Function prototypes
void generate_processes(Process processes[], int num_processes);
void print_processes(Process processes[], int num_processes);
void fcfs_scheduling(Process processes[], int num_processes);
void non_preemptive_sjf(Process processes[], int num_processes);
void preemptive_sjf(Process processes[], int num_processes);
void non_preemptive_priority(Process processes[], int num_processes);
void preemptive_priority(Process processes[], int num_processes);
void round_robin(Process processes[], int num_processes, int time_quantum);
void reset_processes(Process processes[], int num_processes);
void print_gantt_chart(Process processes[], int num_processes, int completion_times[]);

void generate_processes(Process processes[], int num_processes) {
    srand(time(NULL));
    for (int i = 0; i < num_processes; i++) {
        processes[i].pid = i + 1;
        processes[i].arrival_time = rand() % 10;
        processes[i].burst_time = rand() % 10 + 1;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].priority = rand() % 10 + 1;
        processes[i].io_burst_time = rand() % 5 + 1;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].completed = false;
    }
}

void print_processes(Process processes[], int num_processes) {
    printf("PID\tArrival\tBurst\tPriority\tI/O Burst\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].priority, processes[i].io_burst_time);
    }
}

void fcfs_scheduling(Process processes[], int num_processes) {
    printf("\nFCFS Scheduling:\n");
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completion_times[MAX_PROCESSES];

    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
        processes[i].completion_time = current_time + processes[i].burst_time;

        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;

        current_time += processes[i].burst_time;
        completion_times[i] = current_time;

        printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void non_preemptive_sjf(Process processes[], int num_processes) {
    printf("\nNon-Preemptive SJF Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completion_times[MAX_PROCESSES];

    while (completed != num_processes) {
        int idx = -1;
        int min_burst = INT_MAX;

        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= current_time && processes[i].burst_time < min_burst) {
                min_burst = processes[i].burst_time;
                idx = i;
            }
        }

        if (idx != -1) {
            processes[idx].waiting_time = current_time - processes[idx].arrival_time;
            processes[idx].turnaround_time = processes[idx].waiting_time + processes[idx].burst_time;
            processes[idx].completion_time = current_time + processes[idx].burst_time;

            total_waiting_time += processes[idx].waiting_time;
            total_turnaround_time += processes[idx].turnaround_time;

            current_time += processes[idx].burst_time;
            processes[idx].completed = true;
            completed++;
            completion_times[completed - 1] = current_time;

            printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void preemptive_sjf(Process processes[], int num_processes) {
    printf("\nPreemptive SJF Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int prev = -1;
    int completion_times[MAX_PROCESSES];

    while (completed != num_processes) {
        int idx = -1;
        int min_remaining = INT_MAX;

        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= current_time && processes[i].remaining_time < min_remaining) {
                min_remaining = processes[i].remaining_time;
                idx = i;
            }
        }

        if (idx != -1) {
            if (prev != -1 && prev != idx) {
                printf("Process %d executed for %d time units.\n", processes[prev].pid, current_time - processes[prev].arrival_time);
            }

            processes[idx].remaining_time--;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                processes[idx].completed = true;
                completed++;
                processes[idx].waiting_time = current_time - processes[idx].arrival_time - processes[idx].burst_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                processes[idx].completion_time = current_time;

                total_waiting_time += processes[idx].waiting_time;
                total_turnaround_time += processes[idx].turnaround_time;
                completion_times[completed - 1] = current_time;

                printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
            }
            prev = idx;
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void non_preemptive_priority(Process processes[], int num_processes) {
    printf("\nNon-Preemptive Priority Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completion_times[MAX_PROCESSES];

    while (completed != num_processes) {
        int idx = -1;
        int max_priority = INT_MIN;

        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= current_time && processes[i].priority > max_priority) {
                max_priority = processes[i].priority;
                idx = i;
            }
        }

        if (idx != -1) {
            processes[idx].waiting_time = current_time - processes[idx].arrival_time;
            processes[idx].turnaround_time = processes[idx].waiting_time + processes[idx].burst_time;
            processes[idx].completion_time = current_time + processes[idx].burst_time;

            total_waiting_time += processes[idx].waiting_time;
            total_turnaround_time += processes[idx].turnaround_time;

            current_time += processes[idx].burst_time;
            processes[idx].completed = true;
            completed++;
            completion_times[completed - 1] = current_time;

            printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void preemptive_priority(Process processes[], int num_processes) {
    printf("\nPreemptive Priority Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int prev = -1;
    int completion_times[MAX_PROCESSES];

    while (completed != num_processes) {
        int idx = -1;
        int max_priority = INT_MIN;

        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= current_time && processes[i].priority > max_priority) {
                max_priority = processes[i].priority;
                idx = i;
            }
        }

        if (idx != -1) {
            if (prev != -1 && prev != idx) {
                printf("Process %d executed for %d time units.\n", processes[prev].pid, current_time - processes[prev].arrival_time);
            }

            processes[idx].remaining_time--;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                processes[idx].completed = true;
                completed++;
                processes[idx].waiting_time = current_time - processes[idx].arrival_time - processes[idx].burst_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                processes[idx].completion_time = current_time;

                total_waiting_time += processes[idx].waiting_time;
                total_turnaround_time += processes[idx].turnaround_time;
                completion_times[completed - 1] = current_time;

                printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
            }
            prev = idx;
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void round_robin(Process processes[], int num_processes, int time_quantum) {
    printf("\nRound Robin Scheduling (Time Quantum: %d):\n", time_quantum);
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completed = 0;
    int completion_times[MAX_PROCESSES];
    int time_slice[MAX_PROCESSES] = { 0 };

    while (completed != num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time > time_quantum) {
                    current_time += time_quantum;
                    processes[i].remaining_time -= time_quantum;
                }
                else {
                    current_time += processes[i].remaining_time;
                    processes[i].waiting_time = current_time - processes[i].arrival_time - processes[i].burst_time;
                    processes[i].turnaround_time = current_time - processes[i].arrival_time;
                    processes[i].remaining_time = 0;
                    processes[i].completion_time = current_time;
                    completed++;

                    total_waiting_time += processes[i].waiting_time;
                    total_turnaround_time += processes[i].turnaround_time;

                    completion_times[completed - 1] = current_time;

                    printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
                }
            }
        }
    }

    print_gantt_chart(processes, num_processes, completion_times);

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

void reset_processes(Process processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completed = false;
    }
}

void print_gantt_chart(Process processes[], int num_processes, int completion_times[]) {
    printf("\nGantt Chart:\n");
    printf(" |");

    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < processes[i].burst_time; j++) {
            printf("--");
        }
        printf("P%d|", processes[i].pid);
    }

    printf("\n0");

    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < processes[i].burst_time; j++) {
            printf("  ");
        }
        if (completion_times[i] > 9) {
            printf("\b");
        }
        printf("%d", completion_times[i]);
    }
    printf("\n");
}

int main() {
    int num_processes;
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    if (num_processes > MAX_PROCESSES) {
        printf("Number of processes should not exceed %d.\n", MAX_PROCESSES);
        return 1;
    }

    Process processes[MAX_PROCESSES];
    generate_processes(processes, num_processes);
    print_processes(processes, num_processes);

    // FCFS Scheduling
    fcfs_scheduling(processes, num_processes);

    // Reset processes
    reset_processes(processes, num_processes);

    // Non-Preemptive SJF Scheduling
    non_preemptive_sjf(processes, num_processes);

    // Reset processes
    reset_processes(processes, num_processes);

    // Preemptive SJF Scheduling
    preemptive_sjf(processes, num_processes);

    // Reset processes
    reset_processes(processes, num_processes);

    // Non-Preemptive Priority Scheduling
    non_preemptive_priority(processes, num_processes);

    // Reset processes
    reset_processes(processes, num_processes);

    // Preemptive Priority Scheduling
    preemptive_priority(processes, num_processes);

    // Reset processes
    reset_processes(processes, num_processes);

    // Round Robin Scheduling
    round_robin(processes, num_processes, TIME_QUANTUM);

    return 0;
}
