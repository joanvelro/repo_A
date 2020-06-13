from sklearn.metrics import mean_absolute_error
from sklearn.metrics import mean_squared_error
from sklearn.metrics import r2_score
from sklearn.metrics import roc_auc_score
from math import sqrt

def mean_forecast_error_score(forecast, expected):
    forecast_errors = [expected[i] - forecast[i] for i in range(len(expected))]
    bias = sum(forecast_errors) * 1.0 / len(expected)
    print('Bias: %f' % bias)
    return bias

def mean_absolute_error_score(forecast, expected):
    mae = mean_absolute_error(expected, forecast)
    print('MAE: %f' % mae)
    return mae


def mean_squared_error_score(forecast, expected):
    mse = mean_squared_error(expected, forecast)
    print('MSE: %f' % mse)
    return mse

def root_mean_squared_error_score(forecast, expected):
    if forecast.size==1:
        mse=abs(expected-forecast)
    else:
        mse = mean_squared_error(expected, forecast)
    rmse = sqrt(mse)
    print('RMSE: %f' % rmse)
    return rmse

def r2_metric_score(forecast, expected):
    r2 = r2_score(expected, forecast)
    print('R2: %f' % r2)
    return r2

def roc_metric_score(forecast, expected):
    roc = roc_auc_score(expected, forecast)
    print('ROC: %f' % roc)
    return roc

def ssr_metric_score(forecast, expected):
    ssr = ((expected - forecast) ** 2).sum()
    print('SSR: %f' % ssr)
    return ssr

def sst_metric_score(forecast, expected):
    sst = ((expected - expected.mean()) ** 2).sum()
    print('SSR: %f' % sst)
    return sst