import subprocess
import pandas as pd
import numpy as np

def minsait_model(program_path):
    """ Load and execute the minsait developed program to generate predictions

    Args:
        program_path (str): Path to the program
    Returns:
        bool: The return value. True for success, False otherwise.

    """
    cmd=program_path+'TT_NL_predictor.exe'
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, cwd=program_path)

    for stdout_line in iter(process.stdout.readline, ""):
        yield stdout_line
    process.stdout.close()
    return_code = process.wait()
    if return_code:
        print(process.stderr)
        raise subprocess.CalledProcessError(return_code, cmd)


def direct_forecast_strategy(models, test_data, id_segment, model_name):
    '''
    1. Direct Multi-step Forecast Strategy
    The direct method involves developing a separate model for each forecast time step

    Reference Documentation: https://machinelearningmastery.com/multi-step-time-series-forecasting/
    :param model:
    :param df_input:
    :param id_segment:
    :return:
    '''

    x_test = test_data.loc[:, test_data.columns.difference(['TIME', 'FECHA'])]
    if model_name == 'extra_trees_2a':
        x_test = x_test[['MES', 'HORA', 'MINUTO','ID_SEGMENT','COD_LABORALIDAD','TOTAL_VEHICULOS','TOTAL_VEHICULOS_1', 'TOTAL_VEHICULOS_2', 'TOTAL_VEHICULOS_3', 'TOTAL_VEHICULOS_4', 'TOTAL_VEHICULOS_5', 'TOTAL_VEHICULOS_6', 'TOTAL_VEHICULOS_7']]

    y_preds = [0, 0, 0, 0, 0, 0, 0, 0, 0]


    for i_model in models:
        y_preds[int(int(i_model)/15)-1] = models[str(i_model)].predict(x_test)[0]

    return y_preds


def recursive_forecast_strategy(models, df_input, id_segment, model_name, target_step, current_date, current_dateCode, forecast_horizon):
    '''
    2. Recursive Multi-step Forecast
    The recursive strategy involves using a one-step model multiple times where the prediction
    for the prior time step is used as an input for making a prediction on the following time step

    Reference Documentation: https://machinelearningmastery.com/multi-step-time-series-forecasting/
    :param model:
    :param df_input:
    :param id_segment:
    :return:
    '''

    test_data = df_input.loc[df_input['ID_SEGMENT'] == id_segment]  # clustering function
    null_values = test_data.isnull().any().sum()

    start_temp_features = min(
        [i for i in range(0, len(test_data.columns)) if test_data.columns[i].startswith('TOTAL_VEHICULOS')])

    if null_values == test_data.iloc[:, start_temp_features:].shape[1]:
        y_preds = [-127,  -127,  -127,  -127,  -127,  -127,  -127,  -127,  -127] # Not possible to generate predictions
        return y_preds

    if null_values >= 0:

        null_cols = np.where(pd.isnull(test_data.iloc[:, start_temp_features:]))

        not_null_cols = [value_col for value_col in range(start_temp_features, test_data.shape[1]) if value_col not in start_temp_features + null_cols[1]]

        for col in null_cols[1]:
            test_data.iloc[:, start_temp_features+col].fillna(test_data.iloc[:, not_null_cols].values.mean(), inplace=True)


    x_test = test_data.loc[:, test_data.columns.difference(['TIME','FECHA'])]

    if model_name == 'extra_trees_2a':
        x_test = x_test[['MES', 'HORA', 'MINUTO','ID_SEGMENT','COD_LABORALIDAD','TOTAL_VEHICULOS','TOTAL_VEHICULOS_1', 'TOTAL_VEHICULOS_2', 'TOTAL_VEHICULOS_3', 'TOTAL_VEHICULOS_4', 'TOTAL_VEHICULOS_5', 'TOTAL_VEHICULOS_6', 'TOTAL_VEHICULOS_7']]

    i_model = 15
    y_pred_15 = models[str(i_model)].predict(x_test)

    forecast_window = int(120 / 15)  # 8 rows for 2h
    time_step = 0

    y_preds = [y_pred_15[0], 0, 0, 0, 0, 0, 0, 0, 0]
    target_col = x_test.columns.get_loc('TOTAL_VEHICULOS')

    while(time_step < forecast_window):

        x_test.iloc[:, target_col:].shift(1, axis=1)

        x_test['TOTAL_VEHICULOS'] = int(y_preds[time_step])

        time_step = time_step + 1

        # Execute predictions
        y_preds[time_step] = models[str(i_model)].predict(x_test)[0]

    return y_preds


def direct_recursive_forecast_strategy(model, df_input, id_segment):
    '''
    3. Direct-Recursive Hybrid Strategies
    A separate model can be constructed for each time step to be predicted, but each model may use
    the predictions made by models at prior time steps as input values.

    Reference Documentation: https://machinelearningmastery.com/multi-step-time-series-forecasting/
    :param model:
    :param df_input:
    :param id_segment:
    :return:
    '''

    return None


def multioutput_forecast_strategy(model, df_input, id_segment):
    '''
    4. Multiple Output Strategy
    The multiple output strategy involves developing one model that is capable of predicting the entire
    forecast sequence in a one-shot manner

    Reference Documentation: https://machinelearningmastery.com/multi-step-time-series-forecasting/
    :return:
    '''

    return