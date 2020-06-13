# -*- coding: utf-8 -*-
from pyspark.sql import SparkSession
import pandas as pd
from datetime import datetime, timedelta

def generate_vehicles_dataset(time_range, sqlserver_password):
    """ Generates the input dataset with the features (explanatory and target variables) to be used to train the algorithms

    Args:
        time_range (str): The time range of the data
        sqlserver_password (str): The secret password to connect to the database

    Returns:
        dataframe: Returns a Spark's DataFrame with the input dataset (df_vehicles_dataset)

    """

    sqlQuery_input_algorithm= "(SELECT Segmento AS ID_SEGMENT" \
              ", Tipo_dia AS COD_LABORALIDAD" \
              ", Fecha AS FECHA" \
              ", Fecha_mes AS MES" \
              ", Fecha_hora AS HORA" \
              ", Fecha_minuto AS MINUTO" \
              ", cant_precip AS CANTIDAD_PREC" \
              ", [1] AS TOTAL_VEHICULOS" \
              ", [2] AS TOTAL_VEHICULOS_1" \
              ", [3] AS TOTAL_VEHICULOS_2" \
              ", [4] AS TOTAL_VEHICULOS_3" \
              ", [5] AS TOTAL_VEHICULOS_4" \
              ", [6] AS TOTAL_VEHICULOS_5" \
              ", [7] AS TOTAL_VEHICULOS_6" \
              ", [8] AS TOTAL_VEHICULOS_7" \
              ", [9] AS TOTAL_VEHICULOS_8" \
              ", [10] AS TOTAL_VEHICULOS_9" \
              ", [11] AS TOTAL_VEHICULOS_10" \
              ", [12] AS TOTAL_VEHICULOS_11" \
              ", [13] AS TOTAL_VEHICULOS_12" \
              ", [14] AS TOTAL_VEHICULOS_13" \
              ", [15] AS TOTAL_VEHICULOS_14" \
              ", [16] AS TOTAL_VEHICULOS_15" \
              ", [17] AS TOTAL_VEHICULOS_16" \
              ", [18] AS TOTAL_VEHICULOS_17" \
              ", [19] AS TOTAL_VEHICULOS_18" \
              ", [20] AS TOTAL_VEHICULOS_19" \
              ", [21] AS TOTAL_VEHICULOS_20" \
              ", [22] AS TOTAL_VEHICULOS_21" \
              ", [23] AS TOTAL_VEHICULOS_22" \
              ", [24] AS TOTAL_VEHICULOS_23" \
              ", [25] AS TOTAL_VEHICULOS_24" \
              " FROM TALEND_GET_TRAFFIC_FLOW_PERIOD " + str(time_range) + ") tmp"

    df_vehicles_dataset = spark.read.format('jdbc').option("driver" , "com.microsoft.sqlserver.jdbc.SQLServerDriver")\
                                  .option("url", r'jdbc:sqlserver://10.0.0.6\NORTE;databaseName=HIST_EXT_HORUS') \
                                  .option("dbtable", sqlQuery_input_algorithm) \
                                  .option("user", "sa")\
                                  .option("password", sqlserver_password)\
                                  .load()


    return df_vehicles_dataset

def generate_result_dataset(time_range, sqlserver_password):
    """ Generates the results dataset containing the features along with the outcomes to be used for testing the algorithms

    Args:
        time_range (str): The time range of the data
        sqlserver_password (str): The secret password to connect to the database

    Returns:
        dataframe: Returns a Spark's DataFrame with the input dataset (df_results_dataset)

    """

    sqlQuery= "(SELECT SEGMENTO as ID_SEGMENT, * from F_GET_FLOW_FORECAST ('1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20', " + "'" + str(time_range[0]) + "'" +  "," + "'" +str(time_range[1]) +"'" + ")) tmp"

    df_results_dataset = spark.read.format('jdbc').option("driver" , "com.microsoft.sqlserver.jdbc.SQLServerDriver")\
                                  .option("url", r'jdbc:sqlserver://10.0.0.6\NORTE;databaseName=HIST_EXT_HORUS') \
                                  .option("dbtable", sqlQuery) \
                                  .option("user", "sa")\
                                  .option("password", sqlserver_password)\
                                  .load()

    df_results_dataset.show(5)
    return df_results_dataset

