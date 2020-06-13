import os
import argparse
import yaml
import sys
import time
from datetime import datetime, timedelta
import logging
import pandas as pd
import numpy as np
from calendar import monthrange
from secrets import cipher_secrets_suite
from algorithms.utilities import model_serialization
import holidays
from algorithms import ml_algorithms, statistical_algorithms

from dataset_utils.data_profiling import arima_parameter_tunning
from dataset_utils import data_load, data_transformation

from evaluation import realtime_monitoring, evaluation_metrics


datasets_groups = {'input': "input"}
algorithms_ids = {'minsait':1, 'tbats': 2, 'ets': 3, 'arima': 4, 'fbprophet': 5, 'desc_ref': 6, 'extra_trees_2': 8, 'extra_trees_2a': 9, 'random_forest': 10, 'mlp_neural_network': 11, 'gradient_boosting':12, 'kn_neighbors':14}

last_update_time = None
current_date = None
df_input = None

project_scope = 'AUSOL' # NORTE

def setup_logger(logging_level, save_dir='..'):
    logging_levels = {'INFO':logging.INFO, 'DEBUG': logging.DEBUG, 'ERROR': logging.ERROR}
    formatter = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    LOGGER_FOLDER = "log_files"
    log_dir = os.path.join(os.path.abspath(save_dir), LOGGER_FOLDER)
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)

    log_file = datetime.now().strftime('dataanalytics.predictive_%H_%M_%d_%m_%Y.log')

    logging.basicConfig(filename=os.path.join(log_dir, log_file), format=formatter, level=logging_levels[logging_level])
    logging.getLogger().addHandler(logging.StreamHandler())

    return logging

def parse_cmdline_args():
    """ Parse command line argument program
    Returns:
        str: The parsed argument program

    """
    parser = argparse.ArgumentParser(description='''predictive_analytics is a Python package to perform predictive analytics''')
    parser.add_argument("-c","--configuration",
                        help='configuration file with the options to be executed',
                        default="configuration.yaml")
    parser.add_argument("-l","--logger",
                        help='logging level for debuging purposes',
                        default="INFO")
    return parser


def check_date_consistency(cfg):
    """ This function check if the date specified the configuration file are correct

    Args:
        cfg (dict): The configuration parameters of this script

    Returns:
        bool: The return value. True for success, False otherwise.

    """
    delta = datetime.strptime(cfg['generate_datasets']['end_date'], '%Y-%m-%d %H:%M:%S') - datetime.strptime(cfg['generate_datasets']['init_date'], '%Y-%m-%d %H:%M:%S')
    print(delta)

    if delta.days <1:
        logging.error("The selected dates (configuration file) are less than 1 day; a minimum of 1 day is required for training and testing")
        sys.exit(0)

    for algo_case in cfg['train_algorithms']:
        if algo_case['activated'] == True & (delta.days*24 + delta.seconds//3600) < algo_case['hours_in_train']:
            logging.error("The selected dates (configuration file) are less than the required training hours for the algorithm: " + algo_case['algorithm_name'])
            sys.exit(0)

    return True


def get_config_params(config_file):
    """ It stores the configuration parameters
    Args:
        args(obj): Argument program
    Returns:
        cfg(dict): A Python's dict with the configuration parameters
    """

    # parse the configuration.yaml configuration file
    with open(config_file, 'r') as ymlfile:
        cfg = yaml.load(ymlfile)

    if cfg['global']['execution_mode']=='realtime' or cfg['global']['execution_mode']=='offline':

        input_data = pd.read_csv(cfg['global']['input_data'], delimiter=";",parse_dates=['FECHA'])

        cfg['generate_datasets']['end_date'] = datetime.strptime(str(input_data['FECHA'][0]), '%Y-%m-%d %H:%M:%S')

        train_hours = [algo_case['hours_in_train'] for algo_case in cfg['train_algorithms'] if algo_case['activated'] == True ]  # filter by test cases to be executed
        if len(train_hours)==0:
            train_hours=[0]
        cfg['generate_datasets']['init_date'] = cfg['generate_datasets']['end_date'] - timedelta(hours=max(train_hours))
        cfg['current_code'] = input_data['COD_LABORALIDAD']
    elif cfg['global']['execution_mode']!='offline':
         check_date_consistency(cfg)

    cfg['range_date'] = (str(cfg['generate_datasets']['init_date']), str(cfg['generate_datasets']['end_date']))


    # get passwords for connecting to db
    # check if master_key is set
    if cfg['pipeline_execution']['generate_datasets']:
        if os.path.exists(os.path.join(cfg['global']['fernet_tokens_path'],'fernet_tokens.txt')):
            cfg = cipher_secrets_suite.decrypt_secrets(cfg)
        else:
            cfg = cipher_secrets_suite.generate_secrets_file(cfg)  # generate secrets file

    global datasets_groups
    datasets_groups['input'] = cfg['global']['input_data'].split('\\')[-1].split('.csv')[0]
    datasets_groups['results'] = cfg['global']['output_path'].split('\\')[-1].split('.csv')[0]

    return cfg


def train_model(algorithm_name, df_training, isupdate, trained_models, segment_id):
    """ Train a data model based on specified algorithm and the training dataset
    Args:
        algorithm_name (str): The name of the algorithm to be save
        df_training (DataFrame): The training dataset used for model generation

    Returns:
        fitted_model (object): The trained model object
    """

    fitted_model = None
    if algorithm_name == 'arima':
        if isupdate:
           model = statistical_algorithms.arima_model(df_training['TOTAL_VEHICULOS'].values)
           fitted_model = model.filter(trained_models[str(algorithm_name)]['model'].params)
        else:
            training_mod = statistical_algorithms.arima_model(df_training['TOTAL_VEHICULOS'].values)
            fitted_model = training_mod.fit()

    elif algorithm_name == 'minsait':
        logging.info("Not implemented")

    elif algorithm_name == 'desc_ref':
         model_path = os.path.dirname(os.path.abspath(__file__)).split("src")[:-1]  # get top level dir
         model_path = os.path.join(model_path[0], 'models', cfg['global']['project_name'])
         fitted_model = pd.read_csv(os.path.join(model_path, algorithm_name, "anual_data_2018.csv"),delimiter=";", parse_dates=['FECHA']) # TODO:MAKE PREDICTIONS

    elif algorithm_name == 'ets':
        if isupdate:
            fitted_model = statistical_algorithms.holtwinters_exp_smoothing_model(df_training['TOTAL_VEHICULOS'])
        else:
            fitted_model = statistical_algorithms.holtwinters_exp_smoothing_model(df_training['TOTAL_VEHICULOS'])

    elif algorithm_name == 'snaive':
        logging.info("The snaive model has been trained succesfully")

    elif algorithm_name == 'tbats':
        if isupdate:
            model = statistical_algorithms.tbats_model()
            fitted_model = model.fit(df_training.values)
        else:
            model = statistical_algorithms.tbats_model()
            fitted_model = model.fit(df_training['TOTAL_VEHICULOS'].values)

    elif algorithm_name == 'fbprophet':

        fitted_model = statistical_algorithms.prophet_model(df_training)

    else:
        logging.warn("this algorithm is not implemented yet")

    logging.debug("The " + algorithm_name + " model for the segment ID " + str(segment_id) + " has been trained succesfully")

    return fitted_model


def get_evaluation_metrics(pred_values, expected_values, interval_errors):
    rmse_results = dict()
    r2_results = dict()

    #evaluation_metrics.mean_forecast_error_score(pred_values, expected_values)
    #evaluation_metrics.mean_absolute_error_score(pred_values, expected_values)
    #evaluation_metrics.mean_squared_error_score(pred_values, expected_values)

    nrows = 1
    while nrows*15  <= (cfg['model_training']['hours_to_forecast'] * 60):  # 2 hours * 60 -> 120 min / 15 = nrows
        #nrows_key = nrows if nrows - 1 == 0 else nrows - 1
        if interval_errors:
            rmse_results[str(nrows * 15)] = evaluation_metrics.root_mean_squared_error_score(pred_values[0:nrows], expected_values[0:nrows])
            r2_results[str(nrows * 15)] = 0
            #r2_results[str(nrows_key * 15)] = evaluation_metrics.r2_metric_score(pred_values[0:nrows], expected_values[0:nrows])
        else:
            rmse_results[str(nrows * 15)] = evaluation_metrics.root_mean_squared_error_score(pred_values[nrows-1],expected_values[nrows-1])
            r2_results[str(nrows * 15)] =0
            #r2_results[str(nrows_key * 15)] = evaluation_metrics.r2_metric_score(pred_values[nrows-1],expected_values[nrows-1])

        if nrows==1:
            nrows = nrows + 3
        else:
            nrows = nrows + 4


    return rmse_results, r2_results


def step_0_load_program_conf(args):
    cfg = get_config_params(args.configuration)
    return cfg


def restore_datasets(datasets, datasets_groups):
    """ This method retrieve datasets from disks
    Args:
        datasets (dict): A Python's dict storing the datasets as dataframes
        datasets_groups(list): A list with the types of datasets to be generated
    Returns:
        datasets (dict): A Python's dict storing the datasets as dataframes
    """

    for key, file_name in datasets_groups.items():
        logging.debug("----> RESTORING DATASET: " + str(key))

        try:
            datasets[str(key)] = pd.read_csv(cfg['generate_datasets']['datasets_path'] + str(file_name) + ".csv", delimiter=";", parse_dates=['FECHA'])
        except FileNotFoundError as e:
            logging.error(str(e))
            logging.error("The dataset needs to be re-generated.")

            datasets = data_load.generate_datasets(datasets, datasets_groups, time_range=cfg['range_date'], datasets_path=cfg['generate_datasets']['datasets_path'], sqlserver_password=cfg['global']['sqlserver_password'], save_to_disk=True)
            return datasets

        if str(datasets['input']['FECHA'][0]) != cfg['range_date'][1]:
            logging.error("The current dataset is outdated, It needs to be re-generated ")
            datasets = data_load.generate_datasets(datasets, datasets_groups, time_range=cfg['range_date'],
                                                   datasets_path=cfg['generate_datasets']['datasets_path'],
                                                   sqlserver_password=cfg['sqlserver_password'], save_to_disk=True)

        print(datasets[str(key)].info())
        datasets[str(key)].index = pd.to_datetime(datasets[str(key)]['FECHA'])

    return datasets


def step_1_load_datasets(cfg):
    """ This method executes (if desired) the load datasets step of the pipeline
    Args:
        cfg (dict): The configuration parameters of this script

    Returns:
        datasets (dict): A Python's dict storing the datasets as dataframes
    """
    datasets = {}


    if cfg['pipeline_execution']['generate_datasets']:
        if cfg['generate_datasets']['split_download'] == 'monthly':
            months_frames = []

            init_year = cfg['generate_datasets']['init_date'].strftime("%Y")
            end_year = cfg['generate_datasets']['end_date'].strftime("%Y")

            init_month = cfg['generate_datasets']['init_date'].strftime("%m")
            end_month = cfg['generate_datasets']['end_date'].strftime("%m")

            init_timeday = str(cfg['generate_datasets']['init_date']).split("-")[-1]
            end_timeday = str(cfg['generate_datasets']['init_date']).split("-")[-1]

            n_years = int(end_year) - int(init_year)

            n_months = abs(int(end_month) - int(init_month)) + n_years * 12

            year = int(init_year)

            for i_year in range(1, n_years+2):

                for month_i in range(1, n_months+2):
                    start_date = "{:04d}".format(year) + '-' +  "{:02d}".format(month_i) + '-' +  str(init_timeday)
                    total_days_month = monthrange(int(init_year), month_i)[1]
                    end_date = "{:04d}".format(year) + '-' +  "{:02d}".format(month_i) + '-' + str(total_days_month) +  ' 00:00:00'

                    range_date=(start_date, end_date)
                    monthly_data = data_load.generate_datasets(datasets, datasets_groups, time_range=range_date, datasets_path=cfg['generate_datasets']['datasets_path'], sqlserver_password=cfg['sqlserver_password'], save_to_disk=False)
                    months_frames.append(monthly_data['input'])
                    monthly_data['input'].to_csv(str(month_i) + '_' + year + '.csv', sep=';', index=False)

                    print("Succesfully downloaded dataset for: " + str(range_date))

                year = year + i_year

                datasets['input'] = pd.concat(months_frames)

                datasets['input'].to_csv('anual_data_' + str(i_year) + '.csv', sep=';', index=False)

        else: # download the data for the range_date specified in the configuration file
            datasets = data_load.generate_datasets(datasets, datasets_groups, time_range=cfg['range_date'],datasets_path=cfg['generate_datasets']['datasets_path'], sqlserver_password=cfg['sqlserver_password'],save_to_disk=True)


    elif cfg['pipeline_execution']['restore_datasets']:# retrieve datasets from disks
        datasets = restore_datasets(datasets, datasets_groups)

    else:
        datasets['input']=None

    return datasets

def step_data_profiling(datasets):
    arima_parameter_tunning.plot_time_series(datasets['input']['TOTAL_VEHICULOS'])
    arima_parameter_tunning.decomposition_time_series(datasets['input']['TOTAL_VEHICULOS'])
    arima_parameter_tunning.is_stationary_visual_check(datasets['input']['TOTAL_VEHICULOS'], "TOTAL_VEHICULOS",
                                                       seasons=96)
    arima_parameter_tunning.correct_non_stationarities(datasets['input'], seasons=96)
    arima_parameter_tunning.day_plot(datasets['input'].loc[datasets['input']['ID_SEGMENT'] == int(cfg['global']['segment_id'])][['TOTAL_VEHICULOS', 'FECHA']])



def step_2_model_training(cfg, selected_segments, df_training, isupdate, trained_models):
    """ This method executes (if desired) the model training step of the pipeline
    Args:
        cfg (dict): The configuration parameters of this script
        selected_segments(list): The list of segments that has been selected in the configuration file in at least one model
        datasets (dict): A Python's dict storing the datasets as dataframes

    Returns:
        trained_models (dict): A Python's dict with the trained models
    """
    if not isupdate:
        trained_models = dict()
    df_test=None

    if cfg['pipeline_execution']['train_algorithms'] or (isupdate & cfg['conitnuous_learning']['train_update']):  # train the models with the new data

        activated_train_algorithms = [algo_case for algo_case in cfg['train_algorithms'] if
                                      algo_case['activated'] == True]  # filter by test cases to be executed

        for segment_id in selected_segments:  # filter data by segment

            trained_models[str(segment_id)] = dict()

            for algo_train_active in activated_train_algorithms:  # loop the list of the activated algorithms

                if segment_id in algo_train_active['by_segment']:

                    if cfg['global']['execution_mode']!='realtime':
                        init_train_date, end_train_date, init_forecast_date, end_forecast_date = data_transformation.generate_sliding_windows(cfg, algo_train_active)

                    else:
                        end_train_date=cfg['range_date'][1]


                    model_name = str(segment_id) + "_" + algo_train_active['algorithm_name'] + "__" + str(end_train_date).replace(" ", "_").replace(':','.')


                    df_training_by_seg = df_training.loc[df_training['ID_SEGMENT'] == segment_id]
                    df_training, df_test = data_transformation.split_train_test_bydate(df_training_by_seg, init_train_date,
                                                                                       end_train_date,
                                                                                       init_forecast_date, end_forecast_date,
                                                                                       datetime.strptime(end_forecast_date,
                                                                                                         '%Y-%m-%d %H:%M:%S'))

                    trained_mod = train_model(algo_train_active['algorithm_name'], df_training_by_seg, isupdate, trained_models, segment_id)

                    model_serialization.save_model(model_name, algo_train_active['code'], trained_mod, cfg)  # finally save the model to disk

                    # Store the model in the Python's results dict
                    if str(algo_train_active['algorithm_name']) not in trained_models[str(segment_id)].keys():
                        trained_models[str(segment_id)][str(algo_train_active['algorithm_name'])] = []
                    trained_models[str(segment_id)][str(algo_train_active['algorithm_name'])].append(
                        {'model_name': model_name + '-' + algo_train_active['code'], 'model': trained_mod, 'validation_test_set': df_test})

    else: # retrieve the models from disk to make predictions
        activated_pred_algorithms = [algo_case for algo_case in cfg['make_predictions'] if
                                     algo_case['activated'] == True]  #

        segs_algorithms = [algo_case for algo_case in cfg['make_predictions'] if
                                     algo_case['activated'] == True]  #

        for segment_id in selected_segments:
            trained_models[str(segment_id)] = dict()
            for algo_pred_active in activated_pred_algorithms:
                segment_list = [algo_case['by_segment'] for algo_case in cfg['make_predictions'] if
                                   algo_case['algorithm_name'] == str(algo_pred_active['algorithm_name'])]  #

                if segment_id in segment_list[0]:
                    end_train_date = cfg['range_date'][1]
                    model_name = str(segment_id) + "__" + algo_pred_active['algorithm_name'] + "__" + str(end_train_date).replace(" ", "_").replace(':', '.')
                    trained_mod = model_serialization.load_model(model_name, algo_pred_active['code'], segment_id, cfg, True, True)

                    # Store the model in the Python's results dict
                    if str(algo_pred_active['algorithm_name']) not in trained_models[str(segment_id)].keys():  # create a new entry
                       trained_models[str(segment_id)][str(algo_pred_active['algorithm_name'])] = []
                    trained_models[str(segment_id)][str(algo_pred_active['algorithm_name'])].append({'model_name':model_name, 'model':trained_mod, 'validation_test_set':df_test})

    #logging.debug("The following models has been succesfully loaded: " + str(trained_models))

    return trained_models


def get_closest_model_intime(train_model, algo_name, current_model_date):
    """ Retrieve the closest model from the generated models
    Args:
        trained_models (dict): A Python's dict with the trained models for a specific segment
        algo_name (str): The name of the algorithm
        current_model_date (date): The start forecasting date

    Returns:
        trained_model (obj): A Python's obj with the trained model
    """

    train_model.sort(key=lambda x: datetime.strptime(x['model_name'].split('__')[-1].replace('_',' ').replace('.', ':'), '%Y-%m-%d %H:%M:%S'))

    i = 0
    closest_model = train_model[i]
    while((closest_model['model_name'].split('_')[-1] < current_model_date) & i<len(train_model)):
        closest_model = train_model[i]
        i=i+1

    return closest_model


def make_predictions(trained_models, selected_segments, forecast_horizon, time_counter, cfg, current_date, df_input):
    """ This method executes the corresponding model to make the forecast
    Args:
        forecast_times (list): The desired forecast times in minutes
        selected_segments(list): The list of segments that has been selected in the configuration file in at least one model
        datasets (dict): A Python's dict storing the datasets as dataframes

    Returns:
        predictions (dict): A Python's dict with the model predictions (forecast horizon) for the selected segments
    """
    minsait_first=True
    predictions=dict()
    accidents = dict()

    for segment_id in selected_segments:  # filter data by segment
        if (len(df_input.loc[(df_input.ID_SEGMENT == segment_id) & (df_input.FECHA == current_date)]['TOTAL_VEHICULOS'].index)==0):
            current_total_veh = 9999999999999999999 # this value does not exist
        else:
            current_total_veh = df_input.loc[(df_input.ID_SEGMENT == segment_id) & (df_input.FECHA == current_date)]['TOTAL_VEHICULOS'].iloc[0]

        if(len(df_input.loc[(df_input.ID_SEGMENT == segment_id) & (df_input.FECHA == current_date)]['COD_LABORALIDAD'].index)==0):
            logging.warn("No data for the date " +  str(current_date) + " and segment: " + str(segment_id))
            break
        else:
            current_dateCode = df_input.loc[(df_input.ID_SEGMENT == segment_id) & (df_input.FECHA == current_date)]['COD_LABORALIDAD'].iloc[0]

        predictions[str(segment_id)] = dict()
        accidents[str(segment_id)] = dict()

        for model_name, trained_model_list in trained_models[str(segment_id)].items():
            model = get_closest_model_intime(trained_model_list, model_name, cfg['range_date'][0])
            predictions[str(segment_id)][model_name] =[]
            accidents[str(segment_id)][model_name] = []

            if model_name == 'arima':
                logging.debug("STEP_3-1 ARIMA FORECASTING HAS STARTED")
                new_predictions = statistical_algorithms.stats_model(model['model'], current_date, segment_id, current_dateCode, None,
                                  None, forecast_horizon, model_name)
                predictions[str(segment_id)][model_name].extend(new_predictions)

            elif model_name == 'minsait':
                if minsait_first==True: # generate predictions
                    logging.debug("STEP_3-1 MINSAIT FORECASTING HAS STARTED")
                    program_path = cfg['global']['minsait_program_path']
                    for program_result in ml_algorithms.minsait_model(program_path):
                        print(str(program_result))#, end="")

                    minsait_first=False

                # take predictions by segment
                df_minsait = pd.read_csv(os.path.join(program_path, "TT_NL_Predicciones.csv"),
                                         delimiter=";", parse_dates=['FECHA'])
                preds_taken=False

                while(not preds_taken): # wait until data is written
                    if len(df_minsait['PREDICCION_INT 120'].index)==0 :
                        df_minsait = pd.read_csv(os.path.join(program_path, "TT_NL_Predicciones.csv"),
                                                 delimiter=";", parse_dates=['FECHA'])
                    else: # take predictions
                        df_minsait_seg = df_minsait.loc[(df_minsait.ID_SEGMENT==segment_id)&(df_minsait.FECHA==current_date)]
                        new_predictions = [df_minsait_seg['PREDICCION_INT 15'].item(), 0, 0, df_minsait_seg['PREDICCION_INT 60'].item(), 0, 0, 0, df_minsait_seg['PREDICCION_INT 120'].item()]
                        new_accidents = [df_minsait_seg['IND_ACCIDENTE_15'].item(), 0, 0, df_minsait_seg['IND_ACCIDENTE_60'].item(), 0, 0, 0, df_minsait_seg['IND_ACCIDENTE_120'].item()]
                        predictions[str(segment_id)][model_name].extend(new_predictions)
                        accidents[str(segment_id)][model_name].extend(new_accidents)
                        preds_taken=True

            elif model_name == 'desc_ref':
                logging.debug("STEP_3-1 DESC REF FORECASTING HAS STARTED")
                preds = statistical_algorithms.descriptive_model(model['model'], current_date, segment_id, current_dateCode, None,
                                  None, forecast_horizon)
                if len(preds)<8:
                    new_predictions = [current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh]
                else:
                    new_predictions = [preds[0], 0, 0, preds[3], 0, 0, 0, preds[7]]
                predictions[str(segment_id)][model_name].extend(new_predictions)

            elif model_name == 'ets':
                logging.debug("STEP_3-1 ETS FORECASTING HAS STARTED")
                new_predictions = statistical_algorithms.stats_model(model['model'], current_date, segment_id, current_dateCode, None,
                                  None, forecast_horizon, model_name)
                predictions[str(segment_id)][model_name].extend(new_predictions)

            elif model_name == 'tbats':
                logging.debug("STEP_3-1 TBATS FORECASTING HAS STARTED")
                new_predictions = statistical_algorithms.stats_model(model['model'], current_date, segment_id, current_dateCode, None,
                                  None, forecast_horizon, model_name)
                predictions[str(segment_id)][model_name].extend(new_predictions)
                # print(fitted_model.summary())

            elif model_name == 'fbprophet':
                logging.debug("STEP_3-1 FBPROPHET FORECASTING HAS STARTED")
                new_predictions = statistical_algorithms.stats_model(model['model'], current_date, segment_id, current_dateCode, None,
                                  None, forecast_horizon, model_name)
                predictions[str(segment_id)][model_name].extend(new_predictions)

            elif model_name == 'extra_trees_1' or model_name == 'extra_trees_2' or model_name == 'extra_trees_2a' or model_name=='gradient_boosting' or model_name == 'random_forest' or model_name == 'mlp_neural_network' or model_name == 'kn_neighbors' or model_name=='bagging_regressor':
                logging.debug("STEP_3-1 ML MODELS FORECASTING HAS STARTED")
                df_input_seg = df_input.loc[(df_input.ID_SEGMENT == segment_id) & (df_input.FECHA == current_date)]

                new_predictions = ml_algorithms.recursive_forecast_strategy(model['model'], df_input_seg, segment_id, model_name, 1, current_date, current_dateCode, forecast_horizon)

                predictions[str(segment_id)][model_name].extend(new_predictions)

            else:
                logging.error("The algorithm has not implemented yet")

    return predictions, accidents



def step_4_evaluation_metrics(cfg, df_predictions, df_expected, finish_date):
    for segment_id in selected_segments:  # filter data by segment
        activated_pred_algorithms = [algo_case for algo_case in cfg['make_predictions'] if
                                algo_case['activated'] == True & segment_id in algo_case[
                                    'by_segment']]  # filter by test cases to be executed

        predictions = dict()
        predictions['FORECAST_DATE'] = []
        forecast_date_range_init = datetime.strptime(cfg['range_date'][1], '%Y-%m-%d %H:%M:%S')

        for algo_pred_active in activated_pred_algorithms:  # loop the list of the activated algorithms

            compute_interval_errors = True
            if cfg['execute_pipeline']['evaluation_metrics']:
                df_evaluation_log = pd.DataFrame(
                    columns=['Highway_Name', 'Segment_ID', 'Dates', 'Model_Name', 'RMSE_15min', 'R2_15min', 'RMSE_1h', 'R2_1h',
                             'RMSE_2h', 'R2_2h'])
                highway_name = str(cfg['global']['ctx_name'])
                segment_id = str(cfg['global']['segment_id'])
                dates = str(cfg['range_date'][0]) + "__" + str(forecast_date_range_init)

                print("STEP_4 EVALUATION METRICS - " + str(algo_pred_active))

                model_name = str(algo_pred_active)

                rmse_results, r2_results = get_evaluation_metrics(
                    df_predictions['res_pred_' + str(algo_pred_active['algorithm_name'])].values,
                    df_expected['TOTAL_VEHICULOS'].values, interval_errors=compute_interval_errors)

                df_evaluation_log = df_evaluation_log.append(
                    {'Highway_Name': highway_name, 'Segment_ID': segment_id, 'Dates': dates, 'Model_Name': model_name,
                     'RMSE_15min': rmse_results['15'], 'R2_15min': r2_results['15'], 'RMSE_1h': rmse_results['60'],
                     'R2_1h': r2_results['60'], 'RMSE_2h': rmse_results['120'], 'R2_2h': r2_results['120']}, ignore_index=True)

                print("STEP_4 EVALUATION METRICS - minsait model")

                if compute_interval_errors:
                    minsait_pred = datasets['results'].loc[(datasets['results']['FECHA'] >= forecast_date_range_init) & (
                                datasets['results']['FECHA'] <= finish_date)]['INTENSIDAD_PRED_15MIN'].values
                else:
                    minsait_pred = np.zeros(int(120 / 15))
                    minsait_pred_aux = datasets['results'].loc[(datasets['results']['FECHA'] == forecast_date_range_init)][
                        ['INTENSIDAD_PRED_15MIN', 'INTENSIDAD_PRED_1H', 'INTENSIDAD_PRED_2H']].values

                    minsait_pred[0] = minsait_pred_aux[0][0]
                    minsait_pred[int((60 / 15) - 1)] = minsait_pred_aux[0][1]
                    minsait_pred[int((120 / 15) - 1)] = minsait_pred_aux[0][2]

                rmse_results, r2_results = get_evaluation_metrics(minsait_pred, df_expected['TOTAL_VEHICULOS'].values,
                                                                  interval_errors=compute_interval_errors)
                df_evaluation_log = df_evaluation_log.append(
                    {'Highway_Name': highway_name, 'Segment_ID': segment_id, 'Dates': dates, 'Model_Name': 'minsait',
                     'RMSE_15min': rmse_results['15'], 'R2_15min': r2_results['15'], 'RMSE_1h': rmse_results['60'],
                     'R2_1h': r2_results['60'], 'RMSE_2h': rmse_results['120'], 'R2_2h': r2_results['120']}, ignore_index=True)

                df_results = datasets['results'].loc[
                    (datasets['results']['ID_SEGMENT'] == int(cfg['global']['segment_id'])) & (
                            datasets['results']['FECHA'] >= forecast_date_range_init) & (
                            datasets['results']['FECHA'] <= finish_date)]

                print("STEP_4 EVALUATION METRICS - historico")
                rmse_results, r2_results = get_evaluation_metrics(df_results['INTENSIDAD_HIST'].values,
                                                                  df_expected['TOTAL_VEHICULOS'].values,
                                                                  interval_errors=compute_interval_errors)

                df_evaluation_log = df_evaluation_log.append(
                    {'Highway_Name': highway_name, 'Segment_ID': segment_id, 'Dates': dates, 'Model_Name': 'historico',
                     'RMSE_15min': rmse_results['15'], 'R2_15min': r2_results['15'], 'RMSE_1h': rmse_results['60'],
                     'R2_1h': r2_results['60'], 'RMSE_2h': rmse_results['120'], 'R2_2h': r2_results['120']}, ignore_index=True)

                print("STEP_4 EVALUATION METRICS - descriptivo")

                rmse_results, r2_results = get_evaluation_metrics(df_results['INTENSIDAD_DESC'].values,
                                                                  df_expected['TOTAL_VEHICULOS'].values, interval_errors=False)

                df_evaluation_log = df_evaluation_log.append(
                    {'Highway_Name': highway_name, 'Segment_ID': segment_id, 'Dates': dates, 'Model_Name': 'descriptivo',
                     'RMSE_15min': rmse_results['15'], 'R2_15min': r2_results['15'], 'RMSE_1h': rmse_results['60'],
                     'R2_1h': r2_results['60'], 'RMSE_2h': rmse_results['120'], 'R2_2h': r2_results['120']}, ignore_index=True)

                path = cfg['global']['results_path']
                name = str(datetime.now())[:19].replace(' ', '-').replace(':', '.').lower() + '.csv'
                df_evaluation_log.to_csv(path + name, encoding='utf-8', index=False)

    return df_evaluation_log


def check_incoming_data():
    """ This method checks if there is more data to generate predictions
    Returns:
        bool: The return value. True for success, False otherwise
        new_df (DataFrame): A DataFrame (if available) with the current data for genereting the predictions
         current_date (datetime): The current date (start date for the forecasting) in the format '%Y-%m-%d %H:%M:%S'
    """
    global last_update_time, current_date, df_input

    input_file = cfg['global']['input_data']

    time.sleep(2)  # sleep 2 sec
    while (not (os.path.isfile(input_file) & os.access(input_file, os.R_OK)  )): # the file is being replaced. Wait until the file appears
        time.sleep(2) # sleep 2 sec
        logging.info("The file is being replaced. Waiting.... ")


    modTimesinceEpoc = os.path.getmtime(input_file)
    # Convert seconds since epoch to readable timestamp
    actual_update_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(modTimesinceEpoc))

    logging.debug("Last File Update Received Time : " + str(last_update_time))
    logging.debug("Last Predictions Made for Datetime File: " + str(current_date))

    if(last_update_time == actual_update_time): # No new data
        current_datetime = datetime.now()
        elapsed_time = abs(current_datetime - datetime.strptime(actual_update_time,'%Y-%m-%d %H:%M:%S'))
        logging.debug("Last File Update Received Time : " + str(actual_update_time))

        if elapsed_time.total_seconds()>=cfg['realtime_mode']['newdata_timeout']: # Timeout has been reached
            logging.info("TIMEOUT REACHED")
            logging.debug("The timeout " + str(cfg['realtime_mode']['newdata_timeout']) + " seconds has been reached. The program didn't receive an updated file since last file time: " + str(actual_update_time))
            logging.debug("Last File Update Received Time : " + str(actual_update_time))
            logging.debug("Current Datetime : " + str(current_datetime))
            sys.exit(0)

        return False, df_input, current_date

    else: # new data
        logging.info("NEW UPDATE HAS BEEN RECEIVED")

        while(not (os.path.isfile(input_file) & os.access(input_file, os.R_OK))):
            time.sleep(2)  # sleep 2 sec
            logging.debug("The file is being replaced. Waiting.... " )
        df_input = pd.read_csv(cfg['global']['input_data'], delimiter=";", parse_dates=['FECHA'])
        current_date = datetime.strptime(str(df_input['FECHA'][0]), '%Y-%m-%d %H:%M:%S')

        last_update_time = actual_update_time

        return True, df_input, current_date


def apply_predefined_policies(df_output, preds, model_name, seg_id, i, current_total_veh):
    """this function manually define some policies based on the observed behaviour of the algorithms"""

    policy0_best_at_15 = ['extra_tree_2', 'gradient_boosting', 'kn_neighbors']
    policy1_best_at_60 = ['mlp_neural_network', 'kn_neighbors']
    policy2_best_at_120 = ['extra_tree_2', 'mlp_neural_network', 'gradient_boosting', 'kn_neighbors']

    ### predicciones a 15
    if model_name in policy0_best_at_15:
        df_output.iloc[i]['PREDICCION_INT 15'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][0]], weights=[0.5, 0.5]))
    else:
        df_output.iloc[i]['PREDICCION_INT 15'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][0]], weights=[0.85, 0.15]))

    ### predicciones a 60
    if model_name in policy1_best_at_60:
        df_output.iloc[i]['PREDICCION_INT 60'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(60 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.3, 0.7, 0.2]))
    else:
        df_output.iloc[i]['PREDICCION_INT 60'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(60 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.25, 0.45, 0.3]))

    ### predicciones a 120
    if model_name in policy2_best_at_120:
        df_output.iloc[i]['PREDICCION_INT 120'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(120 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.35, 0.55, 0.1]))
    else:
        df_output.iloc[i]['PREDICCION_INT 120'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(120 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.45, 0.4, 0.15]))

    return df_output


acc_levels = {'0': [None, 0.008199976508829674], '1': [0.008199976508829674, 0.02151736814165683],
              '2': [0.02151736814165683, 0.03210525810536322], '3': [0.03210525810536322, 0.05619920142209989],
              '4': [0.05619920142209989, 0.09007174413502755], '5': [0.09007174413502755, None]}

cv_segments = {1: 0.023529411764705882, 2: 0.01568627450980392, 3: 0.06274509803921569,
               4: 0.16862745098039217, 5: 0.11764705882352941, 6: 0.06666666666666667,
               7: 0.0196078431372549, 8: 0.023529411764705882, 11: 0.054901960784313725,
               12: 0.023529411764705882, 13: 0.03137254901960784, 14: 0.1843137254901961,
               15: 0.0784313725490196, 16: 0.06666666666666667, 17: 0.03529411764705882,
               18: 0.027450980392156862}

cv_time = {'23_02': 0.5753424657534247, '03_07': 0.19726027397260273, '08_15': 0.9945205479452055,
           '16_22': 0.7608610567514676}

cv_lab = {'Working': 0.7016129032258065, 'No_Working': 0.6610169491525424}

cv_traf = {'0.8747': [0, 840], '0.1206': [840, 1320], '0.0047': [1320, 1840], '0': [1840, 2200],
           '0.0': [2200, 2400], '0.00': [2400]}


# entrada : ID_SEGMENT;FECHA;PREDICCION_INT 15;PREDICCION_INT 60;PREDICCION_INT 120

# Funciones auxiliares

def take_perc_lab(x):
    for key in cv_traf.keys():
        valor = cv_traf[key]
        if (len(valor) > 1):
            if ((x >= valor[0]) and (x < valor[1])):
                return float(key)
        else:
            if (x >= valor[0]):
                return float(key)


def percent_to_level(x):
    for level in acc_levels.keys():
        limits = acc_levels[level]
        if (limits[0] == None):
            if (x <= limits[1]):
                return int(level)
        elif (limits[1] == None):
            if (x > limits[0]):
                return int(level)
        else:
            if ((limits[0] < x) & (x <= limits[1])):
                return int(level)


def take_perc_time(x):
    for key in cv_time.keys():
        limits = key.split('_')
        if (limits[0] == '23'):
            if ((x >= int(limits[0])) | (x <= int(limits[1]))):
                return cv_time[key]
        else:
            if ((x >= int(limits[0])) & (x <= int(limits[1]))):
                return cv_time[key]


# Funcion de prediccion

def make_accident_predictions(df_one_line):

    segment = df_one_line['ID_SEGMENT'].iloc[0]

    #print('Segmento:', segment)

    if (segment in [9, 10, 19, 20]):
        return -127, -127, -127

    hol = holidays.PTE()  # Dias festivos portugal
    fecha_dt = pd.to_datetime(df_one_line.FECHA.iloc[0])
    wk = ''

    if (fecha_dt.weekday() > 4):
        wk = 'No_Working'
    else:
        wk = 'No_Working' if (fecha_dt in hol) else 'Working'

    #print('Lab:', wk)

    d15 = timedelta(hours=0, minutes=15)
    d60 = timedelta(hours=1, minutes=0)
    d120 = timedelta(hours=2, minutes=0)

    date_15 = fecha_dt + d15
    date_60 = fecha_dt + d60
    date_120 = fecha_dt + d120

    hour_15 = date_15.hour
    hour_60 = date_60.hour
    hour_120 = date_120.hour

    #print('Hora:', hour_15, hour_60, hour_120)

    # Tenemos nuestras tres variables: segment, wk, hour
    ### Lo siguiente es calcular los coeficientes

    # Segmento
    seg_coef = cv_segments[segment]

    # Hora
    time_coef_15 = take_perc_time(hour_15)
    time_coef_60 = take_perc_time(hour_60)
    time_coef_120 = take_perc_time(hour_120)

    # Laboralidad
    lab_coef = cv_lab[wk]

    ### Calculamos el nivel de tráfico, que es distinto para cada predicción

    # 15 minutos
    t_pred_15 = df_one_line['PREDICCION_INT 15'].iloc[0]
    tl_15_coef = take_perc_lab(t_pred_15)

    # 1 hora
    t_pred_60 = df_one_line['PREDICCION_INT 60'].iloc[0]
    tl_60_coef = take_perc_lab(t_pred_60)

    # 2 horas
    t_pred_120 = df_one_line['PREDICCION_INT 120'].iloc[0]
    tl_120_coef = take_perc_lab(t_pred_120)

    ### Calculamos el coeficiente total

    coef_15 = seg_coef * time_coef_15 * lab_coef * tl_15_coef  # 15 minutos
    coef_60 = seg_coef * time_coef_60 * lab_coef * tl_60_coef  # 1 hora
    coef_120 = seg_coef * time_coef_120 * lab_coef * tl_120_coef  # 2 horas

    #print(coef_15, coef_60, coef_120)

    ### Calculamos el nivel de accidentalidad

    acc_15 = percent_to_level(coef_15)
    acc_60 = percent_to_level(coef_60)
    acc_120 = percent_to_level(coef_120)

    return acc_15, acc_60, acc_120


def generate_output(selected_segments, active_models, preds, accs, current_date, df_input):
    """ This method generated the output as Pandas DataFrame in order to easily export it to a csv file (in the expected output format)
    Args:
        selected_segments (list): The list of segments that has been selected in the configuration file in at least one model
        active_models (list): A list with the current active models
        preds (dict): A nested Python's dict with the predictions made
        current_date (datetime): The current date (start date for the forecasting) in the format '%gmaiY-%m-%d %H:%M:%S'

    Returns:
        predictions (dict): A Python's dict with the model predictions (forecast horizon) for the selected segments
    """
    logging.info("STEP_4 SAVING RESULTS TO OUTPUT FILE")

    df_output = pd.DataFrame(index=range(0, int(len(selected_segments)*len(active_models))), columns=['ALGORITHM_ORIGIN', 'ID_SEGMENT', 'FECHA', 'PREDICCION_INT 15', 'PREDICCION_INT 60', 'PREDICCION_INT 120', 'IND_ACCIDENTE_15', 'IND_ACCIDENTE_60', 'IND_ACCIDENTE_120'])
    # fill DataFrame with the predictions
    i=0
    for seg_id in selected_segments:

        if (len(df_input.loc[(df_input.ID_SEGMENT == seg_id) & (df_input.FECHA == current_date)]['TOTAL_VEHICULOS'].index)==0):
            current_total_veh =  -127  # this value does not exist
        elif(pd.isnull(df_input.loc[(df_input.ID_SEGMENT == seg_id) & (df_input.FECHA == current_date)]['TOTAL_VEHICULOS'].iloc[0])):
            current_total_veh =  -127 # this value does not exist
        else:
            current_total_veh = df_input.loc[(df_input.ID_SEGMENT == seg_id) & (df_input.FECHA == current_date)]['TOTAL_VEHICULOS'].iloc[0]

        for model_name in active_models:
            model_name = model_name.split('-')[0]
            print("--> Segmento ID: " + str(seg_id) + " Fecha: " +  str(current_date))

            segment_list = [algo_case['by_segment'] for algo_case in cfg['make_predictions'] if
                            algo_case['algorithm_name'] == str(model_name)]  #

            if seg_id in segment_list[0]:
                if(str(seg_id) in preds.keys()):

                    if(len(preds[str(seg_id)][model_name][:])<8):
                        print(model_name)
                        print("Warning! Zero's predictions at: " +  str(seg_id) +  " Algorithm: " + str(model_name) + " Date: " + str(current_date))
                        preds[str(seg_id)][model_name]=[current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh, current_total_veh]
                    else:
                        if current_total_veh == -127:
                            current_total_veh = preds[str(seg_id)][model_name][0]

                    df_output.iloc[i]['ALGORITHM_ORIGIN'] = algorithms_ids[str(model_name)]
                    df_output.iloc[i]['ID_SEGMENT'] = seg_id
                    df_output.iloc[i]['FECHA'] = datetime.strptime(str(current_date), '%Y-%m-%d %H:%M:%S')
                    df_output.iloc[i]['PREDICCION_INT 15'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][0]], weights=[0.5, 0.5]))


                    if model_name == 'extra_trees_1' or model_name == 'extra_trees_2' or model_name == 'extra_trees_2a' or model_name=='gradient_boosting' or model_name == 'random_forest' or model_name == 'mlp_neural_network' or model_name == 'kn_neighbors' or model_name=='bagging_regressor':
                        df_output = apply_predefined_policies(df_output, preds, model_name, seg_id, i, current_total_veh)

                    if model_name=='arima':
                        df_output.iloc[i]['PREDICCION_INT 60'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(60 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.3, 0.3, 0.4]))
                        df_output.iloc[i]['PREDICCION_INT 120'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(120 / 15) - 1], preds[str(seg_id)]['desc_ref'][int(120 / 15) - 1]], weights=[0.25, 0.25, 0.5]))

                    elif model_name!='arima' and model_name!='minsait':
                        df_output.iloc[i]['PREDICCION_INT 60'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(60 / 15) - 1],  preds[str(seg_id)]['desc_ref'][int(120/15)-1]], weights=[0.3, 0.4, 0.3]))
                        df_output.iloc[i]['PREDICCION_INT 120'] = int(np.average([current_total_veh, preds[str(seg_id)][model_name][int(120/15)-1],  preds[str(seg_id)]['desc_ref'][int(120/15)-1]],  weights=[0.25, 0.5, 0.25]))


                    if model_name=='minsait':
                        df_output.iloc[i]['IND_ACCIDENTE_15'] = accs[str(seg_id)][model_name][0]
                        df_output.iloc[i]['IND_ACCIDENTE_60'] = accs[str(seg_id)][model_name][int(60/15)-1]
                        df_output.iloc[i]['IND_ACCIDENTE_120'] = accs[str(seg_id)][model_name][int(120/15)-1]
                    else: # not implemented yet
                        acc_15, acc_60, acc_120 = make_accident_predictions(df_output.iloc[i])
                        df_output.iloc[i]['IND_ACCIDENTE_15'] = acc_15
                        df_output.iloc[i]['IND_ACCIDENTE_60'] = acc_60
                        df_output.iloc[i]['IND_ACCIDENTE_120'] = acc_120
                    i = i + 1

    return df_output

def save_checkpoint(current_date, df_result):
    if current_date.strftime('%Y-%m-%d %H:%M:%S') == '2019-02-01 02:00:00':  # first check point
        logging.debug("---> generating first check point")
        df_result.to_csv('test2_1_partial_results.csv', sep=';', index=False)

    if current_date.strftime('%Y-%m-%d %H:%M:%S') == '2019-02-01 10:00:00':  # first check point
        logging.debug("---> generating first check point")
        df_result.to_csv('test2_1_partial_results.csv', sep=';', index=False)

    if current_date.strftime('%Y-%m-%d %H:%M:%S') == '2019-02-01 20:00:00':  # second check point
        logging.debug("---> generating second check point")
        df_result.to_csv('test2_2_partial_results.csv', sep=';', index=False)

    if current_date.strftime('%Y-%m-%d %H:%M:%S') == '2019-02-02 20:00:00':  # third check point
        logging.debug("---> generating third check point")
        df_result.to_csv('test2_3_partial_results.csv', sep=';', index=False)


def _build_cfg(project_name, project_scope, execution_mode, reports_path, input_data, output_path, logs_path, newdata_timeout, algorithms_configuration):
    """
    Auxiliar function used for interoperability purposes

    :return:

    """

    cfg = dict()

    cfg['global']["project_name"] = project_name
    cfg['global']["project_scope"] = project_scope
    cfg['global']["execution_mode"] = execution_mode
    cfg['global']["reports_path"] = reports_path
    cfg['global']["logs_path"] = logs_path
    cfg['global']["input_data"] = input_data
    cfg['global']["output_path"] = output_path

    cfg['realtime_mode']['newdata_timeout'] = newdata_timeout

    cfg['pipeline_execution']['generate_datasets'] = False
    cfg['pipeline_execution']['restore_datasets'] = False
    cfg['pipeline_execution']['descriptive_analysis'] = False
    cfg['pipeline_execution']['train_algorithms'] = False
    cfg['pipeline_execution']['model_evaluation'] = False
    cfg['pipeline_execution']['realtime_monitoring'] = False

    cfg['pipeline_execution']['make_predictions'] = True


    cfg['make_predictions'] = algorithms_configuration

    return cfg

def _main(project_name, project_scope, execution_mode, reports_path, input_data, output_path, logs_path, newdata_timeout, algorithms_configuration):
    """

    :param cfg:
       e.g.
    cfg['global']["project_name"] = "01_horus_traffic_prediction"
    cfg['global']["project_scope"] =  "AUSOL",
    cfg['global']["execution_mode"] = "offline"
    cfg['global']["reports_path"] = "C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\reports\\"
    cfg['global']["logs_path"] = "C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\reports\\"
    cfg['global']["input_data"] = "C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\data\\TT_NL.csv"
    cfg['global']["output_path"] = "C:\\Users\\yhoz\\Documents\\dataanalytics.predictive\\output\\results.csv"

    cfg['realtime_mode']['newdata_timeout'] = 190

    cfg['make_predictions'] = [{'algorithm_name': 'minsait', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                            {'algorithm_name': 'desc_ref', 'activated': True,
                              'by_segment': [1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                                             30, 31, 32, 33, 34, 35, 39, 40, 42, 43, 100, 101, 102, 103, 104, 105, 106,
                                             107], 'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'ets', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'arima', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'snaive', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'tbats', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'fbprophet', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': ''},
                             {'algorithm_name': 'extra_trees_2a', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m4c4v4g'},
                             {'algorithm_name': 'extra_trees_2', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 30, 31,
                                             32, 33, 40, 42, 100, 101, 102, 103, 104, 105, 106, 107],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m4c1v4g'},
                             {'algorithm_name': 'random_forest', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 30, 31,
                                             32, 33, 40, 42, 100, 101, 102, 103, 104, 105, 106, 107],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m2c1v4g'},
                             {'algorithm_name': 'mlp_neural_network', 'activated': True,
                              'by_segment': [1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 30, 31,
                                             32, 33, 40, 42, 100, 101, 102, 103, 104, 105, 106, 107],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m2c1v3g'},
                             {'algorithm_name': 'gradient_boosting', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 30, 31,
                                             32, 33, 40, 42, 100, 101, 102, 103, 104, 105, 106, 107],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m4c1v4g'},
                             {'algorithm_name': 'kn_neighbors', 'activated': False,
                              'by_segment': [1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 30, 31,
                                             32, 33, 40, 42, 100, 101, 102, 103, 104, 105, 106, 107],
                              'hours_to_forecast': 100, 'incremental_forecast': False, 'code': '6m2c1v4g'}]

    :return:
    """

    cfg = _build_cfg(project_name, project_scope, execution_mode, reports_path, input_data, output_path, logs_path, newdata_timeout, algorithms_configuration)

    logging.info("STEP_1 LOAD DATASETS")
    datasets = dict()

    datasets = step_1_load_datasets(cfg)

    ### STEP_2 MODEL TRAINING OR RESTORING
    ### --------------------------------------
    if cfg['pipeline_execution']['make_predictions'] or cfg['pipeline_execution']['train_algorithms']:
        selected_segments = list(set(tuple(segs['by_segment']) for segs in cfg['train_algorithms']))[
            0]  # get active segments list

        logging.info("STEP_2 MODEL TRAINING OR RESTORING")
        trained_models = step_2_model_training(cfg, selected_segments, datasets['input'], False, None)
        end_dataset_date = datetime.strptime(cfg['range_date'][1], '%Y-%m-%d %H:%M:%S')

    ### STEP_3 GENERATE PREDICTIONS
    ### --------------------------------------
    if cfg['pipeline_execution']['make_predictions']:
        logging.debug("STEP_3 GENERATE PREDICTIONS IN MODE: " + str(cfg['global']['execution_mode']))
        forecast_times = [15, 60, 120]
        preds = dict()
        accs = dict()
        active_models = [str(algo_case['algorithm_name'] + '-' + algo_case['code']) for algo_case in
                         cfg['make_predictions'] if algo_case['activated'] == True]
        forecast_horizon = max(forecast_times)  # the number of predictions to be made
        logging.debug("CHECKING NEW DATA")
        isnew_data, df_input, current_date = check_incoming_data()

        df_input.sort_values(by=['FECHA'], inplace=True)
        df_input = df_input.reset_index(drop=True)

        if cfg['global']['execution_mode'] == 'realtime':
            data_training = pd.DataFrame()
            time_counter = 0
            isnew_data = True

            while (True):

                if isnew_data:
                    cfg['current_code'] = df_input.iloc[0]['COD_LABORALIDAD']

                    logging.info("GENERATING PREDICTIONS FOR INPUT FILE: " + str(current_date))
                    preds, accs = make_predictions(trained_models, selected_segments, forecast_horizon, time_counter,
                                                   cfg, current_date,
                                                   df_input)  # TODO: selected_segments with the list of predictions part of cfg
                    df_predictions = generate_output(selected_segments, active_models, preds, accs, current_date,
                                                     df_input)

                    df_predictions.to_csv(cfg['global']['output_path'], sep=';', index=False)

                    time_counter = time_counter + 1

                logging.debug("CHECKING NEW DATA")
                isnew_data, df_input, current_date = check_incoming_data()

                if cfg['conitnuous_learning']['train_update'] == True & time_counter >= cfg['conitnuous_learning'][
                    'hours_to_retrain'] * int(60 / 15):
                    trained_models = step_2_model_training(cfg, selected_segments, datasets['input'], True,
                                                           trained_models)
                    time_counter = 0
                    data_training = pd.DataFrame().reindex_like(datasets['input'])

                # if cfg['pipeline_execution']['model_evaluation']:
                #    df_log = step_4_evaluation_metrics(cfg, predictions, df_expected, finish_date, df_log)

        else:  # validation

            init_date = datetime.strptime(str(df_input.head(1)['FECHA'][0]), '%Y-%m-%d %H:%M:%S')
            end_date = datetime.strptime(str(df_input.tail(1)['FECHA'][df_input.shape[0] - 1]), '%Y-%m-%d %H:%M:%S')
            df_result = pd.DataFrame()
            current_date = init_date

            while (current_date <= end_date):
                predictions, accs = make_predictions(trained_models, selected_segments, forecast_horizon, 0, cfg,
                                                     current_date, df_input)
                df_predictions = generate_output(selected_segments, active_models, predictions, accs, current_date,
                                                 df_input)
                df_result = df_result.append(df_predictions)
                current_date = current_date + timedelta(minutes=15)

                save_checkpoint(current_date, df_result)

            df_result.to_csv(cfg['global']['output_path'], sep=';', index=False)

if project_scope == 'AUSOL':
    acc_levels = {'0': [None, 0.0028522647259977335], '1': [0.0028522647259977335, 0.0074223502150555344],
                  '2': [0.0074223502150555344, 0.010663433219218475], '3': [0.010663433219218475, 0.014410037645342056],
                  '4': [0.014410037645342056, 0.23975201208123215], '5': [0.23975201208123215, None]}

    cv_segments = {1: 0.08490566037735849, 2: 0.02830188679245283, 4: 0.07547169811320754,
                   5: 0.0660377358490566, 7: 0.07547169811320754, 8: 0.14150943396226415,
                   15: 0.18867924528301888, 16: 0.04716981132075472, 18: 0.08490566037735849,
                   19: 0.08490566037735849, 21: 0.07547169811320754, 22: 0.04716981132075472}

    cv_time = {'02_05': 0.11214953271028037, '06_10': 0.31401869158878504,
               '11_19': 0.43198338525441327, '20_01': 0.3115264797507788}

    cv_lab = {'Working': 0.3348623853211009, 'No_Working': 0.2980769230769231}

    cv_traf = {'0.6325524044389642': [0, 210.0], '1.0810810810810811': [210.0, 330.0],
               '3.3820224719101124': [330.0, 460.0], '38.26086956521739': [460.0, 550.0],
               '0': [550.0, 600.0], '18.0': [600.0]}


    # entrada : ID_SEGMENT;FECHA;PREDICCION_INT 15;PREDICCION_INT 60;PREDICCION_INT 120

    # Funciones auxiliares

    def take_perc_lab(x):
        for key in cv_traf.keys():
            valor = cv_traf[key]
            if (len(valor) > 1):
                if ((x >= valor[0]) and (x < valor[1])):
                    return float(key)
            else:
                if (x >= valor[0]):
                    return float(key)


    def percent_to_level(x):
        for level in acc_levels.keys():
            limits = acc_levels[level]
            if (limits[0] == None):
                if (x <= limits[1]):
                    return int(level)
            elif (limits[1] == None):
                if (x > limits[0]):
                    return int(level)
            else:
                if ((limits[0] < x) & (x <= limits[1])):
                    return int(level)


    def take_perc_time(x):
        for key in cv_time.keys():
            limits = key.split('_')
            if (limits[0] == '20'):
                if ((x >= int(limits[0])) | (x <= int(limits[1]))):
                    return cv_time[key]
            else:
                if ((x >= int(limits[0])) & (x <= int(limits[1]))):
                    return cv_time[key]


    ## Dias festivos Andalucia
    hol_AND = holidays.CountryHoliday('ES', prov='AND', state=None)
    hol_AND.append(datetime(2018, 12, 24))
    hol_AND.append(datetime(2018, 12, 31))
    hol_AND.append(datetime(2019, 12, 24))
    hol_AND.append(datetime(2019, 12, 31))
    hol_AND.append(datetime(2017, 12, 24))
    hol_AND.append(datetime(2017, 12, 31))


    # Funcion de prediccion

    def make_accident_predictions(df_one_line):

        # print('Dataframe de entrada: ', df_one_line)

        segment = df_one_line['ID_SEGMENT']

        # print('Segmento: ', segment)

        if (segment not in [1, 2, 4, 5, 7, 8, 15, 16, 18, 19, 21, 22]):
            return -127, -127, -127

        fecha_dt = pd.to_datetime(df_one_line.FECHA)
        wk = ''

        if (fecha_dt.weekday() > 4):
            wk = 'No_Working'
        else:
            wk = 'No_Working' if (fecha_dt in hol_AND) else 'Working'

        # print('Lab:', wk)

        d15 = timedelta(hours=0, minutes=15)
        d60 = timedelta(hours=1, minutes=0)
        d120 = timedelta(hours=2, minutes=0)

        date_15 = fecha_dt + d15
        date_60 = fecha_dt + d60
        date_120 = fecha_dt + d120

        hour_15 = date_15.hour
        hour_60 = date_60.hour
        hour_120 = date_120.hour

        # print('Hora:', hour_15, hour_60, hour_120)

        # Tenemos nuestras tres variables: segment, wk, hour
        ### Lo siguiente es calcular los coeficientes

        # Segmento
        seg_coef = cv_segments[segment]

        # Hora
        time_coef_15 = take_perc_time(hour_15)
        time_coef_60 = take_perc_time(hour_60)
        time_coef_120 = take_perc_time(hour_120)

        # Laboralidad
        lab_coef = cv_lab[wk]

        ### Calculamos el nivel de tráfico, que es distinto para cada predicción

        # 15 minutos
        _15 = True
        tl_15_coef = 0
        t_pred_15 = df_one_line['PREDICCION_INT 15']
        if (t_pred_15 < 0):
            _15 = False
        else:
            tl_15_coef = take_perc_lab(t_pred_15)

        # 1 hora
        _60 = True
        tl_60_coef = 0
        t_pred_60 = df_one_line['PREDICCION_INT 60']
        if (t_pred_60 < 0):
            _60 = False
        else:
            tl_60_coef = take_perc_lab(t_pred_60)

        # 2 horas
        _120 = True
        tl_120_coef = 0
        t_pred_120 = df_one_line['PREDICCION_INT 120']
        if (t_pred_120 < 0):
            _120 = False
        else:
            tl_120_coef = take_perc_lab(t_pred_120)

        ### Calculamos el coeficiente total y Calculamos el nivel de accidentalidad
        if (_15):
            coef_15 = seg_coef * time_coef_15 * lab_coef * tl_15_coef  # 15 minutos
            acc_15 = percent_to_level(coef_15)
        else:
            acc_15 = -127
        if (_60):
            coef_60 = seg_coef * time_coef_60 * lab_coef * tl_60_coef  # 1 hora
            acc_60 = percent_to_level(coef_60)
        else:
            acc_60 = -127
        if (_120):
            coef_120 = seg_coef * time_coef_120 * lab_coef * tl_120_coef  # 2 horas
            acc_120 = percent_to_level(coef_120)
        else:
            acc_120 = -127

        return acc_15, acc_60, acc_120

else:

    acc_levels = {'0': [None, 0.0016426548381705923], '1': [0.0016426548381705923, 0.009114958351401407],
                  '2': [0.009114958351401407, 0.02758504757345051], '3': [0.02758504757345051, 0.06554705999146901],
                  '4': [0.06554705999146901, 0.21970579081135017], '5': [0.21970579081135017, None]}

    cv_segments = {1: 0.023529411764705882, 2: 0.01568627450980392, 3: 0.06274509803921569,
                   4: 0.16862745098039217, 5: 0.11764705882352941, 6: 0.06666666666666667,
                   7: 0.0196078431372549, 8: 0.023529411764705882, 11: 0.054901960784313725,
                   12: 0.023529411764705882, 13: 0.03137254901960784, 14: 0.1843137254901961,
                   15: 0.0784313725490196, 16: 0.06666666666666667, 17: 0.03529411764705882,
                   18: 0.027450980392156862}

    cv_time = {'23_02': 0.5753424657534247, '03_07': 0.19726027397260273, '08_15': 0.9945205479452055,
               '16_22': 0.7608610567514676}

    cv_lab = {'Working': 0.7016129032258065, 'No_Working': 0.6610169491525424}

    cv_traf = {'0.9178384050367262': [0, 210.0], '2.710112359550562': [210.0, 330.0],
               '1.8800000000000001': [330.0, 460.0], '0': [460.0, 550.0],
               '0.0': [550.0, 600.0], '0.00': [600.0]}


    # entrada : ID_SEGMENT;FECHA;PREDICCION_INT 15;PREDICCION_INT 60;PREDICCION_INT 120

    # Funciones auxiliares

    def take_perc_lab(x):
        for key in cv_traf.keys():
            valor = cv_traf[key]
            if (len(valor) > 1):
                if ((x >= valor[0]) and (x < valor[1])):
                    return float(key)
            else:
                if (x >= valor[0]):
                    return float(key)


    def percent_to_level(x):
        for level in acc_levels.keys():
            limits = acc_levels[level]
            if (limits[0] == None):
                if (x <= limits[1]):
                    return int(level)
            elif (limits[1] == None):
                if (x > limits[0]):
                    return int(level)
            else:
                if ((limits[0] < x) & (x <= limits[1])):
                    return int(level)


    def take_perc_time(x):
        for key in cv_time.keys():
            limits = key.split('_')
            if (limits[0] == '23'):
                if ((x >= int(limits[0])) | (x <= int(limits[1]))):
                    return cv_time[key]
            else:
                if ((x >= int(limits[0])) & (x <= int(limits[1]))):
                    return cv_time[key]


    ## Dias festivos Portugal
    hol_PTE = holidays.PTE()


    # Funcion de prediccion

    def make_accident_predictions(df_one_line):

        # print('Dataframe de entrada: ', df_one_line)

        segment = df_one_line['ID_SEGMENT']

        if (segment in [9, 10, 19, 20]):
            return -127, -127, -127

        fecha_dt = pd.to_datetime(df_one_line.FECHA)
        wk = ''

        if (fecha_dt.weekday() > 4):
            wk = 'No_Working'
        else:
            wk = 'No_Working' if (fecha_dt in hol_PTE) else 'Working'

        # print('Lab:', wk)

        d15 = timedelta(hours=0, minutes=15)
        d60 = timedelta(hours=1, minutes=0)
        d120 = timedelta(hours=2, minutes=0)

        date_15 = fecha_dt + d15
        date_60 = fecha_dt + d60
        date_120 = fecha_dt + d120

        hour_15 = date_15.hour
        hour_60 = date_60.hour
        hour_120 = date_120.hour

        # print('Hora:', hour_15, hour_60, hour_120)

        # Tenemos nuestras tres variables: segment, wk, hour
        ### Lo siguiente es calcular los coeficientes

        # Segmento
        seg_coef = cv_segments[segment]

        # Hora
        time_coef_15 = take_perc_time(hour_15)
        time_coef_60 = take_perc_time(hour_60)
        time_coef_120 = take_perc_time(hour_120)

        # Laboralidad
        lab_coef = cv_lab[wk]

        ### Calculamos el nivel de tráfico, que es distinto para cada predicción

        # 15 minutos
        _15 = True
        tl_15_coef = 0
        t_pred_15 = df_one_line['PREDICCION_INT 15']
        if (t_pred_15 < 0):
            _15 = False
        else:
            tl_15_coef = take_perc_lab(t_pred_15)

        # 1 hora
        _60 = True
        tl_60_coef = 0
        t_pred_60 = df_one_line['PREDICCION_INT 60']
        if (t_pred_60 < 0):
            _60 = False
        else:
            tl_60_coef = take_perc_lab(t_pred_60)

        # 2 horas
        _120 = True
        tl_120_coef = 0
        t_pred_120 = df_one_line['PREDICCION_INT 120']
        if (t_pred_120 < 0):
            _120 = False
        else:
            tl_120_coef = take_perc_lab(t_pred_120)

        ### Calculamos el coeficiente total y Calculamos el nivel de accidentalidad
        if (_15):
            coef_15 = seg_coef * time_coef_15 * lab_coef * tl_15_coef  # 15 minutos
            acc_15 = percent_to_level(coef_15)
        else:
            acc_15 = -127
        if (_60):
            coef_60 = seg_coef * time_coef_60 * lab_coef * tl_60_coef  # 1 hora
            acc_60 = percent_to_level(coef_60)
        else:
            acc_60 = -127
        if (_120):
            coef_120 = seg_coef * time_coef_120 * lab_coef * tl_120_coef  # 2 horas
            acc_120 = percent_to_level(coef_120)
        else:
            acc_120 = -127

        return acc_15, acc_60, acc_120

if __name__ == "__main__":
    global logging
    try:
        parser = parse_cmdline_args()
        args = parser.parse_args()
    except Exception as e:
        sys.stdout.write("Check command line arguments (%s)" % e.strerror)

### STEP_0 LOAD PROGRAM CONFIGURATION FILE
### --------------------------------------
    logging = setup_logger(args.logger)
    logging.info("STEP_0 LOAD PROGRAM CONFIGURATION FILE")
    cfg = step_0_load_program_conf(args)

    datasets=dict()

### STEP_1 LOAD DATASETS
### --------------------------------------
    logging.info("STEP_1 LOAD DATASETS")
    datasets = step_1_load_datasets(cfg)

    ### STEP_ DESCRIPTIVE ANALYSIS
    ### --------------------------------------
    if cfg['pipeline_execution']['descriptive_analysis']:
        logging.debug("EXECUTING DESCRIPTIVE ANALYSIS")
        step_data_profiling(datasets)

### STEP_2 MODEL TRAINING OR RESTORING
### --------------------------------------
    if cfg['pipeline_execution']['make_predictions'] or cfg['pipeline_execution']['train_algorithms']:
        selected_segments = list(set(tuple(segs['by_segment']) for segs in cfg['train_algorithms']))[0]  # get active segments list

        logging.info("STEP_2 MODEL TRAINING OR RESTORING")
        trained_models = step_2_model_training(cfg, selected_segments, datasets['input'], False, None)
        end_dataset_date = datetime.strptime(cfg['range_date'][1], '%Y-%m-%d %H:%M:%S')

### STEP_3 GENERATE PREDICTIONS
### --------------------------------------
    if cfg['pipeline_execution']['make_predictions']:
        logging.debug("STEP_3 GENERATE PREDICTIONS IN MODE: " + str(cfg['global']['execution_mode']))
        forecast_times=[15, 60, 120]
        preds = dict()
        accs = dict()
        active_models = [str(algo_case['algorithm_name'] + '-' + algo_case['code']) for algo_case in cfg['make_predictions'] if algo_case['activated'] == True]
        forecast_horizon = max(forecast_times)  # the number of predictions to be made
        logging.debug("CHECKING NEW DATA")
        isnew_data, df_input, current_date = check_incoming_data()

        df_input.sort_values(by=['FECHA'], inplace=True)
        df_input = df_input.reset_index(drop=True)

        if cfg['global']['execution_mode']=='realtime':
            data_training = pd.DataFrame()
            time_counter = 0
            isnew_data=True

            while (True):

                if isnew_data:
                    cfg['current_code'] = df_input.iloc[0]['COD_LABORALIDAD']

                    logging.info("GENERATING PREDICTIONS FOR INPUT FILE: " + str(current_date))
                    preds, accs = make_predictions(trained_models, selected_segments, forecast_horizon, time_counter, cfg, current_date, df_input) # TODO: selected_segments with the list of predictions part of cfg
                    df_predictions = generate_output(selected_segments, active_models, preds, accs, current_date, df_input)

                    df_predictions.to_csv(cfg['global']['output_path'], sep=';', index=False)

                    time_counter = time_counter + 1

                logging.debug("CHECKING NEW DATA")
                isnew_data, df_input, current_date = check_incoming_data()


                if cfg['conitnuous_learning']['train_update'] == True & time_counter>=cfg['conitnuous_learning']['hours_to_retrain']*int(60/15):
                    trained_models = step_2_model_training(cfg, selected_segments, datasets['input'], True, trained_models)
                    time_counter = 0
                    data_training = pd.DataFrame().reindex_like(datasets['input'])

                #if cfg['pipeline_execution']['model_evaluation']:
                #    df_log = step_4_evaluation_metrics(cfg, predictions, df_expected, finish_date, df_log)

        else:  # validation

            init_date = datetime.strptime(str(df_input.head(1)['FECHA'][0]), '%Y-%m-%d %H:%M:%S')
            end_date = datetime.strptime(str(df_input.tail(1)['FECHA'][df_input.shape[0]-1]), '%Y-%m-%d %H:%M:%S')
            df_result = pd.DataFrame()
            current_date = init_date

            while(current_date<=end_date):

                predictions, accs = make_predictions(trained_models, selected_segments, forecast_horizon, 0, cfg, current_date, df_input)
                df_predictions = generate_output(selected_segments, active_models, predictions, accs, current_date, df_input)
                df_result = df_result.append(df_predictions)
                current_date = current_date + timedelta(minutes=15)

                save_checkpoint(current_date, df_result)

            df_result.to_csv(cfg['global']['output_path'], sep=';', index=False)





### STEP_5  REALTIME MONITORING VISUALIZATION
### --------------------------------------

    if cfg['pipeline_execution']['realtime_monitoring']:
        logging.info("STEP_5 REALTIME MONITORING VISUALIZATION")

        df_result = pd.read_csv('D:\\Usuarios\mdbrenes\Documents\\dataanalytics.predictive\\output\\results.csv', delimiter=";")
        df_result.dropna(how='all', inplace=True)
        #df_result = pd.read_csv(cfg['global']['output_path'], delimiter=";")
        df_result = df_result.loc[pd.to_datetime(df_result['FECHA']) < datetime.strptime('2019-02-01 10:00:00', '%Y-%m-%d %H:%M:%S')]
        df_input = pd.read_csv(cfg['global']['input_data'], delimiter=";")

        #df_input_2 = pd.read_csv('C:\\Users\\yhoz\\Documents\dataanalytics.predictive\\src\\preds_march_2019.csv', delimiter=";")
        segment_id = 16
        df_result_seg = df_result.loc[df_result['ID_SEGMENT'] == segment_id]
        df_input_seg = df_input.loc[(df_input['ID_SEGMENT'] == segment_id)]
        #df_input_seg_2 = df_input_2.loc[(df_input_2['ID_SEGMENTO'] == segment_id) & (df_input_2.MES == 3)]
        df_input_seg = df_input_seg.loc[pd.to_datetime(df_input_seg['FECHA']) < datetime.strptime('2019-02-01 10:00:00', '%Y-%m-%d %H:%M:%S')]
        horizon = 15
        code='6m4c4v4g'


        df_final= pd.DataFrame(index=df_input_seg.FECHA, columns=['FECHA','res_real_ref', 'res_real','res_arima','res_desc_ref', 'res_fbprophet', 'res_ets', 'res_mlp_neural_network', 'res_extra_tree_1', 'res_extra_tree_2',  'res_extra_tree_2a', 'res_random_forest', 'res_gradient_boosting', 'res_kn_neighbors'])
        total_dates = len(df_input_seg.FECHA.values)
        df_final['FECHA']= df_input_seg.FECHA.values
        #df_final['res_real_ref'] = df_input_seg['TOTAL_VEHICULOS_' + str(int(horizon/15))].values
        df_final['res_real'] = df_input_seg['TOTAL_VEHICULOS'].values
        #df_final['res_arima'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['arima']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        df_final['res_desc_ref'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['desc_ref']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        #df_final['res_fbprophet'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['fbprophet']]['PREDICCION_INT 120'].values.astype(int)[0:total_dates]
        #df_final['res_ets'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['ets']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]


        df_final['res_mlp_neural_network'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['mlp_neural_network']]['PREDICCION_INT 60'].values.astype(int)[0:total_dates]
        #df_final['res_extra_tree_1'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['extra_trees_1']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        df_final['res_extra_tree_2'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['extra_trees_2']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        #df_final['res_extra_tree_2a'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['extra_trees_2a']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        df_final['res_random_forest'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['random_forest']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        df_final['res_gradient_boosting'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['gradient_boosting']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]
        #df_final['res_kn_neighbors'] = df_result_seg.loc[df_result_seg['ALGORITHM_ORIGIN'] == algorithms_ids['kn_neighbors']]['PREDICCION_INT '+ str(horizon)].values.astype(int)[0:total_dates]

        #x_time = df_result['FECHA'].unique()

        app = realtime_monitoring.generate_realtime_monitor(segment_id, cfg['range_date'],  df_final)
        app.run_server(debug=True, use_reloader=False)






        # (3) generate train dataset --> train_TT_NL.csv
        #train = test.join(target, ["ID_SEGMENT","FECHA"], how='left')
        #col_features = test.columns
        #col_features.append('INTENSIDAD_TR')  # INTENSIDAD_TR is the target
        #train = train.select(col_features)
        #train.show(100)
