import matplotlib.pyplot as plt
import numpy as np
from itertools import groupby
from statistics import mean

m = 5
n = 109200
r = 26146

def theoretical_time(occ):
    return ((m + np.log(n))*m*np.log(r)*np.log(np.log(r)) + occ*np.log(n)*np.log(np.log(r)))/1760+3


filename = "results.txt"

p_size = 0

results = []

# Read the file and parse the contents
with open(filename, "r") as file:
    for line in file:
        occ, p_size, t = map(float, line.split())
        results.append((occ, t))  # Store only occs and time as tuples

# Sort the results by occs
results.sort(key=lambda x: x[0])

grouped_points = groupby(results, key=lambda point: point[0])

averaged_points = [
    (x, mean(y for _, y in group))
    for x, group in grouped_points
]


occs, time = zip(*averaged_points)

theoretical = [theoretical_time(occ) for occ in occs]

# Fit a polynomial curve (degree 2 for example)
degree = 1
coefficients = np.polyfit(occs, time, degree)
polynomial = np.poly1d(coefficients)

occs_sorted = np.linspace(min(occs), max(occs), 500)  # Smooth curve
fitted_time = polynomial(occs_sorted)


plt.figure(figsize=(8, 5))
plt.scatter(occs, time, color='b', label='Data Points', s=7)
#plt.plot(occs, time, marker='o', linestyle='-', color='b', label='Time vs Occurrences')
# Plot the fitted curve
plt.plot(occs_sorted, fitted_time, color='g', label=f'Polynomial Fit (Degree {degree})')
plt.plot(occs, theoretical, color='r', linestyle='--', label='Theoretical Prediction/1760+3')



plt.xlabel('Occurrences', fontsize=12)
plt.ylabel('Time', fontsize=12)
plt.title('Time as function of occurences for patterns of size 2', fontsize=14)
plt.legend()  # Show the legend
plt.grid(True)  # Add gridlines

# Display the plot
plt.show()
