'''
pattern_size time
2 1.64449
3 1.97411
5 1.94891
7 2.20389
9 2.48988
13 3.12181
16 3.41859
18 3.61536
21 2.5378
23 2.73096
26 3.09739
29 3.59129
32 4.06694
34 4.30514
37 4.60918
40 3.57036
43 3.92105
46 4.35437
49 4.87906
51 5.19415
53 5.38387
56 5.71825
59 4.80911
63 5.25789
65 5.58333
69 6.3228
74 6.91337
78 6.22465
81 6.59281
83 6.90998
86 7.41096
89 7.91123
92 8.27877
96 8.69826
'''
import matplotlib.pyplot as plt
#plot time vs. pattern_size
pattern_size = [2, 3, 5, 7, 9, 13, 16, 18, 21, 23, 26, 29, 32, 34, 37, 40, 43, 46, 49, 51, 53, 56, 59, 63, 65, 69, 74, 78, 81, 83, 86, 89, 92, 96]
time = [1.64449, 1.97411, 1.94891, 2.20389, 2.48988, 3.12181, 3.41859, 3.61536, 2.5378, 2.73096, 3.09739, 3.59129, 4.06694, 4.30514, 4.60918, 3.57036, 3.92105, 4.35437, 4.87906, 5.19415, 5.38387, 5.71825, 4.80911, 5.25789, 5.58333, 6.3228, 6.91337, 6.22465, 6.59281, 6.90998, 7.41096, 7.91123, 8.27877, 8.69826]
plt.figure(figsize=(10, 6))
plt.xlabel("Tamaño del patrón")
plt.ylabel("Tiempo (ms)")

plt.title("Tiempo de búsqueda vs. Tamaño del patrón, con cantidad de ocurrencias fija")
plt.scatter(pattern_size, time, alpha=0.9, label="Tamaño del patrón vs. Tiempo")

# set grid so that it shoes a linea line every 0.1 in y axis
plt.yticks([i*0.5 for i in range(0, 20, 1)])
plt.xticks([i for i in range(0, 100, 5)])
# polinomial fit
import numpy as np
x = pattern_size
y = time
p = np.polyfit(x, y, 1)
plt.plot(x, np.polyval(p, x), label="Tendencia", color="red", alpha=0.6)

plt.legend()
plt.grid()
plt.show()
