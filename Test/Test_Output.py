from subprocess import call
from Checker import this_file_exists
from Checker import file_modified
import Select_15_min
from Select_15_min import process_stop
import threading
import time
import os
import csv
import numpy as np
from multiprocessing.pool import ThreadPool

def llamada(programa,parametros):
    i = 0
    params = ""
    while (i<len(parametros)):
        params = params + parametros[i] + " "
    return_code = call([programa, params])
    return return_code

# Los datos tienen 32 columnas y 21 filas
# Las predicciones tienen (20 x N) + 1 filas con N el nº de algoritmos
# Las predicciones tienen 15 + 1 filas

def comprueba_formato(fichero,filas,columnas):
    f = open(fichero, 'r')
    reader = csv.reader(f,delimiter=",")
    headers = next(reader)
    data = list(reader)
    data = np.array(data)
    print(data.shape)
    resul = True if (data.shape == (filas,columnas)) else False
    return resul

def comprueba_existencia(iters, duerme, fichero):
    i = 0
    resul = list()
    date1 = 0
    if (this_file_exists(fichero)):
        date1 = os.path.getmtime(fichero)
    while (i < iters):
        time.sleep(duerme-(int(duerme/10)))
        modified = False
        if (this_file_exists(fichero)):
            modified = file_modified(date1,fichero)
        resul.append(modified)
        time.sleep(duerme/10)
        i = i + 1
    return resul

class Test(object):

    def test_generating_files(self):
        """ Ejecucion Select_15_min.py """

        duerme = 300  # segundos
        entrada = "Datos_Manual_Febrero19.csv"
        salida = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\data\\TT_NL.csv"
        iters = 4
        # threads = list()
        t1 = threading.Thread(target=process_stop, args=(entrada, salida, duerme, iters))

        # process_(entrada,salida,duerme)

        """ Ejecucion MULTIALGORITMO """

        programa = "python C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\src\\main.py"
        args = "-c C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\configuration.yaml"
        line_comand = programa + " " + args

        # t2 = threading.Thread(target=llamada, args=(programa, parametros))
        t2 = threading.Thread(target=os.system, args=(line_comand,))

        # llamada(programa,parametros)

        """ Ejecucion Checker.py"""

        t3 = ThreadPool(processes=1)

        # fichero = "predicciones"
        fichero = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\output\\results.csv"

        t1.start()
        async_result = t3.apply_async(comprueba_existencia, (iters, duerme, fichero))  # comprueba_existencia(iters, duerme, fichero)
        # llamada(programa, parametros)
        t2.start()


        resul = list()

        t1.join()
        t2.join()
        resul = async_result.get()
        print(resul)

        assert (set(resul) == {True})

    ### Para este test podríamos añadir que el header fuera el correcto

def test_correct_format():

    duerme = 300  # segundos
    entrada = "Datos_Manual_Febrero19.csv"
    salida = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\data\\TT_NL.csv"
    iters = 4

    hilo1 = threading.Thread(target=process_stop, args=(entrada, salida, duerme, iters))

    programa = "python C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\src\\main.py"
    args = "-c C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\configuration.yaml"
    line_comand = programa + " " + args

    # hilo2 = threading.Thread(target=llamada, args=(programa, parametros))
    hilo2 = threading.Thread(target=os.system, args=(line_comand,))

    fichero = "C:\\OrquestadorTT\\NORTE\\Algoritmos\\dataanalytics.predictive\\output\\results.csv"

    hilo1.start()
    hilo2.start()
    resul = list()
    i = 0
    filas = 20
    columnas = 9
    while (i < iters):
        time.sleep(duerme-int(duerme/10))
        resul.append(comprueba_formato(fichero, filas, columnas))
        i = i + 1
        time.sleep(int(duerme / 10))
    hilo1.join()
    hilo2.join()
    print(resul)
    assert (set(resul) == {True})
