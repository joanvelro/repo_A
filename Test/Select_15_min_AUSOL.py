import time
import os
import pandas as pd

def read_from_csv_(file):
    df = pd.read_csv(file, delimiter=';', header=0)
    return df

def write_in_csv_(file,df):
    df.to_csv(file, sep=';', index=False)

def new_dataframe(df, date):
    df_date = df.loc[df.FECHA == date]
    return df_date

def process_stop(entrada, salida, duerme,iters):
    df = read_from_csv_(entrada)
    dates = df['FECHA'].unique()
    i = 0
    for date in dates:
        i = i+1
        df_date = new_dataframe(df, date)
        write_in_csv_(salida, df_date)
        time.sleep(duerme)
        os.remove(salida)
        if (i >= iters):
            break
        print(i)


#duerme = 10  # segundos
#entrada = "D:\\Usuarios\mdbrenes\Documents\prueba\Predicciones Anual\enero18.csv"
#salida = "salidaTest.csv"

#process_(entrada,salida,duerme)