"""
keysight_kt33000 Python API Example Program

Creates a driver object, reads a few DriverIdentity interface properties, and checks
the instrument error queue.  May include additional instrument specific functionality.

Runs in simulation mode without an instrument.

Requires Python 3.6 or newer and keysight_kt33000 Python module installed.
"""

import keysight_kt33000
import serial
import serial.tools.list_ports
import numpy as np
import sys
import os
import yaml
import logging
import time

from Crypto.Cipher import AES
from keysight_kt33000 import FunctionShape
from keysight_kt33000 import BurstGatePolarity
from keysight_kt33000 import TriggerSource, TriggerSlope
from datetime import timedelta

USAGE = '''
Usage:
    python dfa_aes.py [options]

Options:
    -h, --help              this help message.
    -p, --port              serial port
    -b, --baudrate          serial baudrate
    -i, --iteration         command iteration
    -c, --config            configuration file    
    -s, --simulatre         simulate Kt33000 driver
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

    def send_command(self, data):
        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()
        self.send(data)
        return self.read()


def set_trigger(source: TriggerSource, slope: TriggerSlope, delay: timedelta):
    """
    """
    driver.output_channels[0].trigger.source = source
    driver.output_channels[0].trigger.slope = slope
    driver.output_channels[0].trigger.delay = delay

def set_delay(delay: timedelta):
    """
    """
    driver.output_channels[0].trigger.delay = delay 

def set_pulse(frequency: float, amplitude: float, offset: float, width:float):
    """
    """
    try:
        driver.output_channels[0].output.voltage.offset.dc_offset = offset
        driver.output_channels[0].output.voltage.amplitude = amplitude
        driver.output_channels[0].output.frequency = frequency
        driver.output_channels[0].output_function.function = FunctionShape.PULSE
        driver.output_channels[0].output_function.pulse.width=width
    except Exception as e:
        print("\n  Exception:", e.__class__.__name__, e.args)   

def set_pulse_width(value: float):
    """
    """
    driver.output_channels[0].output_function.pulse.width=value

def set_burst(enabled: bool):
    driver.output_channels[0].burst.enabled = enabled

def set_output(enabled: bool):
    driver.output_channels[0].output.enabled = enabled

def main(argv=None):
    """
    Edit resource_name and options as needed.  resource_name is ignored if option Simulate=true
    For this example, resource_name may be a VISA address(e.g. "TCPIP0::<IP_Address>::INSTR")
    or a VISA alias.  For more information on using VISA aliases, refer to the Keysight IO
    Libraries Connection Expert documentation.
    """
    import getopt

    if argv is None:
        argv = sys.argv[1:]
    try:
        opts, args = getopt.gnu_getopt(argv, 'hp:b:i:c:s:', ['help', 'port=', 'baudrate=', 'config=', 'iteration=', 'simulate=', 'logging='])
        port = '/dev/ttyACM0'
        baudrate = None
        iteration = 1
        simulation = True
        config = "dfa_aes.yaml"
        logfile = False
        
        for o, a in opts:
            if o in ('-h', '--help'):
                print(USAGE)
                return 0
            elif o in ('-p', '--port'):
                port = a
            elif o in ('-b', '--baudrate'):
                baudrate = a
            elif o in ('-i', '--iteration'):
                iteration = a
            elif o in ('-c', '--config'):
                config = a
            elif o in ('-s', '--simulate'):
                simulation = True
            elif o in ('-l', '--logging'):
                logfile = True   
                             
    except getopt.GetoptError:
        e = sys.exc_info()[1]     # current exception
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write(USAGE+"\n")
        return 1

    if baudrate is None:
        baudrate = "115200"

    # get script path
    current_dir = os.path.dirname(os.path.realpath(__file__))
    # open yaml file
    config = os.path.join(current_dir,config)
    try:
        with open(config, 'r') as file:
            cfg = yaml.safe_load(file)
    except FileNotFoundError as e:
        print(f"Configuration file opening error: {e}")
        return -1
    
    # create log file
    log = logging.getLogger(__name__)
    log.setLevel(logging.INFO)

    # log file name is composed of configuation file name and a timestamp.
    if logfile:
        stamp = "%d" % time.time()
        name = os.path.splitext(config)[0] + "_" + stamp + ".log"
        file = logging.FileHandler(name)
        fileformat = logging.Formatter("%(asctime)s:%(levelname)s:%(message)s")
        file.setFormatter(fileformat)
        log.addHandler(file)

    stream = logging.StreamHandler()
    streamformat = logging.Formatter("%(asctime)s:%(levelname)s:%(message)s")
    stream.setFormatter(streamformat)
    log.addHandler(stream)

    resource_name_wave = "USB0::2391::11015::MY59000625::0::INSTR"
    resource_name = resource_name_wave
    #  Edit the initialization options as needed
    idQuery = True
    reset   = True
    options = f"QueryInstrStatus=False, Simulate={simulation}, Trace=False"

    try:
        print("\n  keysight_kt33000 Python API Example1\n")

        # Call driver constructor with options
        global driver # May be used in other functions
        driver = None
        driver = keysight_kt33000.Kt33000(resource_name, idQuery, reset, options)
        print("Driver Initialized")

        #  Print a few identity properties
        print('  identifier:    ', driver.identity.identifier)
        print('  revision:      ', driver.identity.revision)
        print('  vendor:        ', driver.identity.vendor)
        print('  manufacturer:  ', driver.identity.instrument_manufacturer)
        # output off
        set_output(False)

        # burst on
        set_burst(True)
        # trigger external on rising edge, set delay to zero by default
        set_trigger(source=TriggerSource.EXTERNAL, slope=TriggerSlope.POSITIVE, delay=timedelta(0))

        # pulse settings
        set_pulse(frequency=1000, amplitude=3, offset=1.5, width=0.000000250)

        # output on
        set_output(True)
        set_delay(0.0)
        # Check instrument for errors
        while True:
            outVal = ()
            outVal = driver.utility.error_query()
            print("  error_query: code:", outVal[0], " message:", outVal[1])
            if(outVal[0] == 0): # 0 = No error, error queue empty
                break

        # get reference
        reference = f"{AES.new(bytes.fromhex(cfg['key']), AES.MODE_ECB).encrypt(bytes.fromhex(cfg['plain'])).hex().upper()}"

        input("press a key to start scenario...")
        
        serial = Link()
        if serial.open(port=port, baudrate=int(baudrate)):
            for j in np.arange(cfg['width']['min'],cfg['width']['max'], cfg['width']['inc']):
                set_pulse_width(j)
                for i in np.arange(cfg['delay']['min'],cfg['delay']['max'],cfg['delay']['inc']):
                    set_delay(i)
                    for k in range(0,int(iteration)):
                        resp = serial.send_command("FE8A02000020" + cfg['key'] + cfg['plain'])
                        if resp.find(reference)!=-1: status="success"
                        else: status = "fail"
                        if resp != "": 
                            log.info(f"{resp.strip()}, width={j}, delay={i}, iteration={k + 1}, result={status}")
                        resp = ""
            serial.close()
            
    except KeyboardInterrupt as e:
        print("\n  Exception:", e.__class__.__name__, e.args)
        sys.exit(-1)
    except Exception as e:
        print("\n  Exception:", e.__class__.__name__, e.args)
     
    finally:
        if driver is not None: # Skip close() if constructor failed
            driver.close()

if __name__ == '__main__':
    sys.exit(main())
