import serial
import json
import logging
from datetime import datetime

pull = []
config = {}
ser = 0
file_pull = 'pull.log'
file_all = 'all.log'

def SerialParser(config):
    try:
        input_mass = ser.readline()
    except Exception as e:
        logging.warning("Ошибка чтения с порта:")
        logging.warning(e)
        return False

    try:
        input_mass_pars = input_mass[4: -1].decode('utf-8').split(' ')
    except Exception as e:
        logging.warning("Ошибка декодирования:")
        logging.warning(e)
        return False
        
    len_input_mass_pars = len(input_mass_pars)
    output_str = ''

    for byte in input_mass_pars:
        len_input_mass_pars -= 1
        zero = "0" if len(byte) == 1 else ""
        space = "  " if len_input_mass_pars else ""
        output_str += zero + byte + space

    if config['logger_all']:
        print_in_file(file_all, output_str)
    return output_str


def komporator(config , enclosed = False):

    # Инициализация статической переменной, для хранения предыдущего сообщения
    if not hasattr(komporator, 'old_str'):
        komporator.old_str = ''

    # Читаем сообщение
    data_str = SerialParser(config)
    if not data_str:
        return

    # Если сообщения нет в пуле
    if(pull.count(data_str) == 0):
        # Выводим предыдущее сообщение
        if not enclosed and komporator.old_str:
            logging.info("OLD " + komporator.old_str)
        
        # Если стоит параметр logger_pull пишем в лог
        if config['logger_pull']:
            print_in_file(file_pull, data_str)

        # Добавляем в пул
        pull.append(data_str)

        # Обновляем предыдущую строку 
        komporator.old_str = data_str

        # Выводим сообщение
        logging.info("NEW " + data_str)

        # Зпускаем следующую интерацию 
        komporator(config, True)
    else:

        # Если есть в пуле то выводим его как следующее
        # при условии что мы в рекурсии
        if enclosed:
            logging.info("FUT " + data_str)
            logging.info("-" * 20 + "\n")
        komporator.old_str = data_str
        

def init_port(config):
    try:
        return serial.Serial (
            port = config['port'],
            baudrate = config["speed"],
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS
        )
    except Exception as e:
        logging.error('Ошибка инициализации порта:')
        logging.error(e)
        exit(1)
    else:
        logging.info('Подключение установлено')


def print_in_file(file_name, data):
    try:
        with open(file_name, 'a', encoding='utf-8') as fh:
            fh.write(data + '\n')
    except Exception as e:
        logging.warning('Ошибка записи в: ' + file_name)

def init_cofig(name):
    config = {}
    try:
        with open(name, 'r', encoding='utf-8') as fh:
            config = json.load(fh)
    except Exception as e:
        print('Ошибка чтения конфигурации:')
        print(e)
        exit(1)
    else:
        print('Конфигурация получена')
        return config


def autopull(_pull, filename = 'autopull.txt'):
    logging.info('Загрузка автопула')
    try:
        f = open(filename)
        for line in f.readlines():
            if pull.count(line) == 0:
                _pull.append(line.replace('\n', ""))
    except Exception as e:
        logging.error('Ошибка чтения файла автопула')
        logging.error(e)
        _pull.clear()
    else:
        if _pull:
            logging.info('Успешное чтение. Добавленно %s записей.' % len(pull))
        else:
            logging.warning('Ничего не добавленно, вероятно файл %s пуст' % filename)
    finally:
        f.close()


def init_loggs(configб):
    try:
        logging.basicConfig(format = u'%(levelname)-8s [%(asctime)s] %(message)s', level = logging.DEBUG)
        time = datetime.now().strftime("\n--- %d-%m-%y %H:%M ---")
        if config['logger_all']:
            print_in_file(file_all, time)
        if config['logger_pull']:
            print_in_file(file_pull, time)
    except Exception as e:
        print('Ошибка инициализации логирования:')
        print(e)
        exit(1)
    else:
        logging.info('Логирование успешно инициализировано')


config = init_cofig('config.json')
init_loggs(config)
ser = init_port(config)

if config['auto_pull']:
    autopull(pull)


while 1 :
    komporator(config)

