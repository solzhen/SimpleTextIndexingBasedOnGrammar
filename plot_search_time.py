import sys
import matplotlib.pyplot as plt
import numpy as np

filenames = ['search_struct.txt', 'search_linear.txt']

def theoretical_time(occ, n, r, m):
    return ((m + np.log(n))*m*np.log(r)*np.log(np.log(r)) + occ*np.log(n)*np.log(np.log(r)))/3200

def plot_search_time(filename, textname):
    #read the file line by line
    with open(filename) as f:
        lines = f.readlines()

    #first line is text_length number_of_rules time_of_construction
    #split the line by space
    n, r, t = map(float, lines[0].split())

    times1 = [] #search_struct
    times2 = [] #search_linear
    occs = []
    ms = [5, 10, 20, 30, 40, 50, 70]#, 100] # pattern lengths

    #every 100 lines are occurrences_of_pattern size_of_pattern(m) time_of_search_struct time_of_search_linear
    #split each line by space
    for j in range(len(ms)):
        measures = 100
        sum1 = 0
        sum2 = 0
        sumocc = 0
        for i in range(measures):
            occ, _, t1, t2 = map(float, lines[j*100 + i+1].split())
            sum1 += t1
            sum2 += t2
            sumocc += occ
        times1.append(sum1/measures)
        times2.append(sum2/measures)
        occs.append(sumocc/measures)
    
    print (times1)
    print (times2)
    print (occs)

    plt.figure(figsize=(8, 5))

    plt.title(f'Tiempo de Búsqueda en texto {textname}', fontsize=14)
    plt.plot(ms, times1, color='r', linestyle='--', label='Búsqueda con estructura')
    plt.plot(ms, times2, color='g', linestyle='--', label='Búsqueda lineal')   

    theoretical = [theoretical_time(occ, n, r, m) for occ, m in zip(occs, ms)]
    plt.plot(ms, theoretical, color='b', linestyle='--', label='Predicción teórica con ajuste de 1/3200')

    plt.xlabel('Largo de patrón', fontsize=12)
    plt.ylabel('Tiempo (ms)', fontsize=12)
    plt.legend()  # Show the legend
    plt.grid(True)  # Add gridlines

        
    print (theoretical)
    # Display the plot
    plt.show()
    # print a latex table with the results
    print (f'"n" & "r" & {"m":<10} & {"ocurrencias":<10} & {"tiempo estructura":<10} & {"tiempo lineal":<10} & {"tiempo teórico":<10} \\\\')
    for i in range(len(ms)):
        print (f'{int(n)} & {int(r)} & {int(ms[i])} & {int(occs[i])} & {times1[i]:.2f} & {times2[i]:.2f} & {theoretical[i]:.2f} \\\\')
        

def main():
    #argv[1] is the filename
    plot_search_time(sys.argv[1], sys.argv[2])

if __name__ == '__main__':
    main()
