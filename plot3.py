import matplotlib.pyplot as plt
import numpy as np

avgs = [5.25, 27.19, 145, 640.47, 997.1, 1413.4, 1598.5, 4999.8, 8661, 11395.1, 13956.9, 18032, 21610.9, 24332.0, 25668.6, 49912.6, 63181.1, 114075, 216726]
n_rules = [8, 43, 734, 3070, 4275, 5595, 5985, 16112, 24186, 30835, 35557, 47577, 50476, 57319, 57991, 103688, 123161, 194712, 337648]
lens = [17, 277, 1702, 8675, 13156, 19498, 23155, 66532, 108526, 131260, 174357, 206964, 257437, 294561, 310827, 560754, 724917, 1244214, 2225845]


# O(c + n log n)
def theoretical_time(n, c):
    return (c + n* np.log(n))/150

theoretical = [theoretical_time(n, c) for n, c in zip(lens, n_rules)]

plt.figure(figsize=(8, 5))

plt.plot(n_rules, avgs, color='r', linestyle='--', label='Run time')
plt.plot(n_rules, theoretical, color='b', linestyle='--', label='Theoretical Prediction/150')

plt.xlabel('Number of rules', fontsize=12)
plt.ylabel('Time', fontsize=12)
plt.title('Time of Construction', fontsize=14)
plt.legend()  # Show the legend
plt.grid(True)  # Add gridlines

# Display the plot
plt.show()
