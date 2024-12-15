import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# rules : {txt_length: number_of_rules}
rules = {3317:1370, 43579:12294, 66532:16112, 108526:24186, 154452:31269, 186998:43844, 237418:46427, 274500:53784, 290801:54170, 1224377:191364}
# occ: number of occurrences, n: text length, r: number of rules, m: pattern length
def theoretical_time(occ, n, r, m):
    return ((m + np.log(n))*m*np.log(r)*np.log(np.log(r)) + occ*np.log(n)*np.log(np.log(r)))/1000

# Load the data
file_path = "2m.results.txt"
data = pd.read_csv(file_path, delim_whitespace=True, header=None, names=["n", "pattern", "occ", "time"])



# Plot time vs. density
for n in sorted(data["n"].unique()):
    plt.figure(figsize=(10, 6))
    plt.xlabel("Ocurrencias")
    plt.ylabel("Tiempo (ms)")
    plt.title("Tiempo de búsqueda vs. Ocurrencias")
    subset = data[data["n"] == n]
    plt.scatter(subset["occ"], subset["time"], label=f"n={n}", alpha=0.6)
    subset = data[data["n"] == n]
    x = subset["occ"]
    y = subset["time"]
    p = np.polyfit(x, y, 1)
    plt.plot(x, np.polyval(p, x), label=f"n={n} (tendency)")
    plt.legend()
    plt.grid()
    plt.show()
    # wait for user input
    input("Press Enter to continue...")
    plt.clf()
