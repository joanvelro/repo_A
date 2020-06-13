from subprocess import call
from Checker import this_file_exists
import Select_15_min_AUSOL
from Select_15_min_AUSOL import process_stop
import threading
import time
import pytest
import csv
import numpy as np
from multiprocessing.pool import ThreadPool
import subprocess
import os


def llamada(programa,parametros):
    i = 0
    params = ""
    while (i<len(parametros)):
        params = params + parametros[i] + " "
    return_code = call([programa, params])
    return return_code

# Los datos tienen 32 columnas y 21 filas
# Las predicciones tienen (20 x N) + 1 filas con N el n de algoritmos
# Las predicciones tienen 15 + 1 filas

def comprueba_formato(fichero,filas,columnas):
    f = open(fichero, 'r')
    reader = csv.reader(f,delimiter=";")
    headers = next(reader)
    data = list(reader)
    data = np.array(data)
    print(data.shape)
    resul = True if (data.shape == (filas,columnas)) else False
    return resul

def comprueba_existencia(iters, duerme, fichero):
    i = 0
    resul = list()
    while (i < iters):
        time.sleep(duerme)
        resul.append(this_file_exists(fichero))
        i = i + 1
    return resul

duerme = 10  # segundos (en caso de minsait, minimo: 300, en otro caso podemos poner un minimo de 240)
inputMensual = "input_data_ausol.csv" # Fichero input mensual
input15m = "C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\data\\TT_NL.csv" # Fichero input 15 minutal
iters = 4
# threads = list()
t1 = threading.Thread(target=process_stop, args=(inputMensual, input15m, duerme, iters))

# process_(entrada,salida,duerme)

programa = "python C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\src\\main.py"
args = "-c C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\configuration.yaml"
parametros = [args]

line_comand = programa + " " + args

# t2 = threading.Thread(target=llamada, args=(programa, parametros))

# llamada(programa,parametros)

""" Ejecucion Checker.py"""

t3 = ThreadPool(processes=1)

# fichero = "predicciones"
results = "C:\\OrquestadorTT\\AUSOL\\Algoritmos\\dataanalytics.predictive\\output\\results.csv" # Fichero de resultados

t1.start()
async_result = t3.apply_async(comprueba_existencia, (iters, duerme, results))  # comprueba_existencia(iters, duerme, fichero)
# llamada(programa, parametros)
time.sleep(4)
os.system(line_comand)
t1.join()
resul = async_result.get()
# t2.join()
print(resul)