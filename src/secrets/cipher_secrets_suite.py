import os
import sys
import yaml

import subprocess
from cryptography.fernet import Fernet

secrets_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),'secrets.yaml')

def decrypt_secrets(cfg):
    with open(os.path.join(cfg['global']['fernet_tokens_path'],'fernet_tokens.txt'), 'r') as token_file:
        master_key = token_file.read()

    with open(secrets_file_path, 'r') as ymlfile:
        secrets_file = yaml.load(ymlfile)
        cipher_suite = Fernet(str.encode(master_key))
        for password_key in secrets_file:
            decrypted_password= (cipher_suite.decrypt(secrets_file[password_key]))
            cfg[str(password_key)]=decrypted_password.decode("utf-8")
    return cfg

def generate_secrets_file(cfg):
    master_key = Fernet.generate_key()
    with open(os.path.join(cfg['global']['fernet_tokens_path'],'fernet_tokens.txt'), 'w') as token_file:
        token_file.write(master_key.decode())

    logging.info('Master key has been generated and stored in: ' + os.path.join(cfg['global']['fernet_tokens_path']))

    with open(secrets_file_path, 'r') as ymlfile:
        secrets_file = yaml.load(ymlfile)
        for password_key in secrets_file:
            password_raw_value = input("Enter the password for " + str(password_key) + " :" + "\n")
            cfg[str(password_key)]=password_raw_value
            cipher_suite = Fernet(master_key)
            ciphered_text = cipher_suite.encrypt(str.encode(password_raw_value))  # required to be bytes
            secrets_file[password_key]=ciphered_text
            with open(secrets_file_path, 'w') as f:
                yaml.dump(secrets_file, f)

    return cfg






