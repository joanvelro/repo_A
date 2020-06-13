from datetime import datetime, timedelta, date
import logging
import sys
from pyspark.sql.functions import percent_rank
from pyspark.sql import Window
import holidays


def get_holidays_dataset(nyear):
    """ This methods will give the bank holidays for a specific year

    Args:
        nyear (int): the year

    Returns:
        es_holidays(obj): A obj with the list of days

    """
    es_holidays = holidays.ES(years=nyear).items()
    return es_holidays


def get_weekdays_year(nyear, weekday_id):
    """ This methods will give the bank holidays for a specific year

    Args:
        nyear (int): the year

    Returns:
        es_holidays(obj): A obj with the list of days

    """

    # weekday_id is python's weekday. The day of the week as an integer, where Monday is 0 and Sunday is 6
    d = date(nyear, 1, 1)  # January 1st
    d += timedelta(days=weekday_id - d.weekday())  # First Sunday
    days = []
    while d.year == nyear:
        days.append(d)
        d += timedelta(days=7)

    return days

def get_weekday(date):
    """ This methods will give the weekday corresponding a specific date

    Args:
        date (str): the date in the format '%Y-%m-%d %H:%M:%S'

    Returns:
        weekday(str): A str with the name of weekday (e.g. Wednesday)

    """

    import datetime
    import calendar
    date_day = date.split(" ")
    month, day, year = (int(x) for x in date_day.split('-'))
    weekday = datetime.date(year, month, day)
    return calendar.day_name[weekday.weekday()]


def join_datasets_by_date(folder, dates):
    """ Joins the datasets generated for different dates in separate folders (named with the date range) into one single dataset
        It will create new single dataset with new range date containing the others

    Args:
        folder (str): The parent folder where the datasets are stored. Children folders are expected to follow the naming convention by range date
        dates (list): The list of dates to be included

    Returns:
        bool: The return value. True for success, False otherwise.

    """
    return True


def aggregate_dataset_by_date(file_path, level):
    """ Performs an aggregation of the dataset at the defined level computing the avg, max, min or exponential avg.
        It will create new single dataset with the new aggregation at the defined level

    Args:
        file_path (str): The dataset to be aggregated
        level (str): The aggregation level to be used
                     Levels:
                            day: Data will be aggregated at day level. This will create a summary day of the data
                            week: Data will be aggregated at week level. This will create a summary week of the data
                            month: Data will be aggregated at month level. This will create a summary month of the data
                            year: Data will be aggregated at year level. This will create a summary year of the data

    Returns:
        bool: The return value. True for success, False otherwise.

    """
    return True


def split_train_test_byfactor(df_input, datasets, volume_factor):
    """ This function will split the df_input (input data) into the tipical train and testing set used for training machine learning algorithms

    Args:
        df_input (dataframe): The input dataset to be split
        datasets (str): The Python dict to store the datastes
        volume_factor (int): The factor indicating the data volume to be included in train and test. E.g. If a factor of 0.7 is defined. 0.7 --> TRAIN and 0.3 --> TEST

    Returns:
        dict: The Python's dict with the generated datasets

    """
    df_input = df_input.withColumn("rank", percent_rank().over(Window.partitionBy().orderBy("FECHA")))  # make sure it is ordered by date
    datasets['train'] = df_input.where("rank <= {}".format(str(volume_factor))).drop("rank").toPandas()
    datasets['test'] = df_input.where("rank > {}".format(str(volume_factor))).drop("rank").drop("TOTAL_VEHICULOS").toPandas()  # Drop TOTAL_VEHICULOS from validation. TOTAL_VEHICULOS is the target (independent variable to be predicted)
    # print('\033[94m' + "----> VALIDATION DATASET: \nNumber of rows: " + str(datasets['test'].count()) + "\n" + "Date range: " + str(datasets['test'].first(1)['FECHA']) + "," + DATE_END +'\033[0m' + '\n')
    # print('\033[94m' + "----> TRAIN DATASET: \n Number of rows: " + str(datasets['train'].count()) + "\n" + "Date range: " + DATE_INIT + "," + str(datasets['test'].first(1)['FECHA']) +'\033[0m' + '\n')
    return datasets


def split_train_test_bydate(df_input, init_train_date, end_train_date, init_forecast_date, end_forecast_date, finish_forecast_date):
    """ This function will split the df_input (input data) into the tipical train and testing set used for training machine learning algorithms

    Args:
        df_input (dataframe): The input dataset to be split
        init_train_date (datetime): The init date for the training dataset
        end_train_date (date): The end date for the training dataset
        init_forecast_date (date): The init date for the testing dataset
        end_forecast_date (date): The end date for the testing dataset
        finish_forecast_date (date): The last date to be forecasted (end of forecasts)

    Returns:
        df_training: Dataframe with the training set
        df_test: Dataframe with the testing set
    """

    df_training = df_input.loc[(df_input['FECHA'] >= init_train_date) & (df_input['FECHA'] < end_train_date)]
    if ((finish_forecast_date-timedelta(hours=2)) == init_forecast_date):# take the last too
        df_test = df_input.loc[
            (df_input['FECHA'] >= init_forecast_date) & (df_input['FECHA'] <= end_forecast_date)]
    else:
        df_test = df_input.loc[(df_input['FECHA'] >= init_forecast_date) & (df_input['FECHA'] < end_forecast_date)]
    df_test.drop("TOTAL_VEHICULOS", axis=1, inplace=True)

    if df_training.empty:
        logging.error("The dataset should be re-generated for the current dates: " + str(init_train_date) + "," + str(end_train_date) + " \n" + "Please, activate in the config file the 'generate_dataset' step")
        sys.exit(0)

    return df_training, df_test


def generate_sliding_windows(cfg, algo_active):
    """ This function will give the range dates for training and testing for each algorithm

    Args:
        cfg (dict): A Python's dict with the configuraiton parameters
        algo_active (str): The name of the algorithm

    Returns:
        init_train_date (datetime): The init date for the training dataset
        end_train_date (date): The end date for the training dataset
        init_forecast_date (date): The init date for the testing dataset
        end_forecast_date (date): The end date for the testing dataset
    """

    # generate sliding windows
    init_train_date = datetime.strptime(cfg['range_date'][0], '%Y-%m-%d %H:%M:%S')
    end_train_date = init_train_date + timedelta(hours=algo_active['hours_in_train'])

    init_forecast_date = init_train_date + timedelta(hours=algo_active['hours_in_train'])
    end_forecast_date = init_train_date + timedelta(hours=algo_active['hours_in_train']) + timedelta(hours=algo_active['hours_in_train'])

    return init_train_date, end_train_date, init_forecast_date, end_forecast_date


def get_total_trainsets(df_anual_data):
    """
    Check the number of training sets to be generated
    :return:
    """

    months = df_anual_data.FECHA.dt.month.unique()
    segments = df_anual_data.ID_SEGMENT.unique()
    codes = df_anual_data.COD_LABORALIDAD.unique()

    print("Months " + str(months))
    print("Segments " + str(segments))
    print("Codes " + str(codes))

    total_trainsets = len(months) * len(segments) * len(codes)
    print("Number of training sets: " + str(total_trainsets))


def get_total_trainsets(df_anual_data, segments):
    """
    # Fill the training_sets dict
    :param df_anual_data:
    :return:
    """
    rows_per_day = int(((60 / 15) * 24))

    training_sets = {'ID_SEGMENT': [], 'MES': [], 'COD_LABORALIDAD': [], 'TRAINING_SET': []}
    for seg_id in segments:  # 1) Particionar anual_data por segmento
        df_seg = df_anual_data.loc[df_anual_data.ID_SEGMENT == seg_id]
        for month_i in df_seg.FECHA.dt.month.unique():  # 2) Dividir mensual_data en 12 datasets
            df_month_seg = df_seg.loc[df_seg.FECHA.dt.month == month_i]
            for code_i in df_month_seg.COD_LABORALIDAD.unique():  # 3) Particionar por dias con mismo código de lab
                df_month_seg_code = df_month_seg.loc[df_month_seg.COD_LABORALIDAD == code_i]

                # Fill training_sets dictionary

                training_sets['ID_SEGMENT'].append(seg_id)
                training_sets['MES'].append(month_i)
                training_sets['COD_LABORALIDAD'].append(code_i)
                training_sets['TRAINING_SET'].append(df_month_seg_code)

    return training_sets

def check_trainingsets(training_sets, total_trainsets):
    """

    :return:
    """
    for row in range(0, total_trainsets):
        print("----------------------------> TRAINING SET NUMBER: " + str(row))
        print("SEGMENTO " + str(training_sets['ID_SEGMENT'][row]))
        print("MES " + str(training_sets['MES'][row]))
        print("COD_LABORALIDAD " + str(training_sets['COD_LABORALIDAD'][row]))
        print("DAYS IN TRAINING_SET: " + str(training_sets['TRAINING_SET'][row].FECHA.dt.day.unique()))