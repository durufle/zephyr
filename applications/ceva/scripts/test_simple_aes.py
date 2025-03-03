#!/usr/bin/env python3

import sys
import time
import serial
import serial.tools.list_ports

USAGE = '''
Usage:
    python test_commands.py [options]

Options:
    -h, --help              this help message.
    -p, --port              serial port
    -b, --baudrate          serial baudrate
    -d, --delay             delay after channel opening 
    -i, --iteration              iteration loop      
'''


class Link:
    def __init__(self):
        self.serial = serial.Serial()

    @staticmethod
    def show_list():
        ports = serial.tools.list_ports.comports()
        for port in ports:
            print(f"-> port: {port}")

    def open(self, port, timeout=0.100, baudrate=115200):
        self.serial.port = port
        self.serial.baudrate = baudrate
        self.serial.parity = serial.PARITY_NONE
        self.serial.stopbits = serial.STOPBITS_ONE
        self.serial.timeout = timeout
        try:
            self.serial.open()
            return self.serial.is_open
        except serial.SerialException as e:
            sys.stderr.write(f"-> error : {e}")
            self.show_list()
            sys.exit(-1)

    def close(self):
        try:
            self.serial.close()
            return self.serial.is_open
        except serial.SerialException as e:
            sys.stderr.write(f"-> error : {e}")
            sys.exit(-1)

    def send(self, data):
        try:
            data = data + '\n'
            self.serial.write( data.encode('utf-8'))
        except serial.SerialException as e:
            sys.stderr.write(f"-> error : {e}")

    def read(self):
        try:
            return self.serial.readline().decode()
        except serial.SerialException as e:
            sys.stderr.write(f"-> error : {e}")

    def send_received(self, data):
        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()
        self.send(data)
        return self.read()

    def send_command(self, header):
        result = self.send_received(header)
        return result


def main(argv=None):
    import getopt

    if argv is None:
        argv = sys.argv[1:]
    try:
        opts, args = getopt.gnu_getopt(argv, 'hp:b:i:d:', ['help', 'port=', 'baudrate=', 'iteration=', 'delay='])
        port = '/dev/ttyACM0'
        baudrate = None
        iteration = 1
        delay = 0.3

        for o, a in opts:
            if o in ('-h', '--help'):
                print(USAGE)
                return 0
            elif o in ('-p', '--port'):
                port = a
            elif o in ('-b', '--baudrate'):
                baudrate = a
            elif o in ('-d', '--delay'):
                delay = a
            elif o in ('-i', '--iteration'):
                iteration = a
    except getopt.GetoptError:
        e = sys.exc_info()[1]     # current exception
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write(USAGE+"\n")
        return 1

    if baudrate is None:
        baudrate = "115200"

    com = Link()

    if com.open(port=port, baudrate=int(baudrate)):
        # Wait some time . Necessary for arduino for example
        time.sleep(float(delay))
        print("\r\nGet version...")
        print("-> {0}".format(com.send_command("FE8000000000")))
        plain = "0102030405060708090A0B0C0D0E0F10"
        key = "2B7E151628AED2A6ABF7158809CF4F3C"
        # Execute simple aes iteration  
        for i in  range(0,int(iteration)):
            print(f"Test simply aes....{i + 1}")
            print("-> {0}".format(com.send_command("FE8A02000020" + key + plain)))
         
        com.close()


if __name__ == '__main__':
    sys.exit(main())
