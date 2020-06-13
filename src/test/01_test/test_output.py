# encoding: utf-8
from subprocess import call
import os
import threading
import time
import pytest
import csv
import numpy as np

def read_from_csv_(file):
    fp = open(file, 'r')
    names = fp.readline()
    a = fp.readlines()
    return a,names

def write_in_csv_(file,a,names,i):
    fp = open(file, 'w')
    fp.writelines(names)
    fp.writelines(a[i:(i+20)])
    fp.close()

def process_(entrada, salida, duerme):
    a, nombres = read_from_csv_(entrada)
    i = 0
    while i < len(a):
        write_in_csv_(salida, a, nombres, i)
        i = i + 20
        time.sleep(duerme)
        os.remove(salida)
        print(i)

def process_stop(entrada, salida, duerme, iters):
    """
    Este metodo se encarga de generar los fichero quinceminutales esperados como input por el programa MultiAlgoritmo usando
    como entrada fichero con mas datos (e.g. mensuales)

    :param entrada: fichero de entrada con datos mensuales a particionar para generar un fichero de salida quinceminutal
    :param salida: fichero generado con datos de 15 mint para todos los segmentos
    :param duerme: periodo en segundo que espera para generar el fichero quiceminutal de salida
    :param iters: numero de ficheros a generar o llamadas a este metodo
    """
    a, nombres = read_from_csv_(entrada)
    i = 0
    lines = iters * 20
    while (i < len(a)) & (i < lines):
        write_in_csv_(salida, a, nombres, i)
        i = i + 20
        time.sleep(duerme)
        os.remove(salida)
        print(i)

def this_file_exists(fichero):
    a = os.path.isfile(fichero)
    return a

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
    reader = csv.reader(f,delimiter=";")
    headers = next(reader)
    data = list(reader)
    data = np.array(data)
    print(data.shape)
    resul = True if (data.shape == (filas,columnas)) else False
    return resul

class Test(object):

    def test_generating_files(self):
        """ Ejecucion Select_15_min.py """

        duerme = 6  # segundos
        entrada = r'C:\Users\yhoz\Documents\dataanalytics.predictive\src\test\test_data\01-18.csv'
        salida = r'C:\Users\yhoz\Documents\dataanalytics.predictive\data\TT_NL.csv'
        iters = 4
        # threads = list()
        print("dfdsfsdf")
        t1 = threading.Thread(target=process_stop, args=(entrada, salida, duerme, iters))

        # process_(entrada,salida,duerme)

        """ Ejecucion MULTIALGORITMO """

        programa = r'python C:\Users\yhoz\Documents\dataanalytics.predictive\src\main.py'
        parametros = [r'-c C:\Users\yhoz\Documents\dataanalytics.predictive\configuration.yaml']

        t2 = threading.Thread(target=llamada, args=(programa, parametros))

        # llamada(programa,parametros)

        """ Ejecucion Checker.py"""

        # fichero = "predicciones"
        fichero = r'C:\Users\yhoz\Documents\dataanalytics.predictive\output\results.csv'

        t1.start()
        t2.start()
        resul = list()
        i = 0
        """while (i < iters):
            time.sleep(duerme)
            result.append(this_file_exists(fichero))
            i = i + 1
            if i == 2:
                t2.terminate()
"""
        t1.join()
        t2.join()
        print(resul)

        assert (set(resul) == {True})



    ### Para este test podríamos añadir que el header fuera el correcto

    def test_correct_format(self):
        duerme = 6 # segundos
        entrada = r'C:\Users\yhoz\Documents\dataanalytics.predictive\src\test\test_data\01-18.csv'
        salida = r'C:\Users\yhoz\Documents\dataanalytics.predictive\data\TT_NL.csv'
        iters = 4

        hilo1 = threading.Thread(target=process_stop, args=(entrada, salida, duerme, iters))

        programa = r'python C:\Users\yhoz\Documents\dataanalytics.predictive\src\main.py'
        parametros = [r'-c C:\Users\yhoz\Documents\dataanalytics.predictive\configuration.yaml']

        hilo2 = threading.Thread(target=llamada, args=(programa, parametros))

        fichero = r'C:\Users\yhoz\Documents\dataanalytics.predictive\output\results.csv'

        hilo1.start()
        hilo2.start()
        resul = list()
        i = 0
        filas = 20
        columnas = 33
        while (i < iters):
            time.sleep(duerme)
            resul.append(comprueba_formato(fichero, filas, columnas))
            i = i + 1
        hilo1.join()
        hilo2.join()
        print(resul)
        assert (set(resul) == {True})