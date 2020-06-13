import time
import os

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
        time.sleep(duerme)
        print(i)

def process_stop(entrada, salida, duerme,iters):
    a, nombres = read_from_csv_(entrada)
    i = 0
    lines = iters * 20 # num de lineas total a leer
    while (i < len(a))&(i < lines):
        write_in_csv_(salida, a, nombres, i)
        i = i + 20
        time.sleep(duerme)
        os.remove(salida)
        time.sleep(duerme)
        print(i)


#duerme = 10  # segundos
#entrada = "D:\\Usuarios\mdbrenes\Documents\prueba\Predicciones Anual\enero18.csv"
#salida = "salidaTest.csv"

#process_(entrada,salida,duerme)