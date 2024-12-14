import matplotlib.pyplot as plt
import numpy as np

avgs = [0.967,2.068,3.24,4.47,5.8,7.198,8.7,10.29,12.015,13.889,15.89,17.994,20.17,22.41]
ms = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]

n = 510
r = 13
occ = 50

def theoretical_time(m):
    return ((m + np.log(n))*m*np.log(r)*np.log(np.log(r)) + occ*np.log(n)*np.log(np.log(r)))/23-12

theoretical = [theoretical_time(m) for m in ms]

plt.figure(figsize=(8, 5))

plt.plot(ms, avgs, color='r', linestyle='--', label='run time as a function of length of pattern')
plt.plot(ms, theoretical, color='b', linestyle='--', label='Theoretical Prediction/23-12')

plt.xlabel('Pattern length', fontsize=12)
plt.ylabel('Time', fontsize=12)
plt.title('Time as function of pattern length', fontsize=14)
plt.legend()  # Show the legend
plt.grid(True)  # Add gridlines

# Display the plot
plt.show()
