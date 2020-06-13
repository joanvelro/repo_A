import pytest
import threading
import time
from subprocess import call
from Select_15_min import process_stop
import numpy as np
import csv

def llamada(programa,parametros):
    i = 0
    params = ""
    while (i<len(parametros)):
        params = params + parametros[i] + " "
    return_code = call([programa, params])
    return return_code

def genera_diccionario():
    dic = {'1':{'Active':True},'2':{'Active':True},'3':{'Active':False},'4':{'Active':False},'5':{'Active':True}}
    return dic

def Select_alg_column(fichero):
    f = open(fichero, 'r')
    reader = csv.reader(f,delimiter=";")
    headers = next(reader)
    nombres = list(headers)
    index = nombres.index("ALGORITHM_ORIGIN")
    data = list(reader)
    data2 = np.array(data)[:,index]
    algs = set(data2)
    return algs

def algs_active(diccionario):
    resul = list()
    for algorithm in diccionario.keys():
        if (diccionario.get(algorithm).get('Active')==True):
            resul.append(algorithm)
    return set(resul)

class Test(object):
    def test_active(self):

        dic = genera_diccionario()
        algsDic = algs_active(dic)

        fichero = "prediccion.csv"
        algsCsv = Select_alg_column(fichero)

        assert (algsDic == algsCsv)

# mirar si coincide con set(ALGORITHM_ORIGIN)
