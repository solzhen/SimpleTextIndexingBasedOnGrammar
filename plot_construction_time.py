import matplotlib.pyplot as plt
import numpy as np

'''
txt_len number_of_rules time 

277 43 9.41075 // scream
3317 1370 225.611   // bill of rights
43579 12294 3504.07  // fall of house
66532 16112 5014.43   // raven
100000 22422 9558.05  // dna100000
108526 24186 8666.82  // poetics
111299 26149 9256.64  // lazarillo
154452 31269 12382  // alice's adventures
186998 43844 16628.7  // emily dickson poems
237418 46427 19993.2  // pinocchio
274500 53784 21610  // eureka
290801 54170 22928.7  // apology,crito,phaedo
540742 99526 48426.6  // cuentos chilenos
1224377 191364 123674 // the republic
'''

txt_lens = [277, 3317, 43579, 66532, 100000, 108526, 111299, 154452, 186998, 237418, 274500, 290801, 540742, 1224377]
n_rules = [43, 1370, 12294, 16112, 22422, 24186, 26149, 31269, 43844, 46427, 53784, 54170, 99526, 191364]
times = [9.41075, 225.611, 3504.07, 5014.43, 9558.05, 8666.82, 9256.64, 12382.0, 16628.7, 19993.2, 21610.0, 22928.7, 48426.6, 123674.0]
#times_memo = [3.87576, 1226.87, 1619.06, 2287.6, 2589.29, 2751.37, 3505.64, 4947.1, 5460.06, 5738.68, 5824.36]

def theoretical_time(n, r):    
    return (n + r * np.log(r) * np.log(n))/260

theoretical = [theoretical_time(n, r) for n, r in zip(txt_lens, n_rules)]
print (theoretical)

plt.figure(figsize=(8, 5))

plt.plot(n_rules, times, color='r', linestyle='--', label='Tiempo de construcción')

#plt.plot(n_rules, times_memo, color='g', linestyle='--', label='Tiempo de construcción con memoización')

plt.plot(n_rules, theoretical, color='b', linestyle='--', label='Predicción teórica con ajuste de 1/260')


plt.xlabel('Numero de reglas', fontsize=12)
plt.ylabel('Tiempo (ms)', fontsize=12)
plt.title('Tiempo de Construcción según número de reglas', fontsize=14)
plt.legend()  # Show the legend
plt.grid(True)  # Add gridlines

# Display the plot
plt.show()




#avgs = [5.25, 27.19, 145, 640.47, 997.1, 1413.4, 1598.5, 4999.8, 8661, 11395.1, 13956.9, 18032, 21610.9, 24332.0, 25668.6, 49912.6, 63181.1, 114075, 216726]
#n_rules = [8, 43, 734, 3070, 4275, 5595, 5985, 16112, 24186, 30835, 35557, 47577, 50476, 57319, 57991, 103688, 123161, 194712, 337648]
#lens = [17, 277, 1702, 8675, 13156, 19498, 23155, 66532, 108526, 131260, 174357, 206964, 257437, 294561, 310827, 560754, 724917, 1244214, 2225845]


