# -*- coding: utf-8 -*-
#from pyramid.arima import auto_arima
import numpy as np
import logging
import sys
from statsmodels.tsa.api import ExponentialSmoothing, SimpleExpSmoothing, Holt
from statsmodels.tsa.arima_model import ARIMA
import statsmodels.api as sm
import matplotlib.pyplot as plt
from statsmodels.tsa.stattools import acf
import matplotlib.pylab as plt
#from fbprophet import Prophet
#from tbats import BATS, TBATS
from datetime import datetime, timedelta
from dateutil.relativedelta import relativedelta
import pandas as pd

def getClosest_DateCode(ref_data_seg, current_dateCode, last_year_date):
    """
    This function returns the closest day with the same code according to last year reference data
    :param current_date: this is the current date. The starting point to make forecast
    :param current_dateCode:  code into forecast date
    :param last_year_date: last year date (reference data)
    :return: day (date): closest day
    """
    i = 0
    first = True
    code1 = 0
    code2 = 0
    day_plus = None
    day_minus = None

    if last_year_date.year == ref_data_seg['FECHA'].iloc[0].year:
        while((code1!=current_dateCode) & (code2 != current_dateCode)):
            if first: # TODO: refractor this part of code and put at the begining of the function
                code1 = ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == last_year_date]['COD_LABORALIDAD'].item()
                first = False
                i = i+1
                day_plus = pd.to_datetime(last_year_date)
                day_minus = pd.to_datetime(last_year_date)
            else:
                try:
                    day_plus = day_plus + timedelta(days=i)
                    if (day_plus.year == last_year_date.year):
                        if len(ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == day_plus]['COD_LABORALIDAD'].index) == 0:
                            code1 = current_dateCode
                        else:
                            code1 = ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == day_plus]['COD_LABORALIDAD'].iloc[0]

                    day_minus =  day_minus - timedelta(days=i)
                    if(day_minus.year == last_year_date.year):
                        if len(ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == day_minus]['COD_LABORALIDAD'].index)==0:
                            code2=current_dateCode
                        else:
                            code2 = ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == day_minus]['COD_LABORALIDAD'].iloc[0]
                except OverflowError as err:
                    print("Not found day with same code in last year data, using the same day")
                    return last_year_date

        if code2==current_dateCode:
            return day_minus
        else:
            return day_plus
    else:
        logging.error("No reference data available for the descriptive model")
        sys.exit(0)


def descriptive_model(ref_data_seg, current_date, segment_id, current_dateCode, total_vehiclesBuffer, smooth_mode, forecast_horizon):
    """
    :param reference_data: it could be the collected anual data, the collected anual data using the monthly code average
    :param current_date: this is the current date. The starting point to make forecast
    :param segment_id:  segment_id selected in forecast (forecast are made by segment)
    :param current_dateCode:  code into forecast date
    :param total_vehiclesBuffer: n old values (before forecast date)
    :param smooth_mode: type of smoothing method
    :param forecast_horizon: number of predictions to be made
    :return: predictions (list): a list with the forecast values
    """
    last_year_date= current_date - relativedelta(years=1)
    if len(ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) == last_year_date].index) == 0: # NO DATA AVAILABLE FOR THAT DAY
        closest_ref_date=ref_data_seg.loc[pd.to_datetime(ref_data_seg['FECHA']) >= last_year_date]['FECHA'].iloc[0] # get first available
    else:
        closest_ref_date=getClosest_DateCode(ref_data_seg, current_dateCode, last_year_date)

    predictions = ref_data_seg.loc[(pd.to_datetime(ref_data_seg['FECHA'])>=closest_ref_date)&(pd.to_datetime(ref_data_seg['FECHA'])<=(closest_ref_date+ timedelta(minutes=forecast_horizon)))]
    return predictions['TOTAL_VEHICULOS'].values

def stats_model(reference_data, current_date, segment_id, current_dateCode, total_vehiclesBuffer, smooth_mode, forecast_horizon, algo_name):
    """
    :param reference_data: it could be the collected anual data, the collected anual data using the monthly code average
    :param current_date: this is the current date. The starting point to make forecast
    :param segment_id:  segment_id selected in forecast (forecast are made by segment)
    :param current_dateCode:  code into forecast date
    :param total_vehiclesBuffer: n old values (before forecast date)
    :param smooth_mode: type of smoothing method
    :param forecast_horizon: number of predictions to be made
    :return: predictions (list): a list with the forecast values
    """
    predictions=[]
    if algo_name=='fbprophet':
        predictions = reference_data.loc[(reference_data.MES == current_date.month) & (reference_data.ID_SEGMENT == segment_id)  & ((reference_data.DAY*24*60)+(reference_data.HOUR*60 + reference_data.MINUTE) >=((current_date.day*24*60)+current_date.hour*60 +current_date.minute))]['TOTAL_VEHICULOS'].values
    else:
        ref_data = reference_data.loc[(reference_data.MES==current_date.month)&(reference_data.ID_SEGMENT==segment_id)&(reference_data.COD_LABORALIDAD==current_dateCode)]
        if current_date.hour >=22: # circle hour
            predictions=[]
            predictions.extend(ref_data.loc[(pd.to_datetime(ref_data['TIME']).dt.hour * 60 + (pd.to_datetime(ref_data['TIME']).dt.minute)) >= (current_date.hour * 60 + current_date.minute)]['TOTAL_VEHICULOS'].values)
            predictions.extend(ref_data.loc[(pd.to_datetime(ref_data['TIME']).dt.hour >= 0) & (pd.to_datetime(ref_data['TIME']).dt.hour <= 2)]['TOTAL_VEHICULOS'].values) # take 2 hours more to complete
        else:
            predictions = ref_data.loc[(pd.to_datetime(ref_data['TIME']).dt.hour*60+(pd.to_datetime(ref_data['TIME']).dt.minute))>=(current_date.hour*60+current_date.minute)]['TOTAL_VEHICULOS'].values
    return predictions


def auto_arima_model(data):
    """ This method train an ARIMA model using the maximum likelihood method to find automatically the best paramaters.
        See pkg documentation, for implementation details.

    Args:
        data (dataframe): The training dataset used to train the model

    Returns:
        model: The trained model

    """
    """   stepwise_model = auto_arima(data, start_p=2, start_q=2,
                           max_p=3, max_q=3,m=96,
                           start_P=0, seasonal=True,
                           d=1, D=1, trace=True,
                           error_action='ignore',
                           suppress_warnings=True,
                           stepwise=True)
    """
    return None #stepwise_model

def arima_model(train):
    """ This method train an ARIMA model
    Args:
        data (dataframe): The training dataset used to train the model

    Returns:
        model: The trained model

    """
    #model = ARIMA(train, order=(96, 0, 1))
    model = sm.tsa.statespace.SARIMAX(train, trend='ct', order=(1, 1, 1), seasonal_order=(1, 1, 1, 96), enforce_stationarity=True, enforce_invertibility=False)
    return model

def prophet_model(df_training):
    """ This method train an FBPROPHET model using the well-known developed pkg by facebook called fbprophet.
        See https://facebook.github.io/prophet/docs/quick_start.html for implementation details
    Args:
        df_training (dataframe): The training dataset used to train the model

    Returns:
        model: The trained model
    """
    df_training['ds'] = df_training.index
    df_training['y'] = df_training['TOTAL_VEHICULOS']
    train_cols = df_training[['ds', 'y']]
    model = None#Prophet()  # instantiate Prophet
    model.fit(train_cols);  # fit the model with your dataframe
    return model

def holtwinters_exp_smoothing_model(train):
    """ This method train an Exponential Smoothing model (Holt-Winters)
        See https://otexts.com/fpp2/holt-winters.html for implementation details
    Args:
        df_training (dataframe): The training dataset used to train the model

    Returns:
        model: The trained model
    """
    ses_model = ExponentialSmoothing(np.asarray(train), seasonal_periods=int(96), seasonal='add').fit()
    return ses_model

def tbats_model():
    """ This method train an TBATs model
    Args:
        df_training (dataframe): The training dataset used to train the model

    Returns:
        model: The trained model
    """
    model = None#TBATS(seasonal_periods=[14, 30.5])
    return model

def seasonal_decompose_model(train):
    """ Performns a seasonal decomposition of the training dataset passed as argument program
    Args:
        df_training (dataframe): The training dataset used to train the model

    Returns:
        bool: The return value. True for success, False otherwise
    """
    sm.tsa.seasonal_decompose(train.Count).plot()
    result = sm.tsa.stattools.adfuller(train.Count)
    plt.show()
    return True

def autocorrelation_function(train):
    """ Plots the autocorrelation from the training dataset
    Args:
        df_training (dataframe): The training dataset used to train the model

    Returns:
        bool: The return value. True for success, False otherwise
    """
    #https://www.vinta.com.br/blog/2018/understanding-time-series-forecasting-python/
    data = np.log(train)
    lac_acf = acf(data, nlags=40)

    plt.figure(figsize=(15, 5))
    plt.subplot(121)
    plt.stem(lac_acf)
    plt.axhline(y=0, linestyle='-', color='black')
    plt.xlabel('Lag')
    plt.ylabel('ACF')
    plt.show()
    return True

