import os
import re

# Define the folder containing the output files
output_folder = './textfile/gsi-results/'

# Regex pattern to match the required lines in the output
line_pattern = re.compile(
    r'G-Index/(\d+)\s+[\d.]+ ns\s+[\d.]+ ns\s+\d+\s+Bits_x_Symbol=([\d.]+)\s+.*Time_x_Occurrence=([\d.]+)us'
)

# List to store results
results = []
results_2 = []
results_4 = []
results_8 = []
results_16 = []
results_32 = []

# Process each output file in the folder
for output_file in os.listdir(output_folder):
    if output_file.endswith('.output.txt'):
        # Extract the group number from the filename
        group_number = int(re.search(r'group_(\d+)', output_file).group(1))
        
        # Open and read the output file
        with open(os.path.join(output_folder, output_file), 'r') as file:
            for line in file:
                match = line_pattern.search(line)
                if match:
                    # Extract G-Index, Bits_x_Symbol, and Time_x_Occurrence
                    g_index = int(match.group(1))
                    bits_x_symbol = float(match.group(2))
                    time_x_occurrence = float(match.group(3))
                    
                    # Append the extracted data as a tuple
                    results.append((group_number, g_index, bits_x_symbol, time_x_occurrence))

# Print the extracted results
""" for result in results:
    print(result) """

occs = []

sorted_results = sorted(results, key=lambda x: x[0])
for result in sorted_results:
    print(result)
    if result[1] == 2:
        results_2.append(result[3])
        occs.append(result[0])
    elif result[1] == 4:
        results_4.append(result[3])
    elif result[1] == 8:
        results_8.append(result[3])
    elif result[1] == 16:
        results_16.append(result[3])
    elif result[1] == 32:
        results_32.append(result[3])

for result in results_2:
    print(result)

print(len(sorted_results))
print(len(results_2))
print(len(results_4))
print(len(results_8))
print(len(results_16))
print(len(results_32))
print(occs)

# n_of_occurrences, structure_type, bit_x_symbol, time_x_occurrence
# plot time_x_occurrence vs n_of_occurrences for each structure_type

import matplotlib.pyplot as plt

# Plot the results
plt.figure(figsize=(10, 6))
plt.plot(occs, results_2, label='2')

plt.xlabel('Number of occurrences')
plt.ylabel('Time x Occurrence (us)')
plt.legend(title='Bits x Symbol')
plt.title('Time x Occurrence vs Number of occurrences')
plt.grid(True)
plt.show()
