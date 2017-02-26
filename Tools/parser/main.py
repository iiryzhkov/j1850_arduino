import serial

def SerialParser(input_mass = {}):

    if len(input_mass) < 1:
        input_mass = ser.readline()

    input_mass_pars = input_mass[4: -1].decode('utf-8').split(' ')
    len_input_mass_pars = len(input_mass_pars)
    output_str = ''

    for byte in input_mass_pars:
        len_input_mass_pars -= 1
        zero = "0" if len(byte) == 1 else ""
        space = "  " if len_input_mass_pars else ""
        output_str += zero + byte + space

    return output_str

def komporator(line_data, new, old_str):
    if(pull.count(line_data) < 1):
        if not new:
           print(old_str)
        pull.append(line_data)
        print("NEW " + line_data)

        return komporator(SerialParser(), True, old_str)
    else:
        if new:
            print(line_data)
            print('\n')
        return False

pull = []
old_str = " "
is_new = True

ser = serial.Serial (
    port='COM5',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

while 1 :

    _str = SerialParser()
    is_new = komporator(_str, is_new, old_str)
    old_str = _str
