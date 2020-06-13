from subprocess import call
from Select_15_min import process_stop
import threading
import psutil
import os
import time
import csv

def llamada(programa,parametros):
    i = 0
    params = ""
    while (i<len(parametros)):
        params = params + parametros[i] + " "
    return_code = call([programa, params])
    return return_code

def memory_usage_psutil():
    process = psutil.Process(os.getpid())
    mem = process.memory_info().rss / float(2 ** 20) # return the memory usage in MB
    memory = psutil.virtual_memory()
    cpu = psutil.cpu_percent()
    return mem,memory,cpu

def date_last_modify(path):
    date = os.path.getmtime(path)
    return date

algorithm = "Minsait"
iters = 10
log = "logs_time_RAM_CPU.csv"

""" Ejecucion Select_15_min.py """

duerme = 360  # segundos
entrada = "Datos_Manual_Febrero19.csv"
input = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\data\\TT_NL.csv"
resultados = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\output\\results.csv"

t1 = threading.Thread(target=process_stop, args=(entrada, input, duerme, iters))

""" Ejecucion MULTIALGORITMO """

programa = "python C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\src\\main.py"
args = "-c C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\configuration.yaml"
parametros = [args]
line_comand = programa + " " + args
t2 = threading.Thread(target=os.system, args=(line_comand,))

t1.start() # Select_15_min se inicia
# os.system(line_comand) # Llamamos al programa de predicciones
t2.start() # Llamamos al programa de predicciones
time.sleep(3)
date1_out = date_last_modify(resultados)
date1_in = date_last_modify(input)
date2_out = date1_out
date2_in = date1_in
i = 1
print ("ANNNTES DE ABRIR")
with open(log,"a",newline='') as f:
    print("DESPUES DE ABRIR")
    while (i<=iters):
        print("Iteración número:", i)
        print("Ultima modificacion del input:", time.strftime('%m/%d/%Y_%H:%M:%S', time.gmtime(date1_in)))
        print("Ultima modificacion del output:", time.strftime('%m/%d/%Y_%H:%M:%S', time.gmtime(date1_out)))
        init = time.clock()
        date = time.strftime("%d:%m:%y_%H:%M:%S")
        RAM, memory, CPU = memory_usage_psutil()
        while (date1_out==date2_out):  # Espera a tener una nueva prediccion
            time.sleep(1)
            date2_out = date_last_modify(resultados)
        print("Nueva modificacion del output:", time.strftime('%m/%d/%Y_%H:%M:%S', time.gmtime(date2_out)))
        end = time.clock()
        time_elapsed = (end - init)
        line = {'ALGORITHM':str(algorithm), 'RAM':str(RAM), 'CPU':str(CPU), 'ITER':(str(i)+"/"+str(iters)), 'TIME':str(time_elapsed), 'DATE':str(date)}
        # line_ = {'ALGORITHM':"Min22",'RAM':"32",'CPU':"33",'ITERS':"1",'TIME':"324",'DATE':str(date)}
        fieldnames = ['ALGORITHM', 'RAM', 'CPU', 'ITER', 'TIME', 'DATE']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writerow(line)
        while ((date1_in==date2_in)&(i<iters)):  # Espera a tener un nuevo input
            time.sleep(1)
            date2_in = date_last_modify(input)
        print("Nueva modificacion del input:", time.strftime('%m/%d/%Y_%H:%M:%S', time.gmtime(date2_in)))
        date1_out = date2_out
        date1_in = date2_in
        i = i+1
f.close()
t1.join()
t2.join()