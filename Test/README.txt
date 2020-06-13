------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------- DOCUMENTACIÓN DEL REPOSITORIO DE TESTING --------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CHECKER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

DESCRIPCIÓN: Este archivo sirve de librería para las funciones de comprobación de existencia y modificación de ficheros
FICHERO: Checker.py
EJECUCIÓN (en command line): $ python Test_Output.py --> nota: NO TIENE SENTIDO EJECUTARLO PORQUE ES UNA LIBRERÍA !!!
FUNCIONES:

          this_file_exists(fichero):  - comprueba que existe el fichero con el path: "fichero"
                                      - devuelve un booleano -> True (existe) / False (no existe)

          file_modified(date1,path):  - comprueba si se ha modificado el fichero con path "path" y última modificación "date1"
                                      - devuelve un booleano -> True (se ha modificado) / False (no se ha modificado)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% GENERA INPUT 15 MIN %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

DESCRIPCIÓN: Librería que sirve para generar el input 15 minutal (a partir de uno mensual) cada periodo de tiempo "duerme". 
             Simula la generación del fichero input que tendremos en producción en tiempo real.
FICHERO: Select_15_min.py
EJECUCIÓN (en command line): $ python Select_15_min.py --> nota: NO TIENE SENTIDO EJECUTARLO PORQUE ES UNA LIBRERÍA !!!
FUNCIONES:

          process_(entrada, salida, duerme):  - genera ficheros input 15 minutales "salida" a partir del fichero mensual "entrada" cada "duerme"
                                              - el proceso terminará cuando se hayan leido todas las líneas de "entrada"
                                              - no devuelve nada. Simplemente genera el input 15 minutal e imprime el nº de líneas que ha leído

          process_stop(entrada, salida, duerme,iters): - genera ficheros input 15 minutales "salida" a partir del fichero mensual "entrada" cada "duerme"
                                                       - el proceso terminará cuando se hayan generado "iters" ficheros "salida"
                                                       - no devuelve nada. Simplemente genera el input 15 minutal e imprime el nº de líneas que ha leído

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TEST DE VERIFICACIÓN DE OUTPUT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DESCRIPCIÓN: Este test verifica que se generen los outputs y que sea con el formato correcto.
FICHERO: Test_Output.py
EJECUCIÓN (en command line en el directorio del test, con pytest): $ pytest Test_Output.py
DEPENDENCIAS/RECURSOS: Select_15_min.py , Checker.py (ambos en el mismo directorio del test)
PRECONDICIONES: 
        En el fichero configuration.yaml cambiamos los campos:
                 - newdata_timeout (segundos). Este valor habrá que ajustarlo para que sea mayor que "duerme" (+60)
                 - project_scope. Modo de ejecución -> realtime: para predicciones en tiempo real 
                                                    -> offline: para predicciones en bloque (no se mantiene levantado el programa)
SUBTESTS: 
                 - test_generating_files: comprueba que se generen los ficheros de resultado
                 - test_correct_format: comprueba que el fichero de resultado tenga el formato (dimensiones) correcto
PARÁMETROS:
      
                 - duerme: periodo de tiempo (segundos) que tardan en generarse los ficheros input 15 minutales
                 - inputMensual: path del archivo mensual del que se extraerá el input 15 minutal (input15m)
                 - input15m: path del archivo input 15 minutal que usarán los algoritmos
                 - iters: iteraciones del test (número de input15m que se generan)
                 - programa: ejecución (en cmd) del programa de predicción en un string (default: "python src\main.py)
                 - args: argumentos del programa de predicciones (default: "-c configuration.yaml")
                 - results: path del archivo de salida de predicciones

ESQUEMA:
       
       test_generating_files

               1.- Se ejecuta el proceso de generación del input15m con un thread.
               2.- Se ejecuta el proceso de checkeo de generación de output con otro thread.
                      2.1.- Se recoge la fecha de la última modificación del output.
                      2.2.- Se espera un tiempo adecuado (dependiente de "duerme") para que se genere el output.
                      2.3.- Se verifica si ha cambiado la fecha de modificación del output -> True/False.
                      2.4.- Se repite este paso "iters" veces y se guarda en una lista.
               3.- Se ejecuta el programa de predicciones.
               4.- Con la lista que devuelve el checker de existencia, se calcula el set y se verifica que es True.

       test_correct_format

               1.- Se ejecuta el proceso de generación del input15m con un thread.
               2.- Se ejecuta el programa de predicciones
               3.- Con un bucle que se ejecuta "iters" veces:
                      3.1.- Se espera un tiempo adecuado (dependiente de "duerme") para que se genere el output.
                      3.2.- Se comprueba el formato (en el sentido de dimensiones) del output -> True/False.
                      3.3.- Se repite este paso "iters" veces y se guarda en una lista.
               4.- Con la lista que devuelve el checker de formato, se calcula el set y se verifica que es True.

OUTPUT: En este caso, al usar la librería pytest, obtendremos:
                 - Passed si el test ha pasado -> Se cumple que que el set de la lista es True
                 - Error si no ha pasado el test -> El set de la lista es False
        Al tener dos tests, podemos obtener: 
                 - (1 Passed , 1 Error)
                 - (2 passed)
                 - (2 Error)
        En el caso de que fallara alguno, por pantalla se imprime el error y procedimiento.



<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TEST DE DEBUG >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DESCRIPCIÓN: Este test verifica que se generen los outputs mostrando los logs por pantalla y guardándolos.
FICHERO: Test_Debug.py
EJECUCIÓN (en command line en el directorio del test): $ python Test_Debug.py
DEPENDENCIAS/RECURSOS: Select_15_min.py , Checker.py (ambos en el mismo directorio del test)
PRECONDICIONES: 
        En el fichero configuration.yaml cambiamos los campos:
                 - newdata_timeout (segundos). Este valor habrá que ajustarlo para que sea mayor que "duerme" (+60)
                 - project_scope. Modo de ejecución -> realtime: para predicciones en tiempo real 
                                                    -> offline: para predicciones en bloque (no se mantiene levantado el programa)
PARÁMETROS:
      
                 - duerme: periodo de tiempo (segundos) que tardan en generarse los ficheros input 15 minutales
                 - inputMensual: path del archivo mensual del que se extraerá el input 15 minutal (input15m)
                 - input15m: path del archivo input 15 minutal que usarán los algoritmos
                 - iters: iteraciones del test (número de input15m que se generan)
                 - programa: ejecución (en cmd) del programa de predicción en un string (default: "python src\main.py)
                 - args: argumentos del programa de predicciones (default: "-c configuration.yaml")
                 - results: path del archivo de salida de predicciones

ESQUEMA:

               1.- Se ejecuta el proceso de generación del input15m con un thread.
               2.- Se ejecuta el proceso de checkeo de generación de output con otro thread.
                      2.1.- Se recoge la fecha de la última modificación del output.
                      2.2.- Se espera un tiempo adecuado (dependiente de "duerme") para que se genere el output.
                      2.3.- Se verifica si ha cambiado la fecha de modificación del output -> True/False.
                      2.4.- Se repite este paso "iters" veces y se guarda en una lista.
               3.- Se ejecuta el programa de predicciones.
               4.- Con la lista que devuelve el checker de existencia, se calcula el set y se verifica que es True.

OUTPUT: Se obtiene un log de los procesos. Realmente este test es para verificar que no hay ningún error en el programa
        de predicciones de los algoritmos, por lo que sirve de debug. Se espera no encontrar error y obtener True 
        (que indicará que se han generado resultados).


<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TEST DE MÉTRICAS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DESCRIPCIÓN: Este test loggea el rendimiento de los algoritmos (RAM, CPU, tiempo)
FICHERO: Test_time_RAM_CPU.py
EJECUCIÓN (en command line en el directorio del test): $ python Test_time_RAM_CPU.py
DEPENDENCIAS/RECURSOS: Select_15_min.py (en el mismo directorio del test)
PRECONDICIONES: 
        En el fichero configuration.yaml cambiamos los campos:
                 - newdata_timeout (segundos). Este valor habrá que ajustarlo para que sea mayor que "duerme" (+60)
                 - project_scope. Modo de ejecución -> realtime: para predicciones en tiempo real 
                                                    -> offline: para predicciones en bloque (no se mantiene levantado el programa)
                 - make_predictions -> activated: poner a True el algoritmo del que queramos obtener el log (= "algortithm")
                            
        Debe existir un fichero llamado logs_time_RAM_CPU.csv con los campos: 
                 - ALGORITHM;RAM;CPU;ITERS;TIME;DATE
PARÁMETROS:
      
                 - duerme: periodo de tiempo (segundos) que tardan en generarse los ficheros input 15 minutales
                 - inputMensual: path del archivo mensual del que se extraerá el input 15 minutal (input15m)
                 - input15m: path del archivo input 15 minutal que usarán los algoritmos
                 - iters: iteraciones del test (número de input15m que se generan)
                 - programa: ejecución (en cmd) del programa de predicción en un string (default: "python src\main.py)
                 - args: argumentos del programa de predicciones (default: "-c configuration.yaml")
                 - results: path del archivo de salida de predicciones
                 - algorithm: algoritmo del que vamos a obtener el log.

ESQUEMA:

               1.- Se ejecuta el proceso de generación del input15m con un thread.
               2.- Se ejecuta el programa de predicciones.
               3.- Se guardan las fechas de modificación de los ficheros de input y output.
               4.- Se abre el archivo logs_time_RAM_CPU.csv y comienza el siguiente bucle de "iters" iteraciones:
                      4.1.- Se activa el "cronómetro".
                      4.2.- Se recogen los datos de CPU, RAM.
                      4.3.- Se espera a que cambie la fecha de modificación del output.
                      4.4.- Se detiene el cronómetro, obteniendo el tiempo de predicción.
                      4.5.- Se escriben los datos en el log (tiempo, RAM, CPU).
                      4.6.- Se espera a que cambie la fecha de modificación del input.
               5.- Se cierra el fichero de log.

OUTPUT: Se obtiene un log de métricas de rendimiento. Por pantalla obtendremos las fechas de modificación de los outputs e inputs.

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TEST DE ACTIVIDAD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DESCRIPCIÓN: Este test verifica que se generen los outputs del algoritmo que hemos configurado como activo.
FICHERO: Test_Output.py
EJECUCIÓN (en command line en el directorio del test, con pytest): $ pytest Test_Output.py
DEPENDENCIAS/RECURSOS: Select_15_min.py , Checker.py (ambos en el mismo directorio del test)
PRECONDICIONES: 
        En el fichero configuration.yaml cambiamos los campos:
                 - newdata_timeout (segundos). Este valor habrá que ajustarlo para que sea mayor que "duerme" (+60)
                 - project_scope. Modo de ejecución -> realtime: para predicciones en tiempo real
                                                    -> offline: para predicciones en bloque (no se mantiene levantado el programa)
                 - make_predictions -> activated: poner a True el algoritmo del que queramos predicciones

PARÁMETROS:
      
                 - duerme: periodo de tiempo (segundos) que tardan en generarse los ficheros input 15 minutales
                 - inputMensual: path del archivo mensual del que se extraerá el input 15 minutal (input15m)
                 - input15m: path del archivo input 15 minutal que usarán los algoritmos
                 - iters: iteraciones del test (número de input15m que se generan) -> en este caso es 1
                 - programa: ejecución (en cmd) del programa de predicción en un string (default: "python src\main.py)
                 - args: argumentos del programa de predicciones (default: "-c configuration.yaml")
                 - results: path del archivo de salida de predicciones

ESQUEMA:

               1.- Se ejecuta el proceso de generación del input15m con un thread.
               2.- Se ejecuta el programa de predicciones.
               3.- Cuando termine el proceso y se haya generado output:
                      3.1.- Se recogen los estados de los algoritmos (descritos en "configuration.yaml") en un diccionario "dic".
                      3.2.- Se genera un set "algsDic" de los algoritmos activos descritos en "dic".
                      3.3.- Se recogen los algoritmos activos (de los que se genera output en results.csv) en un set "algsCsv".
               4.- Se verifica que el conjunto (set) algsDic sea igual al de algsCsv.

OUTPUT: En este caso, al usar la librería pytest, obtendremos:
                 - Passed si el test ha pasado -> Se cumple que que: algsDic = algsCsv --> se generan predicciones de los algoritmos activados
                 - Error si no ha pasado el test -> algsDic != algsCsv --> no se estan generando las predicciones configuradas
        En el caso de que fallara, por pantalla se imprime el error y procedimiento.



