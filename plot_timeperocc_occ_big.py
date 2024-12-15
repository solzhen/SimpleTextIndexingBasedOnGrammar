filenames = ["world_leaders","Escherichia_Coli","influenza","coreutils","kernel","para","cere","einstein.en.txt"]
# each file has a bunch of lines
# the first line has no worthwith information
# the rest of the lines have the following format:
# txt_length num_of_rules pattern_length num_of_occurrences time_taken
# lines are sorted by pattern_length
# plot for each file time_per_occurence vs num_of_occurences
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit


filenames = ["einstein.en.txt"]

for filename in filenames:
    plt.figure(figsize=(10, 6))
    plt.xscale("log")
    plt.yscale("log")
    plt.minorticks_on()
    plt.xlabel("Ocurrencias")
    plt.ylabel(r"Tiempo por ocurrencia ($\mu$s)") 
    plt.title(f"{filename}")      
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.5)
    ticks = [1, 2, 3, 4, 5, 10, 15, 20, 25, 30, 40, 50, 75, 100, 200, 300, 400, 500, 700, 1000, 2000, 3000, 5000]
    plt.yticks(ticks, [str(t) for t in ticks])  # Set ticks and labels
    with open(f"textfile/{filename}.big.results.txt") as f:
        lines = f.readlines()[1:]  # Ignore first line    
        occs = []
        times_per_occ = []
        pattern_lengths = []
        n = 0
        r = 0        
        # Parse the data
        for line in lines:
            n, r, m, occ, t = map(int, line.split())
            if occ == 0:  # This never happens
                continue
            # Get time per occurrence
            t_per_occ = t / occ
            occs.append(occ)
            times_per_occ.append(t_per_occ)
            pattern_lengths.append(m)        
        occs = np.array(occs)
        times_per_occ = np.array(times_per_occ)
        occs, times_per_occ = zip(*sorted(zip(occs, times_per_occ))) 
        plt.scatter(occs, times_per_occ, alpha=0.6, label=f"{filename}")
    plt.legend()
    plt.show()
    plt.clf()
plt.clf()
