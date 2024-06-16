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
    int waiting_time;
    int turnaround_time;
    int completion_time;
    bool completed;
} Process;

int compare_arrival_time(const void* a, const void* b);
void enqueue(int queue[], int* rear, int value);
int dequeue(int queue[], int* front);

// Function prototypes
void generate_processes(Process processes[], int num_processes);
void print_processes(Process processes[], int num_processes);
void calculate_average_times(Process processes[], int num_processes, float* avg_waiting_time, float* avg_turnaround_time);
void export_averages_to_csv(const char* filename, const char* algorithm_name, float avg_waiting_time, float avg_turnaround_time);
void fcfs_scheduling(Process processes[], int num_processes);
void non_preemptive_sjf(Process processes[], int num_processes);
void preemptive_sjf(Process processes[], int num_processes);
void non_preemptive_priority(Process processes[], int num_processes);
void preemptive_priority(Process processes[], int num_processes);
void round_robin(Process processes[], int num_processes, int time_quantum);
void reset_processes(Process processes[], int num_processes);
void print_gantt_chart(Process processes[], int num_processes, int timeline[], int timeline_size, int time_stamps[]);

// 도착 시간을 기준으로 정렬하기 위한 비교 함수
int compare_arrival_time(const void* a, const void* b) {
    const Process* pa = (const Process*)a;
    const Process* pb = (const Process*)b;

    if (pa->arrival_time < pb->arrival_time) return -1;
    if (pa->arrival_time > pb->arrival_time) return 1;
    return 0;
}

void generate_processes(Process processes[], int num_processes) {
    srand(time(NULL));
    for (int i = 0; i < num_processes; i++) {
        processes[i].pid = i + 1;
        processes[i].arrival_time = rand() % 10;
        processes[i].burst_time = rand() % 10 + 1;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].priority = rand() % 10 + 1;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].completed = false;
    }
}

void print_processes(Process processes[], int num_processes) {
    printf("PID\tArrival\tBurst\tPriority\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].priority);
    }
}

void calculate_average_times(Process processes[], int num_processes, float* avg_waiting_time, float* avg_turnaround_time) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    for (int i = 0; i < num_processes; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    *avg_waiting_time = (float)total_waiting_time / num_processes;
    *avg_turnaround_time = (float)total_turnaround_time / num_processes;

    printf("Average Waiting Time: %.2f\n", *avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", *avg_turnaround_time);
}

void export_averages_to_csv(const char* filename, const char* algorithm_name, float avg_waiting_time, float avg_turnaround_time) {
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    fprintf(fp, "%s,%.2f,%.2f\n", algorithm_name, avg_waiting_time, avg_turnaround_time);
    fclose(fp);
}

void fcfs_scheduling(Process processes[], int num_processes) {
    printf("\nFCFS Scheduling:\n");
    int current_time = 0;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;

    qsort(processes, num_processes, sizeof(Process), compare_arrival_time);

    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].waiting_time = current_time - processes[i].arrival_time;
        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        current_time = processes[i].completion_time;

        for (int j = 0; j < processes[i].burst_time; j++) {
            timeline[timeline_size] = processes[i].pid;
            time_stamps[timeline_size++] = current_time - processes[i].burst_time + j;
        }

        printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n",
            processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "FCFS", avg_waiting_time, avg_turnaround_time);
}

void non_preemptive_sjf(Process processes[], int num_processes) {
    printf("\nNon-Preemptive SJF Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;

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
            current_time += processes[idx].burst_time;
            processes[idx].completed = true;
            completed++;

            for (int j = 0; j < processes[idx].burst_time; j++) {
                timeline[timeline_size] = processes[idx].pid;
                time_stamps[timeline_size++] = current_time - processes[idx].burst_time + j;
            }

            printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "Non-Preemptive SJF", avg_waiting_time, avg_turnaround_time);
}

void preemptive_sjf(Process processes[], int num_processes) {
    printf("\nPreemptive SJF Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;

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
            processes[idx].remaining_time--;
            timeline[timeline_size] = processes[idx].pid;
            time_stamps[timeline_size++] = current_time;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                processes[idx].completed = true;
                completed++;
                processes[idx].waiting_time = current_time - processes[idx].arrival_time - processes[idx].burst_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                processes[idx].completion_time = current_time;

                printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
            }
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "Preemptive SJF", avg_waiting_time, avg_turnaround_time);
}

void non_preemptive_priority(Process processes[], int num_processes) {
    printf("\nNon-Preemptive Priority Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;

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
            current_time += processes[idx].burst_time;
            processes[idx].completed = true;
            completed++;

            for (int j = 0; j < processes[idx].burst_time; j++) {
                timeline[timeline_size] = processes[idx].pid;
                time_stamps[timeline_size++] = current_time - processes[idx].burst_time + j;
            }

            printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "Non-Preemptive Priority", avg_waiting_time, avg_turnaround_time);
}

void preemptive_priority(Process processes[], int num_processes) {
    printf("\nPreemptive Priority Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;

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
            processes[idx].remaining_time--;
            timeline[timeline_size] = processes[idx].pid;
            time_stamps[timeline_size++] = current_time;
            current_time++;

            if (processes[idx].remaining_time == 0) {
                processes[idx].completed = true;
                completed++;
                processes[idx].waiting_time = current_time - processes[idx].arrival_time - processes[idx].burst_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                processes[idx].completion_time = current_time;

                printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
            }
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "Preemptive Priority", avg_waiting_time, avg_turnaround_time);
}

void round_robin(Process processes[], int num_processes, int time_quantum) {
    printf("\nRound Robin Scheduling:\n");
    int current_time = 0;
    int completed = 0;
    int queue[100];
    int front = 0;
    int rear = -1;
    int timeline[100];
    int time_stamps[100];
    int timeline_size = 0;
    int context_switches = 0;

    bool in_queue[MAX_PROCESSES] = { false };

    while (completed != num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time && !in_queue[i]) {
                enqueue(queue, &rear, i);
                in_queue[i] = true;
            }
        }

        if (front <= rear) {
            int idx = dequeue(queue, &front);
            int exec_time = (processes[idx].remaining_time < time_quantum) ? processes[idx].remaining_time : time_quantum;
            context_switches++;

            for (int j = 0; j < exec_time; j++) {
                timeline[timeline_size] = processes[idx].pid;
                time_stamps[timeline_size++] = current_time + j;
            }

            processes[idx].remaining_time -= exec_time;
            current_time += exec_time;

            if (processes[idx].remaining_time == 0) {
                processes[idx].completed = true;
                completed++;
                processes[idx].waiting_time = current_time - processes[idx].arrival_time - processes[idx].burst_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                processes[idx].completion_time = current_time;

                printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[idx].pid, processes[idx].waiting_time, processes[idx].turnaround_time);
            }
            else {
                for (int i = 0; i < num_processes; i++) {
                    if (processes[i].arrival_time <= current_time && !in_queue[i] && !processes[i].completed) {
                        enqueue(queue, &rear, i);
                        in_queue[i] = true;
                    }
                }
                enqueue(queue, &rear, idx);
            }
        }
        else {
            current_time++;
        }
    }

    print_gantt_chart(processes, num_processes, timeline, timeline_size, time_stamps);
    printf("Number of context switches: %d\n", context_switches);

    float avg_waiting_time, avg_turnaround_time;
    calculate_average_times(processes, num_processes, &avg_waiting_time, &avg_turnaround_time);
    export_averages_to_csv("scheduling_results.csv", "Round Robin", avg_waiting_time, avg_turnaround_time);
}

void reset_processes(Process processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].completed = false;
    }
}

void enqueue(int queue[], int* rear, int value) {
    queue[++(*rear)] = value;
}

int dequeue(int queue[], int* front) {
    return queue[(*front)++];
}

void print_gantt_chart(Process processes[], int num_processes, int timeline[], int timeline_size, int time_stamps[]) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < timeline_size; i++) {
        printf("P%d ", timeline[i]);
    }
    printf("\n");
    for (int i = 0; i < timeline_size; i++) {
        printf("%d ", time_stamps[i]);
    }
    printf("%d\n", time_stamps[timeline_size - 1] + 1);
}

int main() {
    Process processes[MAX_PROCESSES];
    int num_processes;

    // CSV file header
    FILE* fp = fopen("scheduling_results.csv", "w");
    if (fp != NULL) {
        fprintf(fp, "Algorithm,Average Waiting Time,Average Turnaround Time\n");
        fclose(fp);
    }

    scanf("%d", &num_processes);

    generate_processes(processes, num_processes);
    print_processes(processes, num_processes);

    // FCFS Scheduling
    fcfs_scheduling(processes, num_processes);
    reset_processes(processes, num_processes);

    // Non-Preemptive SJF Scheduling
    non_preemptive_sjf(processes, num_processes);
    reset_processes(processes, num_processes);

    // Preemptive SJF Scheduling
    preemptive_sjf(processes, num_processes);
    reset_processes(processes, num_processes);

    // Non-Preemptive Priority Scheduling
    non_preemptive_priority(processes, num_processes);
    reset_processes(processes, num_processes);

    // Preemptive Priority Scheduling
    preemptive_priority(processes, num_processes);
    reset_processes(processes, num_processes);

    // Round Robin Scheduling
    round_robin(processes, num_processes, TIME_QUANTUM);

    return 0;
}
