import os
import csv


def this_file_exists(fichero):
    a = os.path.isfile(fichero)
    return a

# file = "prediccion.csv"
# data = Select_first_column(file)
# print(data)
# print(set(data))


# prueba = "datos.csv"
# print(comprueba_formato(prueba,17280,5))