#include "processsimulator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <climits>
#include <iomanip>
#include "processsimulator.h"


using namespace std;

// Function to split a string by delimiter
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
// Function to trim leading and trailing whitespaces
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == string::npos) return "";
    else return s.substr(start, end - start + 1);
}
// Function to read processes from file
vector<Process> readProcesses(const string& filename) {
    vector<Process> processes;
    ifstream file(filename);
    string line;
    // Check if file is open
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return processes;
    }
// Skip first two lines
    getline(file, line);
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> parts = split(line, '|');
        if (parts.size() < 5) continue;
        // Parse process details
        try {
            Process p;
            p.pid = stoi(trim(parts[1]));
            p.arrivalTime = stoi(trim(parts[2]));
            p.burstTime = stoi(trim(parts[3]));
            p.priority = stoi(trim(parts[4]));
            processes.push_back(p);
        } catch (const exception& e) {
            cerr << "Error parsing line: " << line << endl;
        }
    }
    return processes;
}
// function to display Gantt chart
void displayGantt(const vector<GanttEntry>& gantt) {
    if (gantt.empty()) {
        cout << "Gantt chart is empty.\n";
        return;
    }
    // display Gantt chart
    cout << "Gantt Chart:\n";
    for (const auto& entry : gantt) {
        cout << "P" << entry.pid << " | ";
    }
    cout << "\n";
    // display timeline
    vector<int> timeline;
    timeline.push_back(gantt[0].start);
    for (const auto& entry : gantt) {
        timeline.push_back(entry.end);
    }
    
    for (const auto& time : timeline) {
        cout << time << " ";
    }
    cout << "\n" << endl;
} 
// function to perform First Come First Serve (FCFS) scheduling
SchedulingResult fcfs(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    
    int currentTime = 0;
    vector<GanttEntry> gantt;
    float totalWT = 0, totalTAT = 0;
    // iterate through processes
    for (auto& p : processes) {
        int start = max(currentTime, p.arrivalTime);
        int end = start + p.burstTime;
        gantt.push_back({p.pid, start, end});
        p.waitingTime = start - p.arrivalTime;
        p.turnaroundTime = end - p.arrivalTime;
        totalWT += p.waitingTime;
        totalTAT += p.turnaroundTime;
        currentTime = end;
    }
    // return scheduling result
    SchedulingResult res;
    res.gantt = gantt;
    res.processes = processes;
    res.avgWT = totalWT / processes.size();
    res.avgTAT = totalTAT / processes.size();
    return res;
}
// function to perform Shortest Job First (SJF) scheduling
SchedulingResult sjf(vector<Process> processes) {
    int n = processes.size();
    vector<bool> completed(n, false);
    int currentTime = 0;
    vector<GanttEntry> gantt;
    float totalWT = 0, totalTAT = 0;
    // iterate through processes until all are completed 
    while (true) {
        int idx = -1;
        int shortestBurst = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (!completed[i] && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < shortestBurst) {
                    shortestBurst = processes[i].burstTime;
                    idx = i;
                } else if (processes[i].burstTime == shortestBurst) {
                    if (processes[i].arrivalTime < processes[idx].arrivalTime) {
                        idx = i;
                    } else if (processes[i].arrivalTime == processes[idx].arrivalTime && processes[i].pid < processes[idx].pid) {
                        idx = i;
                    }
                }
            }
        }
        
        if (idx == -1) {
            int minArrival = INT_MAX;
            for (int i = 0; i < n; ++i) {
                if (!completed[i]) {
                    minArrival = min(minArrival, processes[i].arrivalTime);
                }
            }
            if (minArrival == INT_MAX) break;
            currentTime = minArrival;
            continue;
        }
        // update process details
        Process& p = processes[idx];
        int start = currentTime;
        int end = start + p.burstTime;
        gantt.push_back({p.pid, start, end});
        p.waitingTime = start - p.arrivalTime;
        p.turnaroundTime = end - p.arrivalTime;
        totalWT += p.waitingTime;
        totalTAT += p.turnaroundTime;
        completed[idx] = true;
        currentTime = end;
    }
    // return scheduling result
    SchedulingResult res;
    res.gantt = gantt;
    res.processes = processes;
    res.avgWT = totalWT / n;
    res.avgTAT = totalTAT / n;
    return res;
}
// function to perform Round Robin scheduling
SchedulingResult roundRobin(vector<Process> processes, int quantum) {
    int n = processes.size();
    vector<int> remainingTime(n);
    for (int i = 0; i < n; ++i) {
        remainingTime[i] = processes[i].burstTime;
    }
    // perform Round Robin scheduling
    queue<int> q;
    vector<bool> inQueue(n, false);
    int currentTime = 0;
    int completed = 0;
    vector<GanttEntry> gantt;
    float totalWT = 0, totalTAT = 0;
    
    vector<int> indices(n);
    for (int i = 0; i < n; ++i) indices[i] = i;// sort processes by arrival time
    sort(indices.begin(), indices.end(), [&](int a, int b) {
        return processes[a].arrivalTime < processes[b].arrivalTime;
    }); // iterate through processes
    
    int nextIdx = 0;
    while (completed < n) {
        while (nextIdx < n && processes[indices[nextIdx]].arrivalTime <= currentTime) {
            int i = indices[nextIdx];
            if (!inQueue[i]) {
                q.push(i);
                inQueue[i] = true;
            }
            nextIdx++;
        }
        // checks if queue is empty
        if (q.empty()) {
            if (nextIdx >= n) break;
            currentTime = processes[indices[nextIdx]].arrivalTime;
            continue;
        }
        
        int idx = q.front();
        q.pop();
        inQueue[idx] = false;
        // executes process
        int execTime = min(quantum, remainingTime[idx]);
        gantt.push_back({processes[idx].pid, currentTime, currentTime + execTime});
        remainingTime[idx] -= execTime;
        currentTime += execTime;
        // checks if process is completed
        while (nextIdx < n && processes[indices[nextIdx]].arrivalTime <= currentTime) {
            int i = indices[nextIdx];
            if (!inQueue[i]) {
                q.push(i);
                inQueue[i] = true;
            }
            nextIdx++;
        }
        
        if (remainingTime[idx] > 0) {
            q.push(idx);
            inQueue[idx] = true;
        } else {
            completed++; // update process details
            processes[idx].completionTime = currentTime;
            processes[idx].turnaroundTime = currentTime - processes[idx].arrivalTime; // calculates waiting time
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime; // updates total waiting time and turnaround time
            totalWT += processes[idx].waitingTime; // updates total waiting time and turnaround time
            totalTAT += processes[idx].turnaroundTime; // updates total waiting time and turnaround time
        }
    }
    // returns scheduling result
    SchedulingResult res;
    res.gantt = gantt;
    res.processes = processes;
    res.avgWT = totalWT / n;
    res.avgTAT = totalTAT / n;
    return res;
}

// Main function
int main() {
    string filename = "processes.txt";
    vector<Process> processes = readProcesses(filename);
    // check if processes are read - if the file is there or not
    if (processes.empty()) {
        cerr << "No processes read. Check the input file." << endl;
        return 1;
    }
    // display scheduling algorithm options
    cout << "Select a scheduling algorithm:\n\n";
    cout << "1. FCFS\n2. SJF\n3. Round Robin\n";
    int choice;
    cin >> choice;
    // performs scheduling 
    SchedulingResult result;
    switch (choice) {
        case 1:
            result = fcfs(processes);
            break;
        case 2:
            result = sjf(processes);
            break;
        case 3: {
            int quantum;
            cout << "Enter time quantum: ";
            cin >> quantum;
            result = roundRobin(processes, quantum);
            break;
        }
        default:
            cout << "Invalid choice." << endl;
            return 1;
    }
    
    displayGantt(result.gantt);
    
    sort(result.processes.begin(), result.processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });
    
    cout << "Process\tWaiting Time\tTurnaround Time\n";
    for (const auto& p : result.processes) {
        cout << "P" << p.pid << "\t" << p.waitingTime << "\t\t" << p.turnaroundTime << endl;
    }
    
    cout << fixed << setprecision(2);
    cout << "\nAverage Waiting Time: " << result.avgWT << endl;
    cout << "Average Turnaround Time: " << result.avgTAT << endl;
    
    return 0;
}
