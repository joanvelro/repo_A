import os, sys
import logging
from sklearn.externals import joblib
import pandas as pd
import pickle

def save_model(algo_name, model, cfg):
    """ This method transforms the Spark's dataframe to Pandas to be stored in a Python dict and save them into disk

    Args:
        algo_name (str): The name of the algorithm to be save

    Returns:
        bool: The return value. True for success, False otherwise.

    """
    pickle_tgt = algo_name + ".pkl" # it will be saved in pkl extension
    model_path = os.path.dirname(os.path.abspath(__file__)).split("src")[:-1] # get top level dir
    model_path = os.path.join(model_path[0], 'models', cfg['global']['project_name'])
    if not os.path.exists(model_path):
        os.makedirs(model_path)

    model_file = os.path.join(model_path, pickle_tgt)
    try:
        joblib.dump(model, model_file, compress=3) # pickle it
        return True
    except Exception as e:
        logging.error(" The model " + algo_name + " cannot be saved: " + str(e))
        return False

    logging.info(" The models have been saved to disk")

def load_model(algo_name, code,  segment_id, cfg, descriptive_model=True, ml_model=True):
    """ This method transforms the Spark's dataframe to Pandas to be stored in a Python dict and save them into disk

    Args:
        algo_name (str): The name of the algorithm to be loaded

    Returns:
        bool: The return value. True for success, False otherwise.

    """
    '''
    if 'desc_ref' not in algo_name:
        pickle_tgt = algo_name + ".pkl" # load the model up to create predictions
        model_path = os.path.dirname(os.path.abspath(__file__)).split("src")[:-1] # get top level dir
        model_path = os.path.join(model_path[0], 'models', cfg['global']['project_name'], pickle_tgt)
        try:
            model_loaded = joblib.load(model_path)
        except Exception as e:
            logging.error(" The model " + algo_name + " cannot be loaded: " + str(e))
            sys.exit(0)
    else:
    '''
    project_model_path = os.path.dirname(os.path.abspath(__file__)).split("src")[:-1]  # get top level dir
    project_model_path = os.path.join(project_model_path[0], 'models', cfg['global']['project_name'],cfg['global']['project_scope'])
    is_loaded = False

    if descriptive_model==True or 'desc_ref' in algo_name:
        if 'desc_ref' in algo_name:
            model_folder = os.path.join('desc_ref', 'segment')
            model_loaded = pd.read_csv(os.path.join(project_model_path, model_folder, str(segment_id)  + "_.csv"), delimiter=";", parse_dates=['FECHA'])  # TODO: load by segment
            is_loaded=True
        elif 'arima' in algo_name:
            model_folder = os.path.join('sarimax', 'segment')
            model_loaded = pd.read_csv(os.path.join(project_model_path, model_folder, str(segment_id) + "_sarimax.csv"), delimiter=";")
            is_loaded = True
        elif 'ets' in algo_name:
            model_folder = os.path.join('ets', 'segment')
            model_loaded = pd.read_csv(os.path.join(project_model_path, model_folder, str(segment_id) + "_exp_smooth.csv"),delimiter=";")
            is_loaded = True
        elif 'fbprophet' in algo_name:
            model_folder = os.path.join('fbprophet', 'segment')
            model_loaded = pd.read_csv(os.path.join(project_model_path, model_folder, str(segment_id) + "_fbprophet.csv"),delimiter=";")
            is_loaded = True
        elif 'tbats' in algo_name:
            model_folder = os.path.join('tbats', 'segment')
            model_loaded = pd.read_csv(os.path.join(project_model_path, model_folder, str(segment_id) + "_tbats.csv"),delimiter=";")
            is_loaded = True
        elif 'minsait' in algo_name:
            model_loaded = None
            is_loaded = True

    if ml_model==True and 'desc_ref' not in algo_name and not is_loaded:
        model_name = algo_name.split("__")[1]
        project_model_path =  os.path.join(project_model_path, 'machine_learning')

        try:
            model_loaded = dict()

            for i_model_horizon in ['15', '60', '120']:
                model_folder = os.path.join(project_model_path, str(model_name) + '-' + str(code), str(model_name) + '_' + str(segment_id) + '_' + str(i_model_horizon) + ".sav")
                if cfg['global']['project_scope'] == 'AUSOL':  # other convention is used here
                    segment_id = int(segment_id)
                else:
                    segment_id = float(segment_id)

                if 'extra_trees_2a' == model_name:
                    if cfg['global']['project_scope']=='AUSOL': # other convention is used here
                        model_folder = os.path.join(project_model_path, str(model_name.split('s_2a')[0]) + '_' + str(i_model_horizon) + '_seg_' + str(segment_id) + '_cod_' + str(code) + ".sav")

                    with open(model_folder, 'rb') as f:
                        model_loaded[str(i_model_horizon)] = pickle.load(f, encoding='latin1')
                else:
                    model_loaded[str(i_model_horizon)] = joblib.load(model_folder)



        except Exception as e:
            logging.error(" The model " + model_name + " cannot be loaded: " + str(e))
            sys.exit(0)


    logging.debug(" The model " + algo_name + " has been loaded succesfully")
    return model_loaded
