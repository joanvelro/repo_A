import os
import csv


def this_file_exists(fichero):
    a = os.path.isfile(fichero)
    return a

def file_modified(date1,path):
    date2 = os.path.getmtime(path)
    modified = False if (date2==date1) else True
    return modified


# file = "prediccion.csv"
# data = Select_first_column(file)
# print(data)
# print(set(data))


# prueba = "datos.csv"
# print(comprueba_formato(prueba,17280,5))