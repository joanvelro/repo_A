import matplotlib.pyplot as plt
from numpy import log
from statsmodels.tsa.stattools import adfuller
from statsmodels.tsa.stattools import kpss
from statsmodels.graphics.tsaplots import plot_acf
from statsmodels.graphics.tsaplots import plot_pacf
from statsmodels.tsa.seasonal import seasonal_decompose
import pandas as pd
from scipy import stats


def is_stationary_with_adf(data, significance_level=0.01):
    """Decide if the given time series is stationary using ADF test."""
    X = data.values
    result_test = adfuller(X, regression='c', autolag='BIC')
    p_value = result_test[1]
    adf_stats = result_test[0]
    print("ADF p-value: {:0.5f}".format(p_value))
    print("ADF Statistic: {:0.5f}".format(adf_stats))
    print('Critical Values:')
    for key, value in result_test[4].items():
        print('\t%s: %.3f' % (key, value))
    return p_value < significance_level


def is_stationary_with_kpss(data, significance_level=0.05):
    """Decide if the given time series is stationary using KPSS test."""
    result_test = kpss(data, regression='c')
    p_value = result_test[1]
    kpss_stats = result_test[0]
    print("KPSS p-value: {:0.5f}".format(p_value))
    print("KPSS Statistic: {:0.5f}".format(kpss_stats))
    print('Critical Values:')
    for key, value in result_test[3].items():
        print('\t%s: %.3f' % (key, value))
    return p_value > significance_level


def is_stationary_visual_check(data, value, seasons, save_to=None):
    seasonal_mean = data.rolling(window=seasons).mean()
    seasonal_std = data.rolling(window=seasons).std()

    fig, ax = plt.subplots(figsize=(18, 6))
    data.plot(x="FECHA", y=value, ax=ax, label="observed", c='lightgrey')
    seasonal_mean.plot(x="FECHA", y=value, ax=ax, label="seasonal (rolling) mean", c='red')
    seasonal_std.plot(x="FECHA", y=value, ax=ax, label="seasonal (rolling) std", c='blue')

    plt.legend(loc='best')
    plt.title('Seasonal Mean & Standard Deviation')

    if save_to is not None:
        plt.savefig(save_to)
    plt.show()

def correct_non_stationarities(data, seasons):
    data['boxcox'], lmbda = stats.boxcox(data['TOTAL_VEHICULOS'])
    data["fd"] = data.boxcox - data.boxcox.shift(1)
    data["stationary"] = data.fd - data.fd.shift(seasons)
    print("Stationary by ADF: {}".format(
        "yes" if is_stationary_with_adf(data.stationary.dropna()) else "no"), '\n')

    print("Stationary by KPSS: {}".format(
        "yes" if is_stationary_with_kpss(data.stationary.dropna()) else "no"))

    is_stationary_visual_check(data['stationary'], value="TOTAL_VEHICULOS", seasons=96)

    fig, ax = plt.subplots(figsize=(18, 6))

    ax = fig.add_subplot(211)
    fig = plot_acf(data.stationary.dropna(inplace=False).iloc[seasons + 96:], lags=96, ax=ax)

    ax = fig.add_subplot(212)
    fig = plot_pacf(data.stationary.dropna(inplace=False).iloc[seasons + 1:], lags=96, ax=ax)
    plt.show()

    return data

def decomposition_time_series(data):
    # Time series decomposition
    decompfreq = 96  #
    model = 'additive'

    decomposition = seasonal_decompose(
        data.interpolate("linear"),
        freq=decompfreq,
        model=model)

    trend = decomposition.trend
    seasonal = decomposition.seasonal
    residual = decomposition.resid


    fig, ax = plt.subplots(figsize=(18, 6))
    data.plot(x="FECHA", y="TOTAL_VEHICULOS", ax=ax, label="observed", c='lightgrey')
    trend.plot(ax=ax, label="trend")
    plt.legend(loc='upper left')

    fig, ax = plt.subplots(figsize=(18, 4))
    seasonal.plot(ax=ax, label="seasonality")
    plt.legend(loc='bottom left')

    fig, ax = plt.subplots(figsize=(18, 4))
    residual.plot(ax=ax, legend="residual")
    plt.legend(loc='upper left')
    plt.show()

def day_plot(data):
    day_zoom_range = data[(data['FECHA'] >= '2019-01-24 00:00:00') & (data['FECHA'] < '2019-01-25 00:00:00')].index
    fig, ax = plt.subplots(figsize=(18, 6))
    data_zoomed = data.loc[day_zoom_range]
    print("Data zoomed rows: " + str(data_zoomed.shape[0]))
    data_zoomed.plot(x="FECHA", y="TOTAL_VEHICULOS", ax=ax, label="24-25 in zoom")
    plt.legend(loc='upper left')
    plt.show()

def plot_time_series(data):
    fig, axs = plt.subplots(3, 1, constrained_layout=True)

    axs[0].plot(data)
    axs[0].set_title('subplot 1 - time series plot')
    axs[0].set_xlabel('date (15min)')
    axs[0].set_ylabel('total vehicles')

    axs[1].hist(data)
    axs[1].set_title('subplot 2 - histogram')
    axs[1].set_xlabel('date (15min)')
    axs[1].set_ylabel('total vehicles')

    trans_data = log(data)
    axs[2].plot(trans_data)
    axs[2].set_title('subplot 3 - log transform time series plot')
    axs[2].set_xlabel('date (15min)')
    axs[2].set_ylabel('total vehicles')


    plot_acf(data, lags=50)
    plot_pacf(data, lags=50)

    plt.show()