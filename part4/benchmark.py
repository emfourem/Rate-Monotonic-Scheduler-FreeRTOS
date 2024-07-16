import matplotlib.pyplot as plt
import os

def extract_first_idle_start_time(filename):
    with open(filename, 'r', encoding='latin-1') as file:
        for line in file:
            if "Idle starts" in line:
                return int(line.split(': ')[1].replace('.', ''))
    return None

def extract_metrics(filename):
    idle_start_times = []
    task_times = []
    context_switches = 0
    last_task_finish_time = 0
    previous_idle_start = None

    with open(filename, 'r', encoding='latin-1') as file:
        for line in file:
            if "Idle" in line:
                current_idle_start = int(line.split(': ')[1].replace('.', ''))
                #print(current_idle_start)
                if previous_idle_start != current_idle_start:
                    idle_start_times.append(current_idle_start)
                    previous_idle_start = current_idle_start
            elif "finished at time" in line:
                finish_time = int(line.split(' ')[-1].replace('.', ''))
                task_times.append(finish_time)
                if finish_time > last_task_finish_time:
                    last_task_finish_time = finish_time
            elif "is running" in line:
               context_switches += 1
    
    total_execution_time = 120 
    total_idle_time = len(idle_start_times)
    idle_percentage = (total_idle_time / total_execution_time) * 100 if total_execution_time > 0 else 0
    
    return total_execution_time, idle_percentage, task_times, context_switches

def plot_performance_comparison(rate_monotonic_metrics, standard_scheduling_metrics):
    categories = ['Total Execution Time', 'Idle Time Percentage', 'Context Switches']
    rate_monotonic_values = [rate_monotonic_metrics[0], rate_monotonic_metrics[1], rate_monotonic_metrics[3]]
    standard_scheduling_values = [standard_scheduling_metrics[0], standard_scheduling_metrics[1], standard_scheduling_metrics[3]]
    
    x = range(len(categories))
    
    fig, ax = plt.subplots()
    ax.bar(x, rate_monotonic_values, width=0.4, label='Rate Monotonic', align='center')
    ax.bar(x, standard_scheduling_values, width=0.4, label='Standard Scheduling', align='edge')
    
    ax.set_xlabel('Performance Metrics')
    ax.set_ylabel('Values')
    ax.set_title('Performance Comparison: Rate Monotonic vs Standard Scheduling')
    ax.set_xticks(x)
    ax.set_xticklabels(categories)
    ax.legend()
    plt.show()

rate_monotonic_file = 'outputs/output_RM.txt'
standard_scheduling_file = 'outputs/output_freertos.txt'


rate_monotonic_metrics = extract_metrics(rate_monotonic_file)
standard_scheduling_metrics = extract_metrics(standard_scheduling_file)

# Display results
print(f"Rate Monotonic Scheduling Metrics:\nTotal Execution Time: {rate_monotonic_metrics[0]}\nIdle Time Percentage: {rate_monotonic_metrics[1]:.2f}%\nContext Switches: {rate_monotonic_metrics[3]}")
print(f"Standard Scheduling Metrics:\nTotal Execution Time: {standard_scheduling_metrics[0]}\nIdle Time Percentage: {standard_scheduling_metrics[1]:.2f}%\nContext Switches: {standard_scheduling_metrics[3]}")

# Plotting the results
plot_performance_comparison(rate_monotonic_metrics, standard_scheduling_metrics)

