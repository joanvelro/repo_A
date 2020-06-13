from dataset_utils.sql_data_retrieval import horusdb_sql_queries

def generate_datasets(datasets, datasets_groups, time_range, datasets_path, sqlserver_password, save_to_disk=True):
    """ This method transforms the Spark's dataframe to Pandas to be stored in a Python dict and save them into disk

    Args:
        datasets (str): The Python dict to store the datastes
        datasets_groups (str): These are the keys of the datasets dict
        time_range (tuple): The time range of the datasets
        datasets_path (str): The datasets path
        sqlserver_password: The password used to query the database. TODO: parametrize the database also
        save_to_disk: If True the datasets will be stored into disk as csv file for later use

    Returns:
        dict: The Python's dict with the generated datasets

    """
    # Generate the input dataset
    datasets_original = horusdb_sql_queries.generate_vehicles_dataset(time_range, sqlserver_password)
    datasets['input'] = datasets_original.toPandas()

    # Generate the result dataset
    #datasets['results'] = horusdb_sql_queries.generate_result_dataset(time_range, sqlserver_password).toPandas()

    if save_to_disk: # Save data to disk
        for grp in datasets_groups:
            with open(datasets_path + str(grp) + "_vehicles_dataset.csv", "a+") as f:
                datasets[grp].to_csv(f, sep=';', index=False, line_terminator='\n')
    else:
        return datasets

