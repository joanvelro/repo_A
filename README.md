# Algoritmos predictivos ITS

Este repositorio contiene utilidades para el desarrollo y puesta en marcha de soluciones de forecasting en el área de transportes. 

Algunos ejemplos de problemas de analítica predictiva en sistemas de transporte inteligentes (ITS) son:
-  Predicción de tiempos de viaje
-  Predicción de flujo de tráfico
-  Predicción de accidentes
-  Predicción de fallos en infraestructura
-  Predicción de flujos de pasajeros en aeropuertos 

> **Nota:** Estos ejemplos han sido extraídos de los pilotos desarrollados por Indra como parte del proyecto europeo H2020 de Transforming Transport (https://transformingtransport.eu/) 

## Estructura del repositorio
Este repositorio contiene la siguiente estructura:
```
├── LICENSE
├── README.md                          # Este fichero
│
├── docs                               # Documentación de código auto-generada
│
├── models                             # Modelos entrenados y serializados 
│   └── proyecto_X             
│
├── notebooks                          # Jupyter notebooks utilizados para exploración de datos
│   └── proyecto_X             
│
├── references                         # Data dictionaries, manuals, and all other explanatory materials.
│
├── reports                            # Informes de análisis generados como HTML, PDF, LaTeX, etc.
│   └── proyecto_X             
│       └── figuras        
│
├── requirements.txt                    # Los requisitos de librerias y paquetes, e.g. `pip freeze > requirements.txt`
│
├── setup.py                            # Para hacer el proyecto python instalable con `pip install -e`
├── src                                 # Código fuente del proyecto. Core del paquete con utilidades reutilizables
│   ├── __init__.py    
│   │
│   ├── dataprocessing_utils            # Utilidades para recuperar/ transformar/pre-procesar datos 
│   │   └── dataset_retrieval.py
│   │   └── build_features.py
│   ├── algoritmos                      # Algoritmos de analítica predictiva
│   ├── models                          # Utilidades para entrenar modelos y hacer predicciones
│   │   ├── predict_model.py
│   │   └── train_model.py
│   │
│   └── evaluation                      # Utilidades para evaluar el rendimiento de los algoritmos 
│       └── realtime_monitoring.py
│       └── eval_metrics.py
└── 
```

## Ejecucion

- Modo de ejecucion 1 (Solo para realizar predicciones)
```
$ python main._main(project_name, project_scope, execution_mode, reports_path, input_data, output_path, logs_path, newdata_timeout, algorithms_configuration)
```
 
- Modo de ejecucion 2
```
$ python main.py -c configuration.yaml
```

Un ejemplo de parámetros de configuración se encuentra en el fichero configuration.yaml



## Instalación del repositorio
Para la instalación de este proyecto, será necesario Python 3.6 y pip (gestor de dependencias de python)

Instrucciones de instalación:

- [Python 3.6](https://www.python.org/downloads/)

- [Gestor de paquetes para Data Science Anaconda](https://docs.anaconda.com/anaconda/install/)

- [Spark](http://desappstre.com/guia-de-instalacion-de-apache-spark-en-windows/)


**Instalación de Dependencias**

Una vez se disponga de Python 3.6 y Anaconda. Será necesario ejecutar el siguiente comando para instalar el resto de dependencias:

```
$ conda env create -f environment.yml​
```

Este comando crea un entorno virtual con todas las dependencias necesarias por este proyecto

Si se añade alguna otra dependencia en el desarrollo del mismo se puede utilizar el siguiente comando para generar el fichero 'environment.yml' que incluya las nuevas dependencias

```
$ conda env export > environment.yml
```

La instalación de este proyecto se ha probado en Windows 10 Pro


**Instalación de Spark**

Se recomienda seguir esta [guia de instalación](http://desappstre.com/guia-de-instalacion-de-apache-spark-en-windows/)

Resumen de requisitos básicos:

- Instalar Java JDK

    1) Descargar e instalar Java JDK 8 
    2) Establecer la variable en entorno JAVA_HOME
    3) Añadir %JAVA_HOME% bin al PATH

- Instalar y configurar variables de entorno de [Apache Spark](http://spark.apache.org/downloads.html)
    
    1) SPARK_HOME: C:\opt\spark\spark-2.1.0-bin-hadoop2.7
    2) PATH:  C:\opt\spark\spark-2.1.0-bin-hadoop2.7\bin

- Instalar [Winutils](https://github.com/steveloughran/winutils)

## Contribute to this repository
This project follows the Google Python Style Guide. All contributions have to follow the guidelines defined there.

- Developer's Style Guide: [Google Python Style Guide](https://github.com/google/styleguide/blob/gh-pages/pyguide.md)

- Documentation: [Sphinx Tool](http://www.sphinx-doc.org)



cambios 


