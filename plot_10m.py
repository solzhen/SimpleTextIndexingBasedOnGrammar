from matplotlib import pyplot as plt


filename = "einstein.en.txt"
with open(f"textfile/{filename}.big.10m.results.txt") as f:
    lines = f.readlines()[1:]
    ms = []
    occs = []
    times_per_occ = []
    for line in lines:
        n, r, m, occ, t = map(int, line.split())
        ms.append(m)
        occs.append(occ)
        times_per_occ.append(t / occ)
    plt.figure(figsize=(10, 6))
    plt.xscale("log")
    #plt.yscale("log")
    plt.minorticks_on()
    plt.xlabel("Ocurrencias")
    plt.ylabel(r"Tiempo por ocurrencia ($\mu$s)")
    plt.title(f"Tiempo por ocurrencias para patrones de largo 10")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.5)
    plt.scatter(occs, times_per_occ, alpha=0.6, label=f"{filename}\nn = 467626544\nr = 163417")
    plt.legend()
    plt.show()
    plt.clf()
